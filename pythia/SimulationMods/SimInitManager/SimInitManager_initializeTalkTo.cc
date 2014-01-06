#include "SimulationMods/SimInitManager.hh"

#include "GeometryBase/Menu/GeometryMenuManager.hh"
#include "GeometryBase/Menu/AbsDetectorNodeSelector.hh"

#include <assert.h>

#include <string>
using std::string;

void SimInitManager::_initializeTalkTo() {

  // Initialize the menus

  // First, claim and initialize the DetectorMenu
  _detectorMenu=GeometryMenuManager::instance()->claimSimDeclarationMenu(this);
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
    ConnectedBoolOption("declareAll",this,true);
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
      s2=s1->newSelector(newCommand.c_str(),this,true);
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
  commands()->append(&_applyAlignment);

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

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
