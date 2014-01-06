//
// rev.  0.0
//

// String Header First  --
//------------------------
#include <string>
using std::string;
#include <string.h>

//-----------------------
// This Class's Header --
//-----------------------
#include "generatorMods/AriadneModule.hh"
#include "pythia_i/Ariadne.hh"
#include "stdhep_i/CdfHepevt.hh"
#include "pythia_i/Pythia.hh"

//---------------
// C++ Headers -- MUST use defects !!!!
//---------------
#include <iostream>
#include <iomanip>
using std::cout;
using std::endl;
using std::setw;

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "AbsEnv/AbsEnv.hh"
#include "stdhep_i/CdfHepevt.hh"

// Access to STDHEP LUNs
//#include "stdlun.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

const char* AriadneModule::genId="Ariadne";

//----------------
// Constructors --
//----------------

AriadneModule::AriadneModule() : 
  AbsGenModule( AriadneModule::genId, 
		"AC++ Ariadne module"),
  _initMode("associateProgram",this,"PYTHIA"),
  _tune("parameterTune",this,"4.12"),
  _popaTotsig("poweronTotSigma",this,2.3),
  _popaPpow("pomeronExponent",this,2.),
  _setMsta("set_msta",this),
  _setPara("set_para",this),
  _setPqmas("set_pqmas",this),
  _setCa("set_ca",this),
  _setPb("set_pb",this),
  _setCf("set_cf",this),
  _setXa("set_xa",this),
  _setNb("set_nb",this),
  _pythiaMenu( new PythiaMenu( this, 0, "PythiaMenu") )
{
  _initializeTalkTo();
}

//--------------
// Destructor --
//--------------

AriadneModule::~AriadneModule()
{
  delete _pythiaMenu;
}

AppResult AriadneModule::genBeginRun(AbsEvent* aRun) {
  return AppResult::OK;
}
void AriadneModule::_initializeTalkTo() {

   // setup commands
   commands()->append(&_initMode);
   commands()->append(&_tune);
   commands()->append(&_popaTotsig);
   commands()->append(&_popaPpow);
   // commons parameters   
   commands()->append(&_setMsta);   
   commands()->append(&_setPara);   
   commands()->append(&_setPqmas);   
   commands()->append(&_setCa);      
   commands()->append(&_setPb);    
   commands()->append(&_setCf);  
   commands()->append(&_setXa);
   commands()->append(&_setNb);
   // pythia setup 
   _pythiaMenu->initialize("PythiaMenu",this);
   _pythiaMenu->initTitle( " Pythia Menu " );
   commands()->append( _pythiaMenu );

   // add descriptions
 _initMode.addDescription("      \t\t\tProgram to be used for the generation[PYTHIA]");
 _tune.addDescription("      \t\t\tTuned parameter set to be used[4.12]");
 _popaTotsig.addDescription("      \t\t\tTot sigma for pomeron structure function[2.3]"); 
 _popaPpow.addDescription("      \t\t\tExponent for pomeron structure function[2.]");  
}
AppResult AriadneModule::genBeginJob() {

  Ariadne* ariadne = Ariadne::Instance();
  Pythia* pythia = Pythia::Instance(); 

  // setup Ariadne
  ariadne->Artune(_tune.value());
  ariadne->Arprda();
  ariadne->Arinit(_initMode.value());

  // setup Pythia
  if (pythia->Pysetini()) errlog(ELfatal,"[PYTHIA_SETUP]")
      << "AriadneModule: Pythia setup failure" << endmsg; 
  // Read a decay table:
  std::string fileMode = _pythiaMenu->decayFileMode();
  std::string fileName = _pythiaMenu->decayFile();
  int mode=_pythiaMenu->decayTableMode();
  if (fileMode=="READ" || fileMode=="read") {
    int lun=37;
    if (pythia->Opdcay(fileName,&lun,"R")) {
      ERRLOG(ELerror,"[PYTHIA_NO_DCYTBL]")
        << "AriadneModule: decay table file " 
        << "\"" << fileName << "\"\n"
        << "cannot be opened for read!" << endmsg;
    } else {
    pythia->Pyupda(&mode,&lun);
    pythia->Cldcay(&lun);
    ERRLOG(ELsuccess,"[PYTHIA_DCYTBL_R")
      << "AriadneModule: read decay table file "
      << "\"" << fileName << "\"" << endmsg;
    }
  }
  std::string frame  = _pythiaMenu->frame();
  std::string target = _pythiaMenu->getTarget();
  double winval = _pythiaMenu->win();
  int minlist   = _pythiaMenu->initlistlevel();
  pythia->pysubs().msel() = _pythiaMenu->msel();
  //  pythia->pydatr().mrpy(1) = _pythiaMenu->randomseed();
  std::string pygiveFile = _pythiaMenu->pygiveFile();
  if(pygiveFile.size()) 
    {
      std::cout << "Pygive file  " << pygiveFile << std::endl;
      pythia->Pygive(pygiveFile);
    }


  pythia->Pyinit(frame,target,&winval );
  pythia->setEvents(0);
  pythia->setEventListLevel(minlist);
  pythia->Pylist(&minlist);

  // Write a new decay table:
  if (fileMode=="WRITE" || fileMode=="write") {
    int mode=1;
    int lun=37;
    if (pythia->Opdcay(fileName,&lun,"W")) {
      ERRLOG(ELerror,"[PYTHIA_NO_DCYTBL]")
        << "AriadneModule: decay table file " 
        << "\"" << fileName << "\"\n"
        << "cannot be opened for write!" << endmsg;
    } else {
      pythia->Pyupda(&mode,&lun);
      pythia->Cldcay(&lun);
      ERRLOG(ELsuccess,"[PYTHIA_DCYTBL_W]")
        << "AriadneModule: wrote decay table file "
        << "\"" << fileName << "\"" << endmsg;
    }
  }
  pythia->setFirstListEvent( _pythiaMenu->firstlistevent() );
  pythia->setLastListEvent ( _pythiaMenu->lastlistevent() );
  pythia->setEventListLevel(_pythiaMenu->eventlistlevel());
  std::cout << " Event list level " 
	    << _pythiaMenu->eventlistlevel()
	    << std::endl;

  return AppResult::OK;
}

