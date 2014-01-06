//***************************************************************************
//* W. H. Bell ************ Feb 15, 2001 ************************************
//* A module to allow HEPG Bank production from Pythia & EvtGen or EvtGen.  *
//***************************************************************************/
// rev: March 22 2003 RJT:  Rename decayBuBdBs --> decayBMeson
//                          add decayBBaryon.
// rev: April 1st 2002 fkw: got rid of CdfHepEvt, fixe typo in talk-to
//                          Paricles are now Particles
// 
// rev: dec 07 2001 lena: inherited AbsDecpackModule; 
//                        replaced CdfHepEvt by CdfHepevt;

#include "generatorMods/EvtGenMod.hh"
#include "EvtGenBase/EvtRandomEngine.hh"

// CLHEP Random Number headers
#include "r_n/CdfRn.hh"
#include "CLHEP/config/CLHEP.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Vector/LorentzRotation.h"

#include "AbsEnv/AbsEnv.hh"
#include "SimulationObjects/HEPG_StorableBank.hh"
#include "Edm/Handle.hh"
#include "Edm/ConstHandle.hh"
#include "Edm/EventRecord.hh"

#include "stdhep_i/CdfHepEvt.hh"  // <= to be fixed

#include <string>
#include <assert.h>
#include <time.h>
#include <iostream>
#include <cstdlib>

const char* EvtGenMod::genId="EvtGen";

//fkw October 13th, 2002
//define class for generating random nubers
//The purpose of this class is to wrap an EvtRandomEngine
//around Elena's way of organizing random engines in CdfRn.
//As a result, the EvtGen code does not need to be changed
//and nevertheless it uses Elena's mechanism for keeping
//track of random numbers.
//The pointer to an instance of this class is made a member function
//of EvtGenMod because I wasn't sure if that's required to not
//drop it out of scope.
//fkw October 13th, 2002
class EvtCLHEPRandomEngine:public EvtRandomEngine{
public:
  double random();
  EvtCLHEPRandomEngine(long seed1, long seed2);
private:
  HepRandomEngine* _engine;
};

double EvtCLHEPRandomEngine::random(){
  return _engine->flat();
}

//constructor:
EvtCLHEPRandomEngine::EvtCLHEPRandomEngine(long seed1, long seed2) {
   CdfRn* rn = CdfRn::Instance();
   rn->SetEngineSeeds(seed1, seed2, EvtGenMod::genId);
   _engine = CdfRn::Instance()->GetEngine(EvtGenMod::genId);
}

//----------------
// Constructors --
//----------------

//fkw 4/6/2001 Kai 4.0 does not allow strings to be initialized with NULL.
//             I therefore had to initialize _useUserDecayFile with "dummy"
EvtGenMod::EvtGenMod() : 
  AbsDecpackModule(EvtGenMod::genId,"AC++ EvtGen module"),
      _useRootParticle("UseRootParticle",this,false),
      _rootParticlePx("RootParticlePx",this,0.0),
      _rootParticlePy("RootParticlePy",this,0.0),
      _rootParticlePz("RootParticlePz",this,0.0),
      _rootParticleHepID("RootParticleHepID",this,0),
      _useUserDecayFile("UseUserDecayFile",this,false),
      _userDecayFile("UserDecayFile",this,"dummy"),
      _adjustCPAsymm("AdjustCPAsymm",this,false),
      _decayPromptCharm("DecayPromptCharm",this,false),
      _decayBMeson("DecayBMeson",this,true),
      _decayBBaryon("DecayBBaryon",this,true),
      _randomSeed1("RandomSeed1",this,0),
      _randomSeed2("RandomSeed2",this,0)
{
  _eventCount = 0;
  _generator = NULL;
  _myevtgenInterface = NULL;
  _myRandomEngine = NULL;
  _initializeTalkTo();
}

//--------------
// Destructor --
//--------------

EvtGenMod::~EvtGenMod()
{
}

//--------------
// Operations --
//--------------

