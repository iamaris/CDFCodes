#include "SimulationMods/IntegrationControl.hh"

#include "Framework/APPMenu.hh"
#include "GeometryBase/Menu/AbsDetectorNodeSelector.hh"
#include "GeometryBase/Menu/GeometryMenuManager.hh"
#include "SimulationMods/Debug.hh"
#include "SimulationMods/ConfigCmd.hh"

namespace sim {

  void IntegrationControl::_initializeTalkTo() 
	{ 
		_debugMenu.initialize("DebugMenu", this);
		_debugMenu.initTitle("Debugging options for IntegrationControl.");
		commands()->append(&_debugMenu);
		_debugMenu.commands()->append(&debug_level);
	}


} // namespace sim


/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
