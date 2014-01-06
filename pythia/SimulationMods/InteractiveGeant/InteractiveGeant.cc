#include "SimulationMods/InteractiveGeant.hh"
#include "geant_i/TGeant3.h"
#include "geant_i/Geant3GeometryInterface.hh"
#include "geant_i/G3Callbacks.hh"
#include "ErrorLogger_i/gERRLOG.hh"
#include "GeometryBase/CdfDetector.hh"
#include "GeometryBase/CdfPhysicalVolume.hh"
#include "GeometryBase/Menu/AbsDetectorNodeSelector.hh"
#include "GeometryBase/Menu/GeometryMenuManager.hh"

#include <string>
using std::string;

InteractiveGeant::InteractiveGeant() 
  :
  AppModule( "InteractiveGeant", "Geometry display without simulation")
  , _showMaterials("showMaterials",this,false)
  , _applyAlignment("applyAlignment",this,true)
  , _showMedia("showMedia",this,true)
  , _showLVolumes("showLogicalVolumes",this,false)
  , _showPVolumes("showPhysicalVolumes",this,false)
  , _resetCopyNumber("resetCopyNumber",this,false)
{

	// Create the geometry interface and initialize Geant3
	_geometryInterface = Geant3GeometryInterface::instance(true);
	
	// Initialize talk-tos
	_initializeTalkTo();
}

InteractiveGeant::~InteractiveGeant() {
  for (CommandList::const_iterator i=_dynamicCommands.begin();
       i!=_dynamicCommands.end(); ++i) {
    delete (*i);
  }
}

AppResult InteractiveGeant::beginJob(AbsEvent* aJob) {

  // if ( _interactiveGeant.value()) 
    _geometryInterface->prepareInteractiveGeant();

  // Set some callbacks
    G3Callbacks* g3callbacks = G3Callbacks::instance();
  g3callbacks->setGufld((void (*)(float *, float *))gufld_);

  // Set the debugResetCopyNumberFlag attribute if appropriate
  _geometryInterface->setDebugResetCopyNumberFlag(_resetCopyNumber.value());

   CdfDetector::instance()->update(1);

  // Now declare all subdetectors.
  _declareDetectors();

  ERRLOG(ELinfo,"[SIMINIT:FINISHED_INIT]") 
    << "Finished geometry declarations."
    << endmsg;
	
  // Now declare the geometry definition finished so Geant can sort things out
  _geometryInterface->endDeclarations();

  // This is where we print any required diagnostic information about
  // the declared objects
  if (_showMaterials.value()) _geometryInterface->showMaterials();
  if (_showMedia.value()) _geometryInterface->showMedia();
  if (_showLVolumes.value()) _geometryInterface->showLogicalVolumes();
  if (_showPVolumes.value()) _geometryInterface->showPhysicalVolumes();
	
  // Start interactive Geant if selected:
  //  if ( _interactiveGeant.value()) 
    _geometryInterface->interactiveGeant();

  return AppResult::OK;
}

AppResult InteractiveGeant::beginRun(AbsEvent* aRun) {
	// Implementing this may actually be very tricky if not impossible in
	// the case of Geant3
	return AppResult::OK;
}

void InteractiveGeant::_declareDetectors() {

  APPCommand** command;
  APPListIterator<APPCommand*> i(*_detectorMenu->commands());
  sim::AbsDetectorNodeSelector* selector;
  while (command=i() ) {
    selector=dynamic_cast<sim::AbsDetectorNodeSelector*>(*command);
    if (selector && selector->isUserEnabled() && selector->value()) {
      _geometryInterface->declareDetectorTree(selector->node(),
                                              selector->subsystemName()+
                                              string(" simulation"));
    }
  }
}

