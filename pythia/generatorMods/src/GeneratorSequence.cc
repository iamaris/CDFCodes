//--------------------------------------------------------
// GeneratorSequence.cc
//
// created July 25 2001, Elena Gerchtein (CMU)
//

#include "generatorMods/GeneratorSequence.hh"

// Framework
#include "Framework/APPFramework.hh"
#include "Framework/APPSequence.hh"

#include "generatorMods/DecayPackageSequence.hh"
#include "GenTrig/GenTrigSequence.hh"
#include "generatorMods/MinBiasSequence.hh"
#include "generatorMods/GenOutputManager.hh"
#include "generatorMods/GenInputManager.hh"
#include "generatorMods/HadScatGenSequence.hh"


// decay packages

#include "generatorMods/TauolaModule.hh"

// min bias generator
#include "generatorMods/MinBiasModule.hh"

// filters
#include "GenTrig/GenTrigModule.hh"
//#include "EvtGen/EvtGenModule.hh"  // ???

// Generator output manager
#include "generatorMods/GenOutputManager.hh"

GeneratorSequence::GeneratorSequence()
  : APPSequence("GeneratorSequence", " Generator sequence ")
{ 
  AppModule* aMod;
  APPSequence* aSeq;
  AppFramework*  theFramework(framework());

  //gen input manager - input module
  theFramework->add( new GenInputManager() );

  // Had. Scat. generators
  aSeq = new HardScatGenSequence();
  theFramework->add( aSeq );

  // Decay Packages
  aSeq = new DecayPackageSequence();
  theFramework->add( aSeq );

  // generator output manager:
  // writes HEPG bank from HEPEVT commonblock;
  // checks that only one generator (I) is enabled.
  
  aMod = new GenOutputManager();
  theFramework->add( aMod );
  aMod->setEnabled(true);

  // currently writes HEPG by its own since MI does not work
  // placed AFTER GenOutputManager for the same reason
  aSeq = new MinBiasSequence();
  theFramework->add( aSeq );


  // Generator filters
  aSeq = new GenTrigSequence();
  theFramework->add( aSeq );

  //*****  HepRootManager should be added in build file

}

GeneratorSequence::~GeneratorSequence() {}



