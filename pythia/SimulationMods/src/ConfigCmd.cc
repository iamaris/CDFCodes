////////////////////////////////////////////////////////////////////////
//
// File: ConfigCmd.cc
// $Id: ConfigCmd.cc,v 1.13 2003/02/05 03:46:14 sexton Exp $
// Purpose: 
//
// Created: ?  
// Modified: 
//
// 25-Sep-2000 Marc Paterno Send error messages to std::cerr, not
// std::cout. Clean up formatting of some error messages.
//
//
////////////////////////////////////////////////////////////////////////





#include <cstdlib>
#include <assert.h>
#include <iostream>

#include "Framework/APPFramework.hh"
#include "Framework/APPList.hh"
#include "FrameUtil/APPCommand.hh"
#include "Framework/APPListIterator.hh"
#include "Framework/AppActionController.hh"
#include "FrameUtil/AbsInterp.hh"
#include "SimulationMods/ConfigCmd.hh"
#include "SimulationMods/G3Data.hh"

using std::string;
using std::cerr;
using std::cout;
using std::endl;

namespace sim {

	// ------------------------------------------------
	int ConfigCmd::handle(int argc, char* argv[])
  {
    if(argc < 4)
      {
        std::cerr << "Format: "
             << argv[0] 
             << " detector_element_class_name digitizer_class_name group_name"
             << std::endl;
        return AbsInterp::ERROR;
      }

		assert (argv[1] != NULL);
		assert (argv[2] != NULL);
		assert (argv[3] != NULL);

    string de_name = argv[1];
    string digi_name = argv[2];
    string group_name = argv[3];

		assert (! de_name.empty() );
		assert (! digi_name.empty() );
		assert (! group_name.empty() );

    string combined_name = group_name + '_' + de_name;

    APPMenu* menu = mgr.findMenu(combined_name);

    if (menu)
      {
        std::cerr << "Already built menu for the following group/detector element: "
             << group_name << '/' << de_name << std::endl;
        return AbsInterp::ERROR;
      }

    G3Factory* fact = G3Factory::instance();
    menu = fact->makeMenu(digi_name);

    if (menu == NULL)
      {
        std::cerr << "Could not create a menu for digitizer: "
             << digi_name
             << "; class not recognized by G3Factory instance."
						 << std::endl;
        return AbsInterp::ERROR;
      }

    // Put finishing touches on this menu...
    // ... first make the title
    string title("Configuration menu for: ");
    title += combined_name;
    menu->initTitle(title.c_str());

    // Check whether we're holding a valid top-level configuration menu
    // into which to place our new one. Should never fail if the
    // framework has been programmed properly.
    assert(!_configMenu || _configMenu->isInitialized());
    
    // Which module to attach the menu to?
    AppModule* mtarget = _configMenu?_configMenu->target():target();

    // Now connect the menu to the right containing menu
    menu->initialize(combined_name.c_str(), // this becomes the tcl command
                     mtarget,  // this gives the correct module
                     _configMenu); // the menu into which to place it
    // and add it to that menu's list of commands
    _configMenu->commands()->append(menu);

    // Give this menu to the SimManager
    if ( mgr.addMenu(group_name, de_name, menu)  == false)
      {
        delete menu;
				std::cerr << "You have already registered a menu for detector element: "
             << de_name
             << " and group: "
             << group_name
             << ".\nThe attempt to add a second has been ignored"
             << std::endl;
        return AbsInterp::ERROR;
      }

    config_map[ de_name ].push_back(ConfigInfo(digi_name, group_name));
		return AbsInterp::OK;
  }

  void ConfigCmd::show() const {
    std::cout << "\t\t\tThe following detector elements have been configured with digitizers:"
         << std::endl;
    ConfigInfoMap::const_iterator mEnd = config_map.end();
    for (ConfigInfoMap::const_iterator i = config_map.begin();
         i!=mEnd; ++i) {
      ConfigInfoList::const_iterator lEnd = (*i).second.end();
      for (ConfigInfoList::const_iterator j =(*i).second.begin();
           j!=lEnd; ++j) {
        std::cout << "\t\t\t\t" << (*i).first 
             << " with digitizer " << (*j).digi_class
             << " in group " << (*j).group_name
             << std::endl;
      }
    }
  }

} // namespace sim

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

