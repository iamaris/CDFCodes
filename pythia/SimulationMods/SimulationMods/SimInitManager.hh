#ifndef _SIMINITMANAGER_HH_
#define _SIMINITMANAGER_HH_

////////////////////////////////////////////////////////////////////////
// File and Version Information:
//			$Id: SimInitManager.hh,v 1.3 2002/05/21 20:08:46 jbk Exp $
//
// Description:
//			Declaration of the SimInitManager class.
//      SimInitManager is the AppModule that declares the CDF geometry (as
//      represented by the CdfDetector class and its associates) to GEANT3.
//      When and if it becomes necessary to declare the geometry to another
//      simulation system (e.g. GEANT4), a new AppModule subclass should
//      be defined to do the job.
//
//      This class is a leaf class; it is not intended for use as a base
//      class.
//
//   Added the DetectorNode struct, used to handle the combination of the
//   AbsParmBool and CdfDetectorNode* that corresponds to a single detector
//   node. Put these into a list, and made filling of the list automatic,
//   so that as a new detector nodes are added to the CdfDetector class,
//   they will automatically be added to this list of DetectorNodes.
//
// Environment:
//			Software developed for CDF
//
// 1999/06/30 CG
// Modified: 2000/03/02 Marc Paterno: Simplified to only deal with GEANT3.
//                      Removed 'virtual' declarations.
//
////////////////////////////////////////////////////////////////////////
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
 
class SimInitManager : public AppModule {

public:

  SimInitManager();
  ~SimInitManager();
			
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

};


#endif	// SIMINITMANAGER_HH

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
