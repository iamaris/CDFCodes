//------------------------------------------------------------------------
// Description:
//	Class Hepevt2HepgModule
//
// this module fills /HEPEVT/ from binary/ascii file and
// and writes HEPG bank from /HEPEVT/ common block in an STDHEP file
//
//  Sebastian Carron (Duke),Elena Gershtein (CMU), Stan Thompson (Glascow)
//
// CEV20110217    Modified by Oksana Tadevosyan to read list of files
//                for generation mode 3
//
// August 9 2011  Modified by Peter Dong with a method allows users to
//                change uncooperative particle codes with a talk-to.
//------------------------------------------------------------------------


// String Header First  --
//------------------------
#include <string>
using std::string;
#include <string.h>

//-----------------------
// This Class's Header --
//-----------------------
#include "Hepevt2HepgModule.hh"

//---------------
// C++ Headers -- MUST use defects !!!!
//---------------
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
using std::cout;
using std::endl;
using std::setw;
#include "Edm/Handle.hh"
#include "Edm/ConstHandle.hh"
#include "Edm/EventRecord.hh"
#include "SimulationObjects/HEPG_StorableBank.hh"

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "AbsEnv/AbsEnv.hh"

#include "stdhep_i/CdfHepevt.hh"



//------------------------------------------
// Fortran routines called from this code --
//--------------------------------------------
extern "C" {
  int hepfil_(const char*,
	      int*,int*,int*,
               int); // lengths of character strings (value, not reference)  
  int hepred_(int*,int*);
  int hepcls_(int*);
}

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

const char* Hepevt2HepgModule::genId="StdhepInput";

//----------------
// Constructors --
//----------------

Hepevt2HepgModule::Hepevt2HepgModule() : 
  AbsGenModule( Hepevt2HepgModule::genId, 
		"AC++ Stdhep Input module"),
  _stdhepFile("StdhepFile",this,""),
  _firstlistevent("Listfirst",    this, 1),
  _lastlistevent ("Listlast",    this, 1),
  _eventlistlevel("Evlistlevel",    this, 1),
  _initlistlevel("Inlistlevel",    this, 11),
  _eventStart("Eventstart",    this, 1),
  _eventEnd("Eventend",    this, 1),
  _inputFileList("InputFileList",    this, 0, 10),
  _particleIDChanges("ParticleIDChanges", this, "")
{
  
  _firstlistevent.addDescription(
  "      \t\t\tFirst event to list");  
  _lastlistevent.addDescription(
  "      \t\t\tLast event to list");  
  _eventlistlevel.addDescription(
  "      \t\t\tEvent listing level");  
  _initlistlevel.addDescription(
  "      \t\t\tLevel of Particle information to be given at run start"); 
  _stdhepFile.addDescription(
  "      \t\t\tFile containing events consisting of dump of  HEPEVT common"); 
 _eventStart.addDescription(
  "      \t\t\tFirst event to process");  
  _eventEnd.addDescription(
  "      \t\t\tLast event to process");
  _inputFileList.addDescription(
  "      \t\t\tList of files to process");
  _particleIDChanges.addDescription(
  "      \t\t\tA command that allows modification of particle ID numbers\
      \t\t\tin the event of unexpected behavior from MC generators.\
      \t\t\tThis is given by a single long string with different entries\
      \t\t\tseparated by colons.\
      \t\t\tFor example, to replace all instances of particle 81 with 21\
      \t\t\t(a photon) and all instances of particle 82 with 6 (a top\
      \t\t\tquark), type: \"ParticleIDChanges set 81-21:82-6\".");
  commands()->append(&_firstlistevent);
  commands()->append(&_lastlistevent);
  commands()->append(&_eventlistlevel);
  commands()->append(&_initlistlevel);
  commands()->append(&_stdhepFile); 
  commands()->append(&_eventStart);
  commands()->append(&_eventEnd);
  commands()->append(&_inputFileList);
  commands()->append(&_particleIDChanges);  
}

//--------------
// Destructor --
//--------------

Hepevt2HepgModule::~Hepevt2HepgModule()
{
}

AppResult Hepevt2HepgModule::genBeginRun(AbsEvent* aRun) {
  return AppResult::OK;
}

AppResult Hepevt2HepgModule::genBeginJob() {

   // Get the Input file:

   //  const string& fileName;
   // oksana, looping over all input files
   for (AbsParmList<std::string>::ConstIterator
           name=_inputFileList.begin();
        name!=_inputFileList.end(); name++)
   {
      std::cout << "   " << *name << std::endl;
   }

   _currentFile = _inputFileList.begin();
   const string& fileName = (*_currentFile);

   if (fileName.empty()) {
      ERRLOG(ELfatal,"Hepevt2HepgModule: No file specified")
         << "@SUB=genBeginJob:No filename given"
         << endmsg;
      return AppResult::ERROR;}
   else {
      _istr=11;
      _events = 1;
      _endoffile=0;
      if (hepfil_(fileName.c_str(),&_istr,&_events,&_endoffile,fileName.size())) 
      {if(_endoffile==1)
         {ofstream out("endfile", std::ios::out | std::ios::binary);
            out.put((char) 1);
            out.close();
            ERRLOG(ELfatal,"Hepevt2Hepg error: STDHEP file has fewer events than your initial event number!!!. endfile file has been created.")
               << "@SUB=genBeginJob" << endmsg;
            return AppResult::ERROR;
         }   
         ERRLOG(ELfatal,"Hepevt2Hepg error reading file")
            << "@SUB=genBeginJob" << endmsg;
         return AppResult::ERROR;
      }
   }
   
// Fill the map for particle ID
   if (_particleIDChanges.value().size() != 0)
   {
      std::istringstream str(_particleIDChanges.value());
      while (str)
      {
         int original, changed;
         str >> original;
// Note that any non-numeric delimeter of exactly one character will be fine
         str.get();
         str >> changed;
         if (str)
         {
            _particleIDMap.insert(std::make_pair(original, changed));
         }
         else
         {
            ERRLOG(ELsevere2,
                   ":StdhepInput Module: Bad pattern passed to ParticleIDChanges")
               << "@SUB=genBeginJob" << endmsg;
         }
         str.get();
      }
   }
// pdong 8/2011 

   return AppResult::OK;
}

