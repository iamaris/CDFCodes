// author: Elena Gerchtein, CMU
//

#include <string>
using std::string;
#include "generatorMods/PythiaMinBias.hh"
#include "generatorMods/PythiaModule.hh"
#include "pythia_i/Pythia.hh"
#include "stdhep_i/CdfHepevt.hh"

#include "Framework/APPFramework.hh"
#include "Framework/APPExecNode.hh"
#include "Framework/APPExecutable.hh"
#include "Framework/APPSequence.hh"

#include <iostream>
#include <iomanip>
using std::cout;
using std::endl;
using std::setw;

#include "AbsEnv/AbsEnv.hh"
#include "stdhep_i/CdfHepevt.hh"
#include "SimulationObjects/HEPG_StorableBank.hh"
#include "Edm/Handle.hh"
#include "generatorMods/HadScatGenSequence.hh"

const char* PythiaMinBias::genId="PythiaMinBias";

PythiaMinBias::PythiaMinBias() : 
  AbsGenModule( PythiaMinBias::genId, 
		"AC++ Pythia module"),
  _pythiaMenu( new PythiaMenu( this, 0, "PythiaMenu") )
{

  _initializeTalkTo();
}

//--------------
// Destructor --
//--------------

PythiaMinBias::~PythiaMinBias()
{
  delete _pythiaMenu;
}

AppResult PythiaMinBias::genBeginRun(AbsEvent* aRun) {
  return AppResult::OK;
}

