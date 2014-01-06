//--------------------------------------------------------
// MinBiasSequence.cc
//
// created July 30 2001, Elena Gerchtein (CMU)
//

#include "generatorMods/MinBiasSequence.hh"

// Framework
#include "Framework/APPFramework.hh"
#include "Framework/APPSequence.hh"

// min bias modules
#include "generatorMods/MinBiasModule.hh"
#include "generatorMods/MixFakeEv.hh"
#include "generatorMods/PythiaMinBias.hh"

MinBiasSequence::MinBiasSequence()
  : APPSequence("MinBiasSequence()", "Min Bias sequence ")
{ 
  AppModule* aMod;
  AppFramework*  theFramework(framework());
  
  aMod = new MinBiasModule();
  append( aMod );
  theFramework->add( aMod );
  aMod->setEnabled(false);

  aMod = new MixFakeEv();
  append( aMod );
  theFramework->add( aMod );
  aMod->setEnabled(false);

  aMod = new PythiaMinBias();
  append( aMod );
  theFramework->add( aMod );
  aMod->setEnabled(false);
  
}

MinBiasSequence::~MinBiasSequence() {}



