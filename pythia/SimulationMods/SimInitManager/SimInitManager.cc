#include "SimulationMods/SimInitManager.hh"
#include "geant_i/TGeant3.h"
#include "geant_i/Geant3GeometryInterface.hh"
#include "geant_i/G3Callbacks.hh"
#include "ErrorLogger_i/gERRLOG.hh"
#include "GeometryBase/CdfDetector.hh"
#include "GeometryBase/CdfPhysicalVolume.hh"
#include "r_n/CdfRn.hh"

extern "C" { 
  void grndm_(float*, int*);
}

SimInitManager::SimInitManager() 
  :
  AppModule( "SimInitManager", "Declare geometry to the underlying simulator")
  , _showMaterials("showMaterials",this,false)
  , _showMedia("showMedia",this,false)
  , _showLVolumes("showLogicalVolumes",this,false)
  , _showPVolumes("showPhysicalVolumes",this,false)
  , _resetCopyNumber("resetCopyNumber",this,false)
  , _applyAlignment("applyAlignment",this,false)
{

	// Create the geometry interface and initialize Geant3
	_geometryInterface = Geant3GeometryInterface::instance(true);
	
	// Initialize talk-tos
	_initializeTalkTo();
}

SimInitManager::~SimInitManager() {
  for (CommandList::const_iterator i=_dynamicCommands.begin();
       i!=_dynamicCommands.end(); ++i) {
    delete (*i);
  }
}

AppResult SimInitManager::beginJob(AbsEvent* aJob) {

  // Set some callbacks
  G3Callbacks* g3callbacks = G3Callbacks::instance();
  g3callbacks->setGufld((void (*)(float *, float *))gufld_);

  // Set the debugResetCopyNumberFlag attribute if appropriate
  _geometryInterface->setDebugResetCopyNumberFlag(_resetCopyNumber.value());

	// lets try this out - poke the geometry system and tell it to
	// do alignment now.  The run number is junk for now

	if(_applyAlignment.value()==true)
		{
			ERRLOG(ELinfo,"[SIMINIT:BEGINJOB]") 
				<< "doing detector update (for alignment)."
				<< endmsg;
	
			CdfDetector::instance()->update(1);
  
			ERRLOG(ELinfo,"[SIMINIT:BEGINJOB]") 
				<< "completed detector update (for alignment)."
				<< endmsg;
		}

  // Now declare all subdetectors.
  _declareDetectors();

	
  ERRLOG(ELinfo,"[SIMINIT:FINISHED_INIT]") 
    << "Finished geometry declarations."
    << endmsg;
	
  // Now declare the geometry definition finished so Geant can sort things out
  _geometryInterface->endDeclarations();

  ERRLOG(ELinfo,"[SIMINIT:FINISHED_INIT]") 
    << "Finished geometry end declarations."
    << endmsg;
	
  // This is where we print any required diagnostic information about
  // the declared objects
  if (_showMaterials.value()) _geometryInterface->showMaterials();
  if (_showMedia.value()) _geometryInterface->showMedia();
  if (_showLVolumes.value()) _geometryInterface->showLogicalVolumes();
  if (_showPVolumes.value()) _geometryInterface->showPhysicalVolumes();

  // --> Force to load cdf version of grndm
  // for extrapolator:
  CdfRn::Instance()->setGeantUid("EXTRAPOLATOR");
  float x = 1.;
  int i = 1;
  grndm_(&x, &i);  // "GRNDM"
  CdfRn::Instance()->setGeantUid("SIMULATION");

  return AppResult::OK;
}

AppResult SimInitManager::beginRun(AbsEvent* aRun) {
	// Implementing this may actually be very tricky if not impossible in
	// the case of Geant3
	return AppResult::OK;
}