AppResult PythiaMinBias::genBeginJob() {

  std::list<APPExecNode*>* nodes; 
  std::string str1;
  std::string en_name;
  std::list<APPSequence*>* secl = framework()->sequences()->stlList();
  for ( std::list<APPSequence*>::iterator s = (*secl).begin();
	  s != (*secl).end(); ++s ) {
    if ( strcmp(((APPExecutable*)(*s))->name(),HardScatGenSequence::name) == 0 ) {
      nodes = ((APPSequence*)(*s))->nodes()->stlList();
       for ( std::list<APPExecNode*>::iterator gen = (*nodes).begin();
	     gen !=  (*nodes).end(); ++gen ) {
	 APPExecutable* target = (*gen)->target();
	 if ( target->isEnabled() ) {
	   str1 = PythiaModule::genId;
	   if ( en_name == str1 ) {
	     errlog(ELfatal,"[PYTHIA_VERS_ERRO]")
	      << "PythiaMinBias: to run this module disable PythiaModule"
	      << endmsg;
	   }
	 }
       }
    }
  }

  Pythia* pythia = Pythia::Instance();

  if(pythia->pydat3().lenMdme() == 8000 && 
     pythia->pypars().mstp(182) < 200) {
    errlog(ELfatal,"[PYTHIA_VERS_ERRO]")
      << "PythiaMinBias: Common blocks are incorrect for Pythia version,"
      << "please delete binaries, setup lund v6_203 (or later) and rebuild." 
      << endmsg;
  }

  // set longer lived Ks and lambdas stable to be decayed in detector
  int index;
  int massCode[] = {310,3112,3122,3222,3312,3322,3334};
  for (int i = 0; i<7; i++) {
    index = pythia->Pycomp(&massCode[i]);
    pythia->pydat3().mdcy(index,1) = 0;
  }
  std::cout<< "PythiaMinBias: Please note the following long lived strange " 
	   << "particles have been" << std::endl;
  std::cout<< "              set stable and cannot be set to decay"<<std::endl;
  std::cout<< "              K_0S, Lambda, Sigma, Xi and Omega." <<std::endl;
  
  // Prevent pile-up
  if (pythia->pypars().mstp(131)) {
    pythia->pypars().mstp(131)=0;
    ERRLOG(ELerror,"[PYTHIA_NO_PILEUP]")
      << "PythiaMinBias: Pile up selected but is not currently supported, "
      << "now turned off"
      << endmsg;
  }
    
  // Read a decay table:
  std::string fileMode = _pythiaMenu->decayFileMode();
  std::string fileName = _pythiaMenu->decayFile();
  if (!fileName.empty()) {
    if (fileMode=="READ" || fileMode=="read") {
      int mode=_pythiaMenu->decayTableMode();
      int lun=37;
      if (pythia->Opdcay(fileName,&lun,"R")) {
        ERRLOG(ELerror,"[PYTHIA_NO_DCYTBL]")
          << "PythiaMinBias: decay table file " 
          << "\"" << fileName << "\"\n"
          << "cannot be opened for read!" << endmsg;
      } else {
      pythia->Pyupda(&mode,&lun);
      pythia->Cldcay(&lun);
      ERRLOG(ELsuccess,"[PYTHIA_DCYTBL_R")
        << "PythiaMinBias: read decay table file "
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

  pythia->Pyinit(frame,target,&winval );
  pythia->setEvents(0);
  pythia->setEventListLevel(minlist);
  pythia->Pylist(&minlist);
  // Temporary dummy call to pyofsh to force private version to be used
  pythia->Pyofsh();

  // Write a new decay table:
  if (!fileName.empty()) {
    if (fileMode=="WRITE" || fileMode=="write") {
      int mode=1;
      int lun=37;
      if (pythia->Opdcay(fileName,&lun,"W")) {
        ERRLOG(ELerror,"[PYTHIA_NO_DCYTBL]")
          << "PythiaMinBias: decay table file " 
          << "\"" << fileName << "\"\n"
          << "cannot be opened for write!" << endmsg;
      } else {
        pythia->Pyupda(&mode,&lun);
        pythia->Cldcay(&lun);
        ERRLOG(ELsuccess,"[PYTHIA_DCYTBL_W]")
          << "PythiaMinBias: wrote decay table file "
          << "\"" << fileName << "\"" << endmsg;
      }
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

int PythiaMinBias::callGenerator(AbsEvent* anEvent) {
    
  Pythia* pythia = Pythia::Instance();

  //// cash commons
  //pythia->copyCommons2Objects();

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
  if ( eventList ) {
    std::cout << "PYEVNT event no. " << pythia->events() 
	      <<" level " << mevlist << std::endl;
    pythia->Pylist(&mevlist); // List this event if required
  }
  return 0;  
}
 
void PythiaMinBias::fillHepevt() {

  // COPY COMMONS BACK BEFORE RETURN

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

/*
// DUMP HEPG HERE

    AbsEvent* event = AbsEnv::instance()->theEvent();
  CdfHepEvt hepevt;
  HEPG_StorableBank* hepg = hepevt.makeHEPG_Bank(event);

  if (!hepg || hepg->is_invalid()) {
    ERRLOG(ELwarning,"MinBiasModule") << "Can't create a valid HEPG_StorableBank" << endmsg;
    pythia->copyObjects2Commons(); return;
  }
  // Add the bank to the event
  // Get the handle
  Handle<HEPG_StorableBank> h(hepg);
  // Append the bank to the event
  if ((event->append(h)).is_null()) {
   ERRLOG(ELwarning,"MinBiasModule") << "Can't add HEPG_StorableBank to the event." << endmsg;
   pythia->copyObjects2Commons(); return;
  }
  pythia->copyObjects2Commons(); return;
*/
}

AppResult PythiaMinBias::genEndRun(AbsEvent* aRun) {
  Pythia* pythia = Pythia::Instance();
  int mstat = _pythiaMenu->statlistlevel();
  cout << "Call PYSTAT at endRun" << endl;
  pythia->Pystat(&mstat);

  return AppResult::OK;
}

AppResult PythiaMinBias::genEndJob() {
  Pythia* pythia = Pythia::Instance();
  int mstat = _pythiaMenu->statlistlevel();
  cout << "Call PYSTAT at endJob" << endl;
  pythia->Pystat(&mstat);

  return AppResult::OK;
}

void PythiaMinBias::_initializeTalkTo() {

  _pythiaMenu->initialize("PythiaMenu",this);
  _pythiaMenu->initTitle( " Pythia Menu " );
  commands()->append( _pythiaMenu );
}









