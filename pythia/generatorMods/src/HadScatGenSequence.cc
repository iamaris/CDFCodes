//--------------------------------------------------------
// HardScatGenSequence.cc
//
// created July 25 2001, Elena Gerchtein (CMU)
//

#include "generatorMods/HadScatGenSequence.hh"

// Framework
#include "Framework/APPFramework.hh"
#include "Framework/APPSequence.hh"

// had.scat. generators 
#include "generatorMods/PythiaModule.hh"
#include "generatorMods/IsajetModule.hh"
#include "generatorMods/HerwigModule.hh"
#include "generatorMods/HeavyQuarkGenModule.hh"
#include "generatorMods/BgeneratorModule.hh"
#include "generatorMods/FakeEv.hh"
#include "generatorMods/AriadneModule.hh"
#include "generatorMods/MCFM_Module.hh"
#include "generatorMods/WGRAD_Module.hh"
#include "generatorMods/WbbgenModule.hh"
#include "generatorMods/VecbosModule.hh"
#include "generatorMods/VecherModule.hh"
#include "generatorMods/UnwtModule.hh"
//#include "generatorMods/GrappaModule.hh"
#include "generatorMods/LesHouchesModule.hh"

const char* HardScatGenSequence::name = "HardScatGenSequence";

HardScatGenSequence::HardScatGenSequence()
  : APPSequence(HardScatGenSequence::name, " Hard Scat. generator sequence ")
{ 
  AppModule* aMod;
  AppFramework*  theFramework(framework());
  
  // hard scat. generators - all disabled by default;
  // No HEPG output from these generators.
  // fills HEPEVT at the end of the event();

  aMod = new PythiaModule();
  append( aMod );
  theFramework->add( aMod );
  aMod->setEnabled(false);

  aMod = new IsajetModule();
  append( aMod );
  theFramework->add( aMod );
  aMod->setEnabled(false);

  aMod = new HerwigModule();
  append( aMod );
  theFramework->add( aMod );
  aMod->setEnabled(false);

  aMod = new HeavyQuarkGenModule();
  append( aMod );
  theFramework->add( aMod );
  aMod->setEnabled(false);

  aMod = new BgeneratorModule();
  append( aMod );
  theFramework->add( aMod );
  aMod->setEnabled(false);

  aMod = new FakeEv();
  append( aMod );
  theFramework->add( aMod );
  aMod->setEnabled(false);

  aMod = new AriadneModule();
  append( aMod );
  theFramework->add( aMod );
  aMod->setEnabled(false);

  aMod = new MCFM_Module();
  append( aMod );
  theFramework->add( aMod );
  aMod->setEnabled(false);

  aMod = new WGRAD_Module();
  append( aMod );
  theFramework->add( aMod );
  aMod->setEnabled(false);

  aMod = new WbbgenModule();
  append( aMod );
  theFramework->add( aMod );  
  aMod->setEnabled(false);

  //  aMod = new GrappaModule();
  //append( aMod );
  //theFramework->add( aMod );  
  //aMod->setEnabled(false);

  //*VecbosModules

  aMod = new VecbosModule();
  append( aMod );
  theFramework->add( aMod );  
  aMod->setEnabled(false);

  aMod = new VecunwModule();
  append( aMod );
  theFramework->add( aMod );  
  aMod->setEnabled(false);

  aMod = new VecherModule();
  append( aMod );
  theFramework->add( aMod );  
  aMod->setEnabled(false);  

// External generators with Les Houches format
  aMod = new LesHouchesModule();
  append( aMod );
  theFramework->add( aMod );
  aMod->setEnabled(false);


}

HardScatGenSequence::~HardScatGenSequence() {}
