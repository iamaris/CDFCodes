#ifndef SIM_CONFINGCMD_INCLUDED
#define SIM_CONFINGCMD_INCLUDED 1
////////////////////////////////////////////////////////////////////////
//
// File: ConfigCmd.hh
// $Id: ConfigCmd.hh,v 1.7 2003/02/05 03:46:14 sexton Exp $
// Purpose: Definition of the ConfigCmd class.
// This class provides the menu item (an APPCommand) that provides for
// adding a new type of digitizer to the simulation. It carries a reference
// to the ConfigInfoMap (which holds the configuration information for the
// digitizer for each detector element type) and a reference to the G3SimMgr,
// which is the SimulationManager that drives the simulation.
//
// Created: ?
// Modified: 25-Sep-2000 Marc Paterno
//           Added a bit of documentation.
//
//
////////////////////////////////////////////////////////////////////////

#include <string>

#include "FrameUtil/APPCommand.hh"
#include "SimulationMods/Config.hh"
#include "SimulationMods/G3Data.hh"

class AppModule;
class APPMenu;

namespace sim {

	class ConfigCmd : public APPCommand
	{
	public:
		ConfigCmd(AppModule* m, 
              G3SimMgr& d, 
              ConfigInfoMap& c,
              APPMenu* configMenu=0)
      :
			APPCommand("add",m),
			mgr(d),
			config_map(c),
			desc("Add Digi Description"),
      _configMenu(configMenu)
			{ }

		ConfigCmd(char* n, 
              char* des, 
              AppModule* m, 
              G3SimMgr& d, 
              ConfigInfoMap& c,
              APPMenu* configMenu=0)
      :
			APPCommand(n,m),
			mgr(d),
			config_map(c),
			desc(des),
      _configMenu(configMenu)
			{ }

		~ConfigCmd() 
			{ }

		void show() const;

		bool isShowable() const 
			{ return true; }
	
		std::string description() const 
			{ return desc; }

		int handle(int argc, char* argv[]);

	protected:
		G3SimMgr &			mgr;
		ConfigInfoMap & config_map;
		std::string			desc;

  private:
    // Where to put newly generated configuration menus
    APPMenu* _configMenu;
	};

}
#endif
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
