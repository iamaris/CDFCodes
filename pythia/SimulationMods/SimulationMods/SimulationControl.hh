#ifndef SIM_SIMULATIONCONTROL_INCLUDED
#define SIM_SIMULATIONCONTROL_INCLUDED 1

// Purpose: SimulationControl is the AC++ module that "runs
// GEANT3". It provides a wrapper for an instance of
// SimulationManager<G3Stepdata>, which does the real work of
// simulation.
//
// Note: This code will not compile under EDM1; it requires that the
// "AbsEvent" passed around by the framework is really an EDM2
// EventRecord.
//
// $Id: SimulationControl.hh,v 1.22 2002/07/18 20:37:33 lena Exp $

#ifndef USE_CDFEDM2
#error The class SimulationControl requires the use of EDM2
#endif 

#include <string>
#include <vector>

#include "Framework/APPModule.hh"
#include "Framework/AbsParmGeneral.hh"
#include "Framework/AbsParmBool.hh"
#include "SimulationMods/Config.hh"
#include "SimulationMods/ProcessCmd.hh"
#include "SimulationMods/DumpFactoryCmd.hh"
#include "SimulationMods/G3Data.hh"
#include "SimulationMods/GccutsCmd.hh"
#include "SimulationMods/SimSetup.hh"
typedef AbsParmGeneral<long> AbsParmGeneral_long;

class AppFramework;
class APPMenu;
class CdfTangible;
class CdfDetectorElement;
class CdfDetectorNode;

namespace sim
{
	
	class ConfigCmd;

	class SimulationControl : public AppModule
	{
	public:
		// G3SimMgr is defined in SimulationMods/G3Data.hh	
		typedef G3SimMgr::step_data_type step_data_type;
		typedef G3SimMgr::se_type se_type;
		typedef G3SimMgr::pvid pvid;
		typedef G3SimMgr::deid deid;
	
		SimulationControl(const char* const theName = "SimulationControlMod", 
											const char* const desc = "Control the Simulation!");

		virtual ~SimulationControl();
	
		AppResult beginJob( EventRecord* aJob );
		AppResult beginRun( EventRecord* aRun );
		AppResult event( EventRecord* anEvent );
		AppResult endRun( EventRecord* aRun );
		AppResult endJob( EventRecord* aJob );
		AppResult abortJob( EventRecord* aJob );
	
	private:
		AppResult recurseTangible(G3::TmpPVID& id,
															const CdfDetectorElement* de,
															const CdfTangible* tang);

		AppResult processDE(const CdfDetectorElement* de);

		// Run GEANT3 on the "current event" (not the EDM event, but the
		// input data that GEANT3 knows about).
		void pokeGeant3();

		// Fill the given collection with pointers to const
		// DetectorNodes for all those detectors that have been
		//    (1) created (declared to geometry system)
		//    (2) declared to the simulation engine (GEANT3)
		//    (3) selected for simulation
		typedef std::vector<const CdfDetectorNode*> DNPtrList;
		void _getActiveDNs(DNPtrList& fill_me) const;

		AbsParmGeneral<float> _pvSigmaZ;
    AbsParmGeneral<float> _pvSigmaX;
		AbsParmGeneral<float> _pvSigmaT;

		AbsParmGeneral<float> _pvCentralX;
    AbsParmGeneral<float> _pvCentralY;
		AbsParmGeneral<float> _pvCentralZ;

    AbsParmGeneral<float> _pvSlope_dXdZ;
    AbsParmGeneral<float> _pvSlope_dYdZ;

		AbsParmGeneral<bool>  _fastTrack;
		AbsParmGeneral<int>   _hshPackage;
		AbsParmGeneral<bool>  _traceSecForClc;
	
		AbsParmGeneral<int> debug_level;
		G3SimMgr						mgr;
		ConfigInfoMap				config_map;
		ConfigCmd*					conf_cmd;
    ProcessCmd*         process_cmd;
		GccutsCmd*          gccuts_cmd;
		DumpFactoryCmd*			dump_cmd;
		bool								is_made;

    // We don't own this: we claim it from GeometryMenuManager
    APPMenu* _detectorMenu;

    // Hold pointers to all dynamically created commands so we can
    // destroy them at the end.
    typedef std::list<APPCommand*> CommandList;
    CommandList _dynamicCommands;

		APPMenu _userDefinedParticle;
		AbsParmGeneral<int>  _itrtyp_com;
		AbsParmGeneral<float> _umass_com;
		AbsParmGeneral<float> _ucharge_com;
		AbsParmGeneral<float> _ulife_com;
		AbsParmGeneral<float> _bratio1_com;
		AbsParmGeneral<float> _bratio2_com;
		AbsParmGeneral<float> _bratio3_com;
		AbsParmGeneral<float> _bratio4_com;
		AbsParmGeneral<float> _bratio5_com;
		AbsParmGeneral<float> _bratio6_com;
		AbsParmGeneral<int>   _dmode1_com;	
		AbsParmGeneral<int>   _dmode2_com;	
		AbsParmGeneral<int>   _dmode3_com;	
		AbsParmGeneral<int>   _dmode4_com;	
		AbsParmGeneral<int>   _dmode5_com;	
		AbsParmGeneral<int>   _dmode6_com;	

    APPMenu _debugMenu;

    AbsParmBool _showAVolumes;

    APPMenu _configMenu; // hold all the configuration menus

    void _initializeTalkTo();

    // Private functions to initialize those horrible parts of the
    // system that we still need that use globals and stuff
    // pulling in as little un-needed stuff as possible.
    void _pashaInitGlobals();
    void _pashaBeginJob();
    void _pashaDestroyGlobals();
    void _pashaInitEvent(AbsEvent* event);
    void _addCdfParticles();

  // Random number menu
  APPMenu _randomNumberMenu;
	
  // Initial random number seeds (2) (ignored if restoreState is requested
  // and stream name is found on engine map))
  AbsParmGeneral_long _randomSeed1;
  static const   long _defaultRandomSeed1;
  AbsParmGeneral_long _randomSeed2;
  static const   long _defaultRandomSeed2;

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
