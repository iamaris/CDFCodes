#ifndef _INTERACTIVEGEANT_HH_
#define _INTERACTIVEGEANT_HH_

#include "Experiment/Experiment.hh"
#include "Framework/APPModule.hh"

//----------------------
// Collaborating Classes
//----------------------

#include "Framework/APPMenu.hh"
#include "Framework/AbsParmBool.hh"
#include "GeometryBase/CdfDetectorNode.hh"
#include "geant_i/Geant3GeometryInterface.hh"

class SimulatorGeometryInterface;

//							---------------------
//							-- Class Interface --
//							---------------------
 
class InteractiveGeant : public AppModule {

public:

  InteractiveGeant();
  ~InteractiveGeant();
			
  AppResult beginJob(AbsEvent* aJob);
  AppResult beginRun(AbsEvent* aRun);

private:
  Geant3GeometryInterface* _geometryInterface;

  void _initializeTalkTo();
  void _declareDetectors();

  // We don't own this: we claim it from GeometryMenuManager
  APPMenu* _detectorMenu;

  // Hold pointers to all dynamically created commands so we can destroy
  // them at the end.
  typedef std::list<APPCommand*> CommandList;
  CommandList _dynamicCommands;

  APPMenu _debugMenu;
  AbsParmBool _resetCopyNumber;
	AbsParmBool _applyAlignment;
  AbsParmBool _showMaterials;
  AbsParmBool _showMedia;
  AbsParmBool _showLVolumes;
  AbsParmBool _showPVolumes;
	//  AbsParmBool _interactiveGeant;
};


#endif	// INTERACTIVEGEANT_HH

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
