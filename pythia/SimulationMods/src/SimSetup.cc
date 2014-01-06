#include "SimulationMods/SimSetup.hh"

SimSetup* SimSetup::_instance = 0;

// default geant seeds
long SimSetup::DefaultAltRandomSeed1 = 0;
long SimSetup::DefaultAltRandomSeed2 = 0;

// default CLHEP seed
long  SimSetup::DefaultRandomSeed = 0;

SimSetup* SimSetup::Instance() {
  if ( _instance == 0 ) _instance = new SimSetup();
  return _instance;
}

SimSetup::SimSetup() :
  _fastTrack(false),
  _traceSecForClc(false),
  _altRandomSeed1(SimSetup::DefaultAltRandomSeed1),
  _altRandomSeed2(SimSetup::DefaultAltRandomSeed2),
  _randomSeed(SimSetup::DefaultRandomSeed)
{
  static Cleaner cleaner;
}

SimSetup::~SimSetup() {}

SimSetup::Cleaner::~Cleaner() 
{
  delete SimSetup::_instance;
  SimSetup::_instance = 0;
}

void SimSetup::setFastTrack( bool param) {
  _fastTrack = param;  
}

void SimSetup::setTraceSecForClc( bool param ) {
  _traceSecForClc = param;  
}