void InteractiveGeant::_initializeTalkTo() {

  // Initialize the menus

  // First, claim and initialize the InteractiveGeantMenu
  _detectorMenu=GeometryMenuManager::instance()->claimIntGeantMenu(this);
  assert(_detectorMenu);
  _detectorMenu->initialize("DetectorMenu",this);
  _detectorMenu->initTitle
    ("Select which detectors to declare to the underlying simulation.");
  commands()->append(_detectorMenu);

  ////////////////////////////////////////////////////////////////////////
  APPMenu* geometryMenu = GeometryMenuManager::instance()->geometryMenu();
  assert(geometryMenu && geometryMenu->isInitialized());
  
  ////////////////////////////////////
  // Catch-all detector selector.
  ConnectedBoolOption* allDets = new
    ConnectedBoolOption("declareAll",this,false);
  allDets->addDescription("Enable the declaration of all currently \
enabled subsystems to GEANT3.");

  // Now populate the menu based on the contents of the geometryMenu
  APPCommand** command;
  APPListIterator<APPCommand*> i(*geometryMenu->commands());
  sim::AbsDetectorNodeSelector *s1,*s2;
  std::string rstring("enable");
  while (command=i() ) {
    s1=dynamic_cast<sim::AbsDetectorNodeSelector*>(*command);
    if (s1) { // A valid detector command from the geometry menu
      // Construct a name for the declaration command. If the
      // detector-writers have followed the convention of naming the
      // selection command, "enableNAME", then the new command will be
      // "declareNAME"
      std::string newCommand(s1->command());
      if (newCommand.find(rstring,0)==0) {
        newCommand.replace(0,rstring.size(),"declare");
      }
      // Create the new command
      s2=s1->newSelector(newCommand.c_str(),this);
      // Add it to the command on the geometry menu as a dependency
      s1->addConnection(s2,ConnectedBoolOption::ENABLE_IF_TRUE);
      s1->addConnection(s2,ConnectedBoolOption::DISABLE_IF_FALSE);
      s1->addConnection(s2,ConnectedBoolOption::SET_FALSE_IF_FALSE);
      // Set the detector subsystem's name from that of the geometry
      // menu command
      s2->setSubsystemName(s1->subsystemName());
      // Describe the command
      s2->addDescription(string("Declare the ") +
                         s2->subsystemName() +
                         string(" to the underlying simulation engine."));
      
      // Add it to the menu
      _detectorMenu->commands()->append(s2);

      // Add a connection to declareAll
      allDets->addConnection(s2,
                             ConnectedBoolOption::SET_FALSE_IF_FALSE,
                             false); // don't initially follow connection
      allDets->addConnection(s2,
                             ConnectedBoolOption::SET_TRUE_IF_TRUE,
                             false); // don't initially follow connection

      // Look after the pointer so we can reclaim the memory
      _dynamicCommands.push_back(s2);
    }
  }
  // Add declareAll to the menu.
  _detectorMenu->commands()->append(allDets);
  _dynamicCommands.push_back(allDets);

  _debugMenu.initialize("DebugMenu",this);
  _debugMenu.initTitle
    ("Various information print options for debugging purposes.");

  commands()->append(&_debugMenu);

  _showMaterials.addDescription(string("\t\t\tShow all materials declared to\
 the underlying\n\t\t\tsimulation (default ")+
                                string(_showMaterials.value()?"true":"false")+ 
                                string(")."));

  _showMedia.addDescription(string("\t\t\tShow all media declared to the\
 underlying\n\t\t\tsimulation (default ")+
                            string(_showMedia.value()?"true":"false")+ 
                            string(")."));

  _showLVolumes.addDescription(string("\t\t\tShow all logical volumes declared\
 to the underlying\n\t\t\tsimulation (default ")+
                               string(_showLVolumes.value()?"true":"false")+ 
                               string(")."));

  _showPVolumes.addDescription(string("\t\t\tShow all physical volumes declared\
 to the underlying\n\t\t\tsimulation (default ")+
                               string(_showPVolumes.value()?"true":"false")+ 
                               string(")."));

  _resetCopyNumber.addDescription(string("\t\t\tReset the copy number assigned\
 to each physical\n\t\t\tvolume at detectorNode boundaries (default ")+
                                  string(_resetCopyNumber.value()?"true":
                                         "false")+ 
                                  string(")."));
  _debugMenu.commands()->append(&_showMaterials);
  _debugMenu.commands()->append(&_showMedia);
  _debugMenu.commands()->append(&_showLVolumes);
  _debugMenu.commands()->append(&_showPVolumes);
  _debugMenu.commands()->append(&_resetCopyNumber);
}