int AriadneModule::callGenerator(AbsEvent* anEvent) {
    
  Pythia* pythia = Pythia::Instance();

  // Generate the event
  pythia->Pyevnt();
  //  cascade it
  Ariadne::Instance()->Arexec();

  bool eventList = ( pythia->events() >= pythia->FirstListEvent() && 
		     pythia->events() <= pythia->LastListEvent() );

  // Update our event counter 
  pythia->addEvent();  // See if we are supposed to list this event  
  int mevlist= pythia->EventListLevel();
  // edit the event if needed
  int medit = _pythiaMenu->pyeditAction();
  if( medit >= 0 ) pythia->Pyedit(&medit);
  if ( eventList ) {
    std::cout << "PYEVNT event no. " << pythia->events() 
	      <<" level " << mevlist;
    if( medit >= 0 ) std::cout << " after PYEDIT action = " << medit; 
    std::cout << std::endl;
    pythia->Pylist(&mevlist); // List this event if required
  }
  
  return 0;  
}
 
void AriadneModule::fillHepevt() {

  Pythia* pythia = Pythia::Instance();
  // Tell lunhep to convert from PYJETS to HEPEVT
  int mconv=1;				
  // Do the conversion
  pythia->Lunhep(&mconv); // Use STDHEP translation routine 1999/01/27 CG
  CdfHepevt::Instance()->HepevtPtr()->NEVHEP = pythia->events();

// fill in some HEPEV4 info, courtesy of Dmitri Tsybychev
  CdfHepevt::Instance()->Hepev4Ptr()->SCALELH[5] =
pythia->pypars().pari(17);

  CdfHepevt::Instance()->Hepev4Ptr()->EVENTWEIGHTLH=
pythia->pypars().pari(10);
//dt.end

}

AppResult AriadneModule::genEndRun(AbsEvent* aRun) {
  Pythia* pythia = Pythia::Instance();
  int mstat = _pythiaMenu->statlistlevel();
  std::cout << "Call PYSTAT at endRun" << std::endl;
  pythia->Pystat(&mstat);

  return AppResult::OK;
}

AppResult AriadneModule::genEndJob() {
  Pythia* pythia = Pythia::Instance();
  int mstat = _pythiaMenu->statlistlevel();
  std::cout << "Call PYSTAT at endJob" << std::endl;
  pythia->Pystat(&mstat);

  return AppResult::OK;
}










