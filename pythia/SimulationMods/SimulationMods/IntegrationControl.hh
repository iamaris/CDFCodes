#ifndef SIM_INTEGRATIONCONTROL_INCLUDED
#define SIM_INTEGRATIONCONTROL_INCLUDED 1

// Purpose: IntegrationControl is the AC++ module that "runs GEANT3"
// in order to integrate the material distribution in the CDF
// detector.
//
// This class is based directly on the class SimulationControl.
//
// $Id: IntegrationControl.hh,v 1.4 2001/05/09 19:52:46 paterno Exp $

#ifndef USE_CDFEDM2
#error The class IntegrationControl requires the use of EDM2
#endif 

#include <string>
#include <vector>
#include <map>

#include "Framework/APPModule.hh"
#include "Framework/AbsParmGeneral.hh"
#include "Framework/AbsParmBool.hh"
#include "geant_i/G3Stepdata.hh"
#include "SimulationMods/PathIntegral.hh"

// class AppFramework;
// class APPMenu;
 class CdfTangible;
// class CdfDetectorElement;
// class CdfDetectorNode;

namespace sim
{
	
	class IntegrationControl : public AppModule
	{
	public:
		// G3Stepdata is defined in geant_i/G3Stepdata.hh
		typedef G3Stepdata                       step_data_type;
		typedef step_data_type::PhysicalVolumeID pvid;
    typedef std::list<APPCommand*>           CommandList;
		typedef std::vector<PathIntegral> path_integral_collection;
	
		IntegrationControl(const char* const theName = "IntegrationControlMod", 
											 const char* const desc = "Integrate material");

		virtual ~IntegrationControl();
	
		AppResult beginJob( EventRecord* aJob );
		AppResult beginRun( EventRecord* aRun );
		AppResult event( EventRecord* anEvent );
		AppResult endRun( EventRecord* aRun );
		AppResult endJob( EventRecord* aJob );
		AppResult abortJob( EventRecord* aJob );

		// Add the information about the current step (as encoded in the
		// G3 common blocks in data), at the given location, to the
		// current PathIntegral.
		void updateCurrentIntegral(const pvid& location,
															 G3Stepdata& data);

	private:
		// Starting with the given TmpPVID as a root, walk the tangible
		// tree based at tang, recording in our map the PVID and address
		// (tangible pointer) for each tangible we encounter.
 		bool _recurseTangibles(G3::TmpPVID& id, const CdfTangible* tang);

		// Record the given PhysicalVolumeID as being associated with the
		// given tangible.
		void _associate(const pvid& id, const CdfTangible* tang);

 		// Add a new PathIntegral for the current "event" being tracked.
 		void _addNewPath();

		// Return a reference to the current path.
		PathIntegral& _currentPath();

		// (Re-) Initialize the collection of PathIntegrals, to be ready
		// for a new Event.
		void _initPaths();

		void _writePaths() const;

    void _initializeTalkTo();

    // Private functions to initialize those horrible parts of the
    // system that we still need that use globals and stuff
    // pulling in as little un-needed stuff as possible.
    void _pashaInitGlobals();
    void _pashaBeginJob();
    void _pashaDestroyGlobals();
    void _pashaInitEvent(AbsEvent* event);


		AbsParmGeneral<float> _pvSigmaZ;
    AbsParmGeneral<float> _pvSigmaX;
		AbsParmGeneral<float> _pvSigmaT; 

		AbsParmGeneral<double> _bmagnt;
		AbsParmGeneral<int>    debug_level;
		bool							     is_made;
		
    // We don't own this: we claim it from GeometryMenuManager
		// Not sure if this is actually used in IntegrationControl; it is
		// carried over from SimulationControl.
    APPMenu* _detectorMenu;

    // Hold pointers to all dynamically created commands so we can
    // destroy them at the end.

    CommandList _dynamicCommands;
    APPMenu     _debugMenu;
    AbsParmBool _showAVolumes;

		// Associate each PhysicalVolumeID with its tangible
		typedef std::map<pvid, const CdfTangible*> tangible_map;
		tangible_map  _tangibles;

 		// We keep one PathIntegral for each particle tracked in an
 		// event. This is a temporary measure, until we figure out what we
 		// really want to do with them. They're pretty big, so we can't
 		// expect to hold too many.
		path_integral_collection  _paths;

	}; // class IntegrationControl

	inline
	void
	IntegrationControl::_initPaths()
	{
		path_integral_collection temp(1); // contains one empty PathIntegral
		_paths.swap(temp);
	}

 	inline
 	void
 	IntegrationControl::_addNewPath()
 	{
		_paths.push_back(PathIntegral());
 	}

	// The path at the back of the vector is the one that is
	// "current". Since we are always careful to make sure the vector is
	// not empty, it is safe to call this. (This is not strictly true;
	// it is not safe to call this until _init() has been called, during
	// the event() method).
	inline
	PathIntegral&
	IntegrationControl::_currentPath()
	{
		return _paths.back();
	}


} // namespace sim

#endif // #ifndef SIM_INTEGRATIONCONTROL_INCLUDED

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
