
//-----------------------
// This Class's Header --
//-----------------------
#include "generatorMods/MCFM_Module.hh"

//---------------
// C++ Headers --
//---------------
#include <iostream>

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "BaBar/Cdf.hh"
#include "inc/bcs.h"
#include "evt/evt.h"
//#include "ParticleDB/hepevt.hh"
#include "stdhep_i/CdfHepevt.hh"
//#include "ParticleDB/CdfParticleDatabase.hh"
#include "pythia_i/Pythia.hh"

// extern "C" {
//  void pyupev_(int*, float*);
// }

extern "C" {
  void upinit_(int*, float*);
  void upevnt_();
}

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------
#include "r_n/CdfRn.hh"
#include <sstream>
using std::ostringstream ;


const char* MCFM_Module::genId = "MCFM";

//----------------
// Constructors --
//----------------

MCFM_Module::MCFM_Module()
  : AbsGenModule( MCFM_Module::genId, 
		  "MCFM"),
    _debug("debug",this,false),
    _usePythia("usePythia",this,true),
    _pythiaMenu(new PythiaMenu(this,0,"PythiaMenu")),
    _rndmSeed("randomSeed",this,1089)    
{
  ostringstream debugDesc;
  debugDesc  << "\tPrint out debug information ?";
  _debug.addDescription(debugDesc.str());
  commands()->append(&_debug);

  ostringstream rndmSeedDesc;
  rndmSeedDesc << "\trandom seed for MCFM";
  _rndmSeed.addDescription(rndmSeedDesc.str());
  commands()->append(&_rndmSeed);
  
  ostringstream usePythiaDesc;
  usePythiaDesc << "\tUse Pythia for PS and underlying event sim. ?";
  _usePythia.addDescription(usePythiaDesc.str());
  commands()->append(&_usePythia);

  _pythiaMenu->initialize("PythiaMenu",this);
  _pythiaMenu->initTitle("Pythia Menu");
  commands()->append(_pythiaMenu);
}

//--------------
// Destructor --
//--------------

MCFM_Module::~MCFM_Module() 
{ 
  delete _pythiaMenu;
}

//--------------
// Operations --
//--------------

AppResult MCFM_Module::genBeginJob( )
{
  Pythia* pythia = Pythia::Instance();
  // Calls to UPINIT and UPEVNT to force private version to be used
  int myint1 = 0;
  float myfloat1 = 0.;
  pythia->setPythiaUid("TEST_DRIVE");
  std::cout <<   "MCFM_Module::genBeginJob : trying to force UPINIT/UPEVNT " << std::endl;
  upinit_(&myint1, &myfloat1);
  upevnt_();

  // Subsequently we want to call MCFM versions :
  pythia->setPythiaUid("MCFM");

  // Initialisation routines (mainly reading from cards file) :
  mcfm_cdf_init_();

  // Set the random number seed based on tcl parameter :
  int mcfm_rndm_seed[1];
  mcfm_rndm_seed[0] = _rndmSeed.value();
  mcfm_cdf_setseed_(mcfm_rndm_seed);

  // First integration :
  mcfm_cdf_vegas_prepare_();
  
  // Set up Pythia as in PythiaModule :
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
  // Prevent pile-up
  if (pythia->pypars().mstp(131)) {
    pythia->pypars().mstp(131)=0;
    ERRLOG(ELerror,"[PYTHIA_NO_PILEUP]")
      << "PythiaModule: Pile up selected but is not currently supported, "
      << "now turned off"
      << endmsg;
  }


  // DSW, 23.07.2002. Use new Pythia singleton interface.
  // Call with special value for frame in order to trigger UPINIT :
  std::string special_frame = "USER";
  const char* frame = special_frame.c_str();
  const char* target = "dummy";
  double winval = 1;
  std::cout << "  MCFM_Module::genBeginJob : calling pythia->Pyinit " << std::endl;  
  pythia->Pyinit(frame,target,&winval);

  return AppResult::OK;
}

AppResult MCFM_Module::genBeginRun( AbsEvent* run )
{
  return AppResult::OK;
}

AppResult MCFM_Module::genEndJob()
{
  mcfm_cdf_exit_();

  return AppResult::OK;
}

AppResult MCFM_Module::genEndRun( AbsEvent* run )
{
  return AppResult::OK;
}

int MCFM_Module::callGenerator( AbsEvent* anEvent ) 
{
  // Call MCFM directly.
  mcfm_cdf_getevent_();

  // Clear up HEPEVT before filling it again :
  CdfHepevt* hepevt = CdfHepevt::Instance();
  hepevt->clear();
  hepevt->clearCommon();

  // Ask MCFM to fill STDHEP common block :
  // mcfm_cdf_stdhep_();

  // DSW, 23.07.2002. Use new Pythia singleton interface.
  Pythia* pythia = Pythia::Instance();
  // std::cout << "  MCFM_Module::callGenerator : calling pythia->Pyevnt()" << std::endl;
  // Let Pythia do it's stuff :
  pythia->Pyevnt();
  // Update the event counter :
  pythia->addEvent();
  int mevlist = pythia->EventListLevel();
  if (pythia->events() >= _pythiaMenu->firstlistevent() &&
      pythia->events() <= _pythiaMenu->lastlistevent())
    {
      std::cout << "PYEVNT event number " << pythia->events() << std::endl;
      pythia->Pylist(&mevlist);
      // int parameterList = 13;
      // pythia->Pylist(&parameterList);
    }
  // Tell lunhep to convert from PYJETS to HEPEVT
  int mconv = 1;
  // Fill HEPEVT :
  pythia->Lunhep(&mconv);
  
  return 0;
}

void MCFM_Module::fillHepevt() {
  // this is done somewhere else, should be moved here
}