int Hepevt2HepgModule::callGenerator(AbsEvent* anEvent) {
  

   CdfHepevt* _cdfhepevt = CdfHepevt::Instance();

  // Read the file
   _endoffile=0;

   if(hepred_(&_istr,&_endoffile)) {
        ERRLOG(ELabort,":StdhepInput Module:******* Error reading file: Bad Event!!***************")
	  <<"@SUB=callGenerator"<< endmsg;
      return 1;
  }

   if(_endoffile==1)
    {
      if (_currentFile== _inputFileList.end()){
     ofstream out("endfile", std::ios::out | std::ios::binary);
     out.put((char) 1);
     out.close();
     ERRLOG(ELabort,":StdhepInput Module: YOU HAVE REACHED THE END OF THE FILE, CORE DUMP WILL HAPPEN BUT YOUR OUTPUT WILL BE FINE - endfile file has been created")
       <<"@SUB=callGenerator"<<endmsg;
    return 1;
      }
      // oksana, close this file, and open next
      hepcls_(&_istr);
      _currentFile++;
      const string& fileName = (*_currentFile);
      int nev=1;
      hepfil_(fileName.c_str(),&_istr,&nev,&_endoffile,fileName.size());
    }
 

//   cout<<"my counter="<<_events<<"\n";
  //_cdfhepevt->common2list();
  // See if we are supposed to list this event  
//  int mevlist=_eventlistlevel.value();
//  if ( (_events >= _firstlistevent.value() && 
//       _events <= _lastlistevent.value()) || !(_events%100)) {
//    cout << " StdHep event no. " << _events << endl;   
//  }

  setPassed(false); 

  // oksana, check if event number is above threshold we want
  if  (_events >= _eventStart.value()) {  
      setPassed(true); 

//    cout<<" stripped event number "<<_events<<"\n";
   for ( std::list<Hepevt*>::iterator i =  _cdfhepevt->contentHepevt().begin();
	 i !=  _cdfhepevt->contentHepevt().end(); i++ ) {

     // make HEPG from hepevt;
     HEPG_StorableBank* hepg = (*i)->makeHEPG(anEvent);
     if (!hepg || hepg->is_invalid()) {
       ERRLOG(ELsevere2,"[GEN_BAD_HEPG]")
         << "Hepevt2HepgModule: can't create a valid "
         << "HEPG_StorableBank"
         << endmsg;
         return AppResult::OK;
     }

// Changes particle IDs according to the input parameter given.
     if (!_particleIDMap.empty())
     {
        for(HEPG_StorableBank::particleIter it(*hepg); it.is_valid(); ++it)
        {
           int id = it.idhep();
           std::map<int, int>::const_iterator mapit = _particleIDMap.find(id);
           if (mapit != _particleIDMap.end())
           {
           bool response = 
              hepg->set_idhep(it.index(), mapit->second);

              if (!response)
              {
                 ERRLOG(ELsevere2,"[GEN_BAD_HEPG]")
                    << "Hepevt2HepgModule: can't change the HEPG bank "
                    << endmsg;
                 return AppResult::OK;
              }
           }           
        }
     }
// pdong 8/2011

     Handle<HEPG_StorableBank> h(hepg);
     if ((anEvent->append(h)).is_null()) {
       ERRLOG(ELsevere2,"[GEN_BAD_HEPG]")
         << "Hepevt2HepgModule: can't add"
         << "HEPG_StorableBank to the event."
         << endmsg;
       return AppResult::OK;
     }

     //_cdfhepevt->clearCommon(); // overkill...
   }

  }

   // Update our event counter 
   _events++;

  _cdfhepevt->clear();

  return AppResult::OK; 

}

void Hepevt2HepgModule::fillHepevt() {
  //  doesn't do much HEPEVT is already filled
  //CdfHepevt* cdfhepevt = CdfHepevt::Instance();
  //Hepevt_t* hepevt = cdfhepevt->hepevtprt();
  //hepevt->NEVHEP = _events; // manually set event counter
  //cdfhepevt->common2list(); 
  _hepevt.nevhep()=_events;  
}

AppResult Hepevt2HepgModule::genEndRun(AbsEvent* aRun) {

  hepcls_(&_istr);

  return AppResult::OK;
}

AppResult Hepevt2HepgModule::genEndJob() {

  hepcls_(&_istr);

  return AppResult::OK;
}
