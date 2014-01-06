#include "SimulationMods/SimulationControl.hh"

#include "Framework/APPMenu.hh"
#include "GeometryBase/Menu/AbsDetectorNodeSelector.hh"
#include "GeometryBase/Menu/GeometryMenuManager.hh"
#include "SimulationMods/Debug.hh"
#include "SimulationMods/ConfigCmd.hh"
#include <string>

namespace sim {

  void SimulationControl::_initializeTalkTo() {
		// AbsParms

		_fastTrack.addDescription("         If true tracking is stopped outside COT" );
		_hshPackage.addDescription("         0 - gheisha; 1 - fluka" );
		_traceSecForClc.addDescription(" If true secondaries are traced inside volume z<400., r<cot_out_radius." );
		
		//    commands()->append(&_bmagnt);

    commands()->append(&_pvCentralX);
    commands()->append(&_pvCentralY);
    commands()->append(&_pvCentralZ);			
	
    commands()->append(&_pvSlope_dXdZ);				
    commands()->append(&_pvSlope_dYdZ);	

    commands()->append(&_pvSigmaZ);
    commands()->append(&_pvSigmaX);
    commands()->append(&_pvSigmaT);		

		commands()->append(&_fastTrack);
		commands()->append(&_hshPackage);
		commands()->append(&_traceSecForClc);

    // Standard commands    
    commands()->append(process_cmd);
		commands()->append(gccuts_cmd);
    commands()->append(conf_cmd);
    commands()->append(dump_cmd);

    // Menu to hold the configuration menus created by conf_cmd
    _configMenu.initialize("ConfigMenus",this);
    _configMenu.initTitle("Configuration menus for digitizers."); 
    commands()->append(&_configMenu);

		_userDefinedParticle.initialize("UserDefinedParticle",this);
		_userDefinedParticle.initTitle("Define your own particle in GEANT"); 
		commands()->append(&_userDefinedParticle);
		_itrtyp_com.addDescription("Type of tracking routine: 1 - GTGAMA, 2 - GTELEC, 3 - GTNEUT, 4 - GTHADR, 5 - GTMUON, 6 - GTNINO, 7 - GTCKOV, 8 - GTHION. Default 5.");
		_umass_com.addDescription("Particle mass");
		_ucharge_com.addDescription("Particle charge");
		_ulife_com.addDescription("Particle lifetime (sec)");
		_bratio1_com.addDescription("ratio1 - ratio6: array of branching ratios in %, maximum 6;");
		_dmode1_com.addDescription("dmode1 - dmode6: array of decay modes");
		
		_userDefinedParticle.commands()->append(&_itrtyp_com);
		_userDefinedParticle.commands()->append(&_umass_com);
		_userDefinedParticle.commands()->append(&_ucharge_com);
		_userDefinedParticle.commands()->append(&_ulife_com);
		_userDefinedParticle.commands()->append(&_bratio1_com);
		_userDefinedParticle.commands()->append(&_bratio2_com);
		_userDefinedParticle.commands()->append(&_bratio3_com);
		_userDefinedParticle.commands()->append(&_bratio4_com);
		_userDefinedParticle.commands()->append(&_bratio5_com);
		_userDefinedParticle.commands()->append(&_bratio6_com);
		_userDefinedParticle.commands()->append(&_dmode1_com);
		_userDefinedParticle.commands()->append(&_dmode2_com);
		_userDefinedParticle.commands()->append(&_dmode3_com);
		_userDefinedParticle.commands()->append(&_dmode4_com);
		_userDefinedParticle.commands()->append(&_dmode5_com);
		_userDefinedParticle.commands()->append(&_dmode6_com);

    // Debug menu
    _debugMenu.initialize("DebugMenu",this);
    _debugMenu.initTitle("Debugging options for SimulationControl.");
    commands()->append(&_debugMenu);
    _debugMenu.commands()->append(&debug_level);
    _debugMenu.commands()->append(&_showAVolumes);

    _showAVolumes.addDescription(std::string("\t\t\t\
Show all active volumes declared \
to the underlying\n\t\t\tsimulation (default ")+
                                 std::string(_showAVolumes.value()?"true":"false")+
                                 std::string(")."));


    // Claim and initialize the detector menu
    _detectorMenu=
      GeometryMenuManager::instance()->claimSimSelectionMenu(this);
    assert(_detectorMenu);
    _detectorMenu->initialize("DetectorMenu",this);
    _detectorMenu->initTitle
      ("Select which detectors to simulate.");
    commands()->append(_detectorMenu);

    // Now populate the menu based on the contents of the simDeclaration menu:
    APPMenu* simDeclMenu = 
      GeometryMenuManager::instance()->simDeclarationMenu();
    assert(simDeclMenu && simDeclMenu->isInitialized());
        
    APPCommand** command;
    APPListIterator<APPCommand*> i(*simDeclMenu->commands());
    sim::AbsDetectorNodeSelector *s1,*s2;
    while (command=i() ) {
      s1=dynamic_cast<sim::AbsDetectorNodeSelector*>(*command);
      if (s1) { // A valid detector command from the geometry menu
        // Construct a name for the command. If the
        // detector-writers have followed the convention of
        // naming the selection command, "enableNAME", then the
        // new command will be "simulateNAME"
        std::string newCommand(s1->command()),rstring("declare");
        if (newCommand.find(rstring,0)==0) {
          newCommand.replace(0,rstring.size(),"simulate");
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
        s2->addDescription(std::string("\t\t\tSimulate detector response in the ") +
                           s2->subsystemName() +
                           std::string("."));
        // Add it to the menu
        _detectorMenu->commands()->append(s2);
        // Look after the pointer so we can reclaim the memory
        _dynamicCommands.push_back(s2);
      }
    }
  

		// Initialize the relevant submenu
		_randomNumberMenu.initialize("RandomNumberMenu",this);
		_randomNumberMenu.initTitle("Fake event random number menu");
		commands()->append(&_randomNumberMenu);
		
		// Add the commands to the menu
		_randomNumberMenu.commands()->append(&_randomSeed1);
		_randomNumberMenu.commands()->append(&_randomSeed2);
		
		std::ostringstream tmpSstream1;
		
		// Describe them
		tmpSstream1 << "      \t\t\tSeed #1 for the random number generator"
								<< "\n\t\t\t(default " << _randomSeed1.value() << ").";
		_randomSeed1.addDescription(tmpSstream1.str());
		tmpSstream1 << "      \t\t\tSeed #2 for the random number generator"
								<< "\n\t\t\t(default " << _randomSeed2.value() << ").";  
		_randomSeed2.addDescription(tmpSstream1.str());	
	}

}
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/



