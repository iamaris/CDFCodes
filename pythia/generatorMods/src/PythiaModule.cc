//
// rev.  aug 30 2001 lena: added genId and ct.
//

// String Header First  --
//------------------------
#include <string>
using std::string;
#include <string.h>

//-----------------------
// This Class's Header --
//-----------------------
#include "generatorMods/PythiaModule.hh"
#include "pythia_i/Pythia.hh"
#include "stdhep_i/CdfHepevt.hh"

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

const char* PythiaModule::genId="Pythia";

//----------------
// Constructors --
//----------------

PythiaModule::PythiaModule() : 
  AbsGenModule( PythiaModule::genId, 
		"AC++ Pythia module"),
  _pythiaMenu( new PythiaMenu( this, 0, "PythiaMenu") )
{

  _initializeTalkTo();
}

//--------------
// Destructor --
//--------------

PythiaModule::~PythiaModule()
{
  delete _pythiaMenu;
}

AppResult PythiaModule::genBeginRun(AbsEvent* aRun) {
  return AppResult::OK;
}

AppResult PythiaModule::genBeginJob() {

  Pythia* pythia = Pythia::Instance(); 
  
  if(Pythia::Instance()->pydat3().lenMdme() == 8000 && 
     Pythia::Instance()->pypars().mstp(182) < 200) {
    errlog(ELfatal,"[PYTHIA_VERS_ERRO]")
      << "PythiaModule: Common blocks are incorrect for Pythia version,"
      << "please delete binaries, setup lund v6_203 (or later) and rebuild." 
      << endmsg;
  }
  // set longer lived Ks and lambdas stable to be decayed in detector
  int index;
  int massCode[] = {310,3112,3122,3222,3312,3322,3334};
  for (int i = 0; i<7; i++) {
    index = Pythia::Instance()->Pycomp(&massCode[i]);
    Pythia::Instance()->pydat3().mdcy(index,1) = 0;
  }
  std::cout<< "PythiaModule: Please note the following long lived strange " 
	   << "particles have been" << std::endl;
  std::cout<< "              set stable and cannot be set to decay"<<std::endl;
  std::cout<< "              K_0S, Lambda, Sigma, Xi and Omega." <<std::endl;
  
  // Prevent pile-up
  if (pythia->pypars().mstp(131)) {
    pythia->pypars().mstp(131)=0;
    ERRLOG(ELerror,"[PYTHIA_NO_PILEUP]")
      << "PythiaModule: Pile up selected but is not currently supported, "
      << "now turned off"
      << endmsg;
  }
  // Allow Pythia to mix B0 mesons, should be off to let EvtGen do this
  if (_pythiaMenu->allowBMixing()) {    
    ERRLOG(ELsuccess,"[PYTHIA_DCYTBL_R")
      << "PythiaModule: B-Bbar mixing is enabled though this will "
      << "conflict with EvtGen if used" << endmsg;
  } else { 
    if( pythia->pydat1().mstj(26) != 0)    
      ERRLOG(ELerror,"[PYTHIA_BMIXOF_R]")
	<< "PythiaModule: B-Bbar mixing is disabled and should " 
	<< "be done using EvtGen"  << endmsg; 
    pythia->pydat1().mstj(26)=0;    
  }

  // Read a decay table:
  std::string fileMode = _pythiaMenu->decayFileMode();
  std::string fileName = _pythiaMenu->decayFile();

  std::cout << "SYJUN: Decay fileName fileMode " << fileName << " " << fileMode << std::endl;

  if (!fileName.empty()) {
    if (fileMode=="READ" || fileMode=="read") {

      int mode=_pythiaMenu->decayTableMode();
      int lun=37;

      std::cout << "SYJUN: Decay fileName fileMode TableMode " << fileName << " " << fileMode << " " << mode << std::endl;

      if (pythia->Opdcay(fileName,&lun,"R")) {
        ERRLOG(ELerror,"[PYTHIA_NO_DCYTBL]")
          << "PythiaModule: decay table file " 
          << "\"" << fileName << "\"\n"
          << "cannot be opened for read!" << endmsg;
      } else {

	std::cout << "SYJUN1: Decay mode lun " << &mode << " " << &lun << std::endl;

	pythia->Pyupda(&mode,&lun);

	std::cout << "SYJUN2: Decay mode lun " << &mode << " " << &lun << std::endl;

	pythia->Cldcay(&lun);
	ERRLOG(ELsuccess,"[PYTHIA_DCYTBL_R")
	  << "PythiaModule: read decay table file "
	  << "\"" << fileName << "\"" << endmsg;
      }
    }
  }
  
  // if non zero value of mstp(142) set up weighting
  if(pythia->pypars().mstp(142)) {
    double expwt = _pythiaMenu->expWeight();
    int param = _pythiaMenu->weightParam();
    pythia->Pyevwt(&param, &expwt);
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


  int slhalun = 37;


  std::cout << "Open Decay File: susyhit_slha.out" << std::endl;
  pythia->Opdcay("susyhit_slha.out",&slhalun,"r");
  std::cout << "File Open" << std::endl;

  pythia->Pyinit(frame,target,&winval );

  std::cout << "Close Decay File: susyhit_slha.out" << std::endl;
  pythia->Cldcay(&slhalun);
  std::cout << "File Closed" << std::endl;

  pythia->setEvents(0);
  pythia->setEventListLevel(minlist);
  

  if (!fileName.empty()) {
    if (fileMode=="WRITE" || fileMode=="write") {
      int mode=1;
      int lun=37;
      if (pythia->Opdcay(fileName,&lun,"W")) {
        ERRLOG(ELerror,"[PYTHIA_NO_DCYTBL]")
          << "PythiaModule: decay table file " 
          << "\"" << fileName << "\"\n"
          << "cannot be opened for write!" << endmsg;
      } else {
        pythia->Pyupda(&mode,&lun);
        pythia->Cldcay(&lun);
        ERRLOG(ELsuccess,"[PYTHIA_DCYTBL_W]")
          << "PythiaModule: wrote decay table file "
          << "\"" << fileName << "\"" << endmsg;
      }
    }


  } 

  pythia->Pylist(&minlist);
  pythia->setFirstListEvent( _pythiaMenu->firstlistevent() );
  pythia->setLastListEvent ( _pythiaMenu->lastlistevent() );
  pythia->setEventListLevel(_pythiaMenu->eventlistlevel());
  std::cout << " Event list level " 
	    << _pythiaMenu->eventlistlevel()
	    << std::endl;

  return AppResult::OK;
}

int PythiaModule::callGenerator(AbsEvent* anEvent) {
    
  Pythia* pythia = Pythia::Instance();

  // Generate the event
  pythia->Pyevnt();
  bool eventList = ( pythia->events() >= pythia->FirstListEvent() && 
		     pythia->events() <= pythia->LastListEvent() );

  if (_pythiaMenu->selectPdg()!=0) {
    int code = _pythiaMenu->selectPdg();
    int icount = 0;
    while(pythia->findPdgCode(&code)==false) {
      if ( verbose() ) {
	if (++icount%100==0 && eventList) std::cout << name() << "::callGenerator generated " << icount << std::endl;
      }
       pythia->Pyevnt();
    }
  }

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
 
void PythiaModule::fillHepevt() {

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

AppResult PythiaModule::genEndRun(AbsEvent* aRun) {
  Pythia* pythia = Pythia::Instance();
  int mstat = _pythiaMenu->statlistlevel();
  cout << "Call PYSTAT at endRun" << endl;
  pythia->Pystat(&mstat);

  return AppResult::OK;
}

AppResult PythiaModule::genEndJob() {
  Pythia* pythia = Pythia::Instance();
  int mstat = _pythiaMenu->statlistlevel();
  cout << "Call PYSTAT at endJob" << endl;
  pythia->Pystat(&mstat);

  return AppResult::OK;
}










