//--------------------------------------------------------
// DecayPackageSequence.cc
//
// created July 30 2001, Elena Gerchtein (CMU)
//

#include "generatorMods/DecayPackageSequence.hh"

// Framework
#include "Framework/APPFramework.hh"
#include "Framework/APPSequence.hh"

// decay packages
#include "generatorMods/TauolaModule.hh"
#include "generatorMods/QQModule.hh"
#include "generatorMods/EvtGenMod.hh"  

const char* DecayPackageSequence::name = "DecayPackageSequence";

DecayPackageSequence::DecayPackageSequence()
  : APPSequence( DecayPackageSequence::name, " Decay Package Sequence ")
{ 
  AppModule* aMod;
  AppFramework*  theFramework(framework());
  
  // decay packages - all disabled by default;

  aMod = new TauolaModule();
  append( aMod );
  theFramework->add( aMod );
  aMod->setEnabled(false);

  aMod = new QQModule();
  append( aMod );
  theFramework->add( aMod );
  aMod->setEnabled(false);

  aMod = new EvtGenMod();
  append( aMod );
  theFramework->add( aMod );
  aMod->setEnabled(false);
  
}

DecayPackageSequence::~DecayPackageSequence() {}