AppResult EvtGenMod::genBeginJob()
{  
  char *decay_file, *pdt_table;
  std::string filename;

  _myRandomEngine=new EvtCLHEPRandomEngine(_randomSeed1.value(), _randomSeed2.value());

  // Initialise EvtGen: reads decay file and properties file.
  if((decay_file = getenv("EVTGEN_DECAY_FILE"))==NULL) {
    ERRLOG(ELfatal,"[EVTGEN_NO_DCYTBL]")
      << "EvtGenModule: EVTGEN_DECAY_FILE not set"
      << endmsg;
  }
  else if((pdt_table = getenv("EVTGEN_PDT_TABLE"))==NULL) {
    ERRLOG(ELfatal,"[EVTGEN_NO_DCYTBL]")
      << "EvtGenModule: EVTGEN_PDT_TABLE not set"
      << endmsg;
  }
  else {
    _generator = new EvtGen(decay_file,pdt_table,_myRandomEngine);
    
    filename = _userDecayFile.value();
    if(_useUserDecayFile.value()) { 
      _generator->readUDecay(filename.c_str());
    }

    _myevtgenInterface = new EvtGenInterface(_generator,_adjustCPAsymm.value(),_verbose.value(), _decayBMeson.value(), _decayBBaryon.value(), _decayPromptCharm.value());
  }
  return AppResult::OK;
}

int  EvtGenMod::callGenerator(AbsEvent* anEvent)
{
  //CdfHepEvt hepevt;   // <= to be fixed
  //hepevt.init();     //  <= to be fixed

  _eventCount++;
  double p[3];
  int errorFlag;

  if (_verbose.value())
    std::cout << "=========== begining event # " << _eventCount << std::endl;
  
  if(_useRootParticle.value()) {
    if( _eventCount < 3 && _eventCount > 0) 
      std::cout <<  name() << "Using root particle\n";
    p[0]=_rootParticlePx.value();
    p[1]=_rootParticlePy.value();
    p[2]=_rootParticlePz.value();
    //errorFlag = _myevtgenInterface->DecayRoot(&hepevt,_rootParticleHepID.value(),p);
    errorFlag = _myevtgenInterface->DecayRoot(_rootParticleHepID.value(),p);
  }
  else {
    if( _eventCount < 3 && _eventCount > 0) 
      std::cout <<  name() << "Finding B Mesons and about to enter DecayAll\n";
    //errorFlag = _myevtgenInterface->DecayAll(&hepevt);
    errorFlag = _myevtgenInterface->DecayAll();
  }

  return 1;
}

AppResult EvtGenMod::genEndJob()
{
  if(_generator != NULL)
    delete _generator;
  if(_myevtgenInterface != NULL)
    delete _myevtgenInterface;
 
  return AppResult::OK;
}

AppResult EvtGenMod::genBeginRun( AbsEvent* anEvent )
{
  return AppResult::OK;
}

AppResult EvtGenMod::genEndRun( AbsEvent* anEvent )
{
  return AppResult::OK;
}




/**************************************************************************/

void EvtGenMod::_initializeTalkTo() {
  _useRootParticle.addDescription("\t\tSelect root particle (T) or read HepEvt (F)");
  _rootParticleHepID.addDescription("\t\tUses standard PDG particle codes");
  _adjustCPAsymm.addDescription("\t\tAccount for integrated probability of B mixing;\n\t\tUsed to generate CP violation.\n\t\t Default = false");
  _decayPromptCharm.addDescription("\t\tDecay prompt charm from hard scatter generator\n\t\t Default = false");
  _decayBMeson.addDescription("Decay B mesons from hard scatter generator\n\t\t Default = true\t\t");
  _decayBBaryon.addDescription("Decay B baryons from hard scatter generator\n\t\t Default = true\t\t");
  commands()->append(&_useRootParticle);
  commands()->append(&_rootParticlePx);
  commands()->append(&_rootParticlePy);
  commands()->append(&_rootParticlePz);
  commands()->append(&_rootParticleHepID);
  commands()->append(&_useUserDecayFile);
  commands()->append(&_userDecayFile);
  commands()->append(&_adjustCPAsymm);
  commands()->append(&_decayPromptCharm);
  commands()->append(&_decayBMeson);
  commands()->append(&_decayBBaryon);
  commands()->append(&_randomSeed1);
  commands()->append(&_randomSeed2);
}


