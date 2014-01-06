//
// Purpose: Implementation of the IntegrationControl class.
//
// $Id: IntegrationControl.cc,v 1.13 2001/06/05 17:07:39 lena Exp $
//
// This implementation was copied from SimulationControl.cc 
// (version 1.22, dated  2001/03/20 18:27:57).
//

/*
	02/2001 jbk: added checks and error logger for inconsistancies between
	the sensitive detector info in gcsets and the current volume in volu.
	This is in response to problems in the CLC with Cerenkov particles.
	Also added the debug flag to the step procedure - value of 7 or
	higher prints Geant info at each step.

	11 Apr 2001 mfp: Enhanced debug output. Each output is produced when
	the flag "repulsiveDebugLevel" is equal to or higher than the given
	value.

	0: no output. This is the default value.

  2: In updateCurrentIntegral, write out information about the current
  step, if something "strange" is detected. Strange currently means:
	(1) no tangible is found; (2) material is "mixed"

	5: Write out when we enter updateCurrentIntegral, and write out the
	information about the IntegrationData just composed for the current
	step. Write out when we reach the end of tracking for the current
	particle.

	6: In updateCurrentIntegral, after the new step has been appended,
	write out the end of the PathIntegral. We don't write out the whole
	thing, 'cause it is too big.

	7: At end of IChandleStep(), prinout from gpcxyz_(). "Prints
	tracking and physics information after the current step."

*/

#include "SimulationMods/IntegrationControl.hh"

#include <iostream>
#include <cassert>
#include <iterator>
#include <algorithm>
#include <fstream> // for debugging

#include "ErrorLogger/ErrorLog.h"
#include "Framework/APPFramework.hh"

#include "GeometryBase/CdfDetector.hh"
#include "GeometryBase/CdfDetectorNode.hh"
#include "GeometryBase/CdfLogicalVolume.hh"
#include "GeometryBase/CdfPhysicalVolume.hh"
#include "GeometryBase/CdfTangible.hh"
#include "GeometryBase/CdfMaterial.hh"

#include "geant_i/G3Callbacks.hh"
#include "geant_i/G3Stepdata.hh"
#include "geant_i/G3StepdataSupport.hh" // for TmpPVID
#include "geant_i/Geant3GeometryInterface.hh"
#include "geant_i/TGeant3.h"
#include "SimulationUtils/McEvent.hh"

#include "SimulationMods/G3Data.hh"
#include "SimulationMods/Debug.hh"
#include "SimulationMods/ConfigCmd.hh"
#include "SimulationMods/geant_services.hh"

#include "GeometryBase/Menu/AbsDetectorNodeSelector.hh"


// The following is TEMPORARY and MUST BE CHANGED...
// This was added to SimulationControl for interaction with Gflash. It
// is not clear if this will be needed at all in IntegrationControl.
//#include "CalorGeometry/CalorMediaMap.hh" 

using namespace G3;
using namespace std;

// Un-named namespace, to hide these details from other compilation
// units.
namespace {

	// Return true if we're done tracking the current particle
	inline
	bool 
	current_particle_done()
	{
		int istop = TGeant3::Instance()->Gctrak()->ISTOP;
		return istop != 0;
	}

	// Return true if there are more particles to process
	inline
	bool 
	event_has_more_particles()
	{
		int size_of_event =  TGeant3::Instance()->Gcnum()->NTRACK;
		int current_particle = TGeant3::Instance()->Gckine()->ITRA;
		return ( current_particle = size_of_event );			
	}


	// Make Geant3 do its stuff, which ends up invoking our callback,
	// IChandleStep.
	void pokeGeant3() 
	{
    TGeant3 *g3 = TGeant3::Instance();
		assert ( g3 != 0 );
    g3->Gtrigc(); // Clear
    g3->Gtrigi(); // Initialize
    g3->Gtrig();  // Simulate
	}


	//
	// File-scope data
	//

	ErrorLog elog("IntegrationControl");
	int repulsiveDebugLevel = 0;

	// This is the pointer we use to pass the IntegrationControl module
	// currently active to the GEANT3 callback that needs to invoke its
	// functions.
	sim::IntegrationControl* local_control_pointer = 0;

} // un-named namespace

void geantGukine();


//
// This is the function which GEANT3 will call as ``GUSTEP''. It does
// is "global" GEANT stuff, then hands off to IntegrationManager.
//
void IChandleStep() 
{
	assert ( local_control_pointer != 0 );

	// Get the G3Stepdata thing
	G3Stepdata* data = G3Stepdata::instance();
  TGeant3* g3      = data->g3();

  Gctrak_t *gct = g3->Gctrak();

	// Skip tracing particles decayed by generator or unknown to GEANT;
	// these particles are not producing hits but being traced.
  if ( strstr((char*)&g3->Gckine()->NAPART,"dummy") ) 
		{
			gct->ISTOP = 1;
			return;
		}

  // Set current position of traced MC particle
  TSimParticle* particle = g3->CurrentParticle();
  assert(particle);
  particle->SetLastPoint(gct->VECT[0], 
                         gct->VECT[1], 
                         gct->VECT[2], 
                         gct->TOFG);

  // Increment the particle's radiation length as reported by G3
  assert ( g3->Gcmate() );
  assert ( g3->Gcmate()->RADL != 0.0 );
  particle->AddRadLength(gct->STEP/gGcmate->RADL);

  // No need to trace secondaries for the task of integration.
  Gcking_t *king = g3->Gcking();
	for (int i = 0; i < king->NGKINE; ++i) king->IFLGK[i]=-1;

  int np = McEvent::Instance()->NParticles();

  // For those particles that have been saved, save the new particle
  // number (from the CDF side) as a user word
	//  int ip=np;

 	Gcsets_t * gcs = g3->Gcsets();
	if(repulsiveDebugLevel >= 7) gpcxyz_();

	if(repulsiveDebugLevel >= 7) 
		{
			int mc_event_particle_num = 
				TGeant3::Instance()->CurrentParticle()->Number();
			
			int gen_list_particle_num = 
				TGeant3::Instance()->CurrentParticle()->GenNumber();
			
			int jkine_particle_num = 
				TGeant3::Instance()->CurrentParticle()->GeantNumber();
			
			int istop = TGeant3::Instance()->Gctrak()->ISTOP;
			
			cout << "Particle numbers:"
					 << "\nmc_event #: " << mc_event_particle_num
					 << "\ngen_list #: " << gen_list_particle_num
					 << "\njkine #   : " << jkine_particle_num
					 << endl;
			cout << "ISTOP: " << istop << endl;
		} // 	if(repulsiveDebugLevel >= 7) 
	
	// Now actually integrate...
	local_control_pointer->updateCurrentIntegral(data->location(),
																							 *data);

} // void IChandleStep()

namespace sim 
{
  
  IntegrationControl::IntegrationControl(const char* const name, 
																				 const char* const desc)
    :
    AppModule(name,desc),
    is_made(false),
    debug_level("DebugLevel", this, 0),
		_pvSigmaZ("pv_sigma_z",this,0.),
		_pvSigmaX("pv_sigma_trans",this,0.),
		_pvSigmaT("pv_sigma_time",this,0.),
		_bmagnt("bmagnt", this, 14.116),
		_showAVolumes("showActiveVolumes",this,false),
		_tangibles(),
		_paths()

  {
    _initializeTalkTo();
    _pashaInitGlobals();

		// This will force calling uginit and initialization of gcphys
    Geant3GeometryInterface* geom = Geant3GeometryInterface::instance();
    geom->callUginit();
  }

  IntegrationControl::~IntegrationControl() 
	{
    _pashaDestroyGlobals();
		CommandList::const_iterator i, end = _dynamicCommands.end();
		for ( i = _dynamicCommands.begin(); i != end; ++i) delete (*i);
  }

	
	
  AppResult IntegrationControl::beginRun( AbsEvent* aRun )
  {
    McEvent::Instance()->SetBeamSigmaZ(_pvSigmaZ.value());
    McEvent::Instance()->SetBeamSigmaX(_pvSigmaX.value());
    McEvent::Instance()->SetBeamSigmaY(_pvSigmaX.value()); // assume beam is round
    McEvent::Instance()->SetSigmaT(_pvSigmaT.value());
		// Start by setting parameters in mcEvent
    if (is_made == true)
      {
        errlog(ELfatal,"IntegrationControl")
          << "Run Change in IntegrationControl and simluator is "
          << "already initialized (not supported)."
          << endmsg;
        return AppResult::ERROR;
      }

    AppResult rc = AppResult::OK;	

		// Go through the tangible tree, and record all the tangibles and
		// their PVIDs in our map.
		const CdfTangible* top = CdfDetector::instance()->worldTangible();
		assert ( top != 0 );
		TmpPVID root;

		is_made = _recurseTangibles ( root, top );
		assert ( root.empty() );

    return rc;
  }

	void writeDebugOutput( const TmpPVID& g3id )
	{
		TmpPVID::const_iterator i = g3id.begin();
		TmpPVID::const_iterator end = g3id.end();

		for ( ; i!=g3id.end(); ++i) 
			if (i!=g3id.begin()) cout << '/' << *i;

		cout << "\n***" << IntegrationControl::pvid(g3id) << "***\n";
	}

	void
	IntegrationControl::updateCurrentIntegral(const pvid& location,
																						G3Stepdata& data)
	{
		if ( repulsiveDebugLevel >= 5 )
			{
				cout << "Entered updateCurrentIntegral: location is: " 
						 << location << '\n';
			}
		// Find the CdfTangible for this step
		tangible_map::iterator ipiece = _tangibles.find(location);
		const CdfTangible* piece = ( ipiece == _tangibles.end()
																 ? 0
																 : (*ipiece).second);
		IntegrationData step;

		// Fill the struct with the correct data...
		step.numSteps = 1;
		TGeant3* g3 = data.g3();
		double currentStepLength = g3->Gctrak()->STEP;
		double currentRadl = g3->Gcmate()->RADL;
		double currentAbsl = g3->Gcmate()->ABSL;
		int    fieldType = g3->Gctmed()->IFIELD;
		double currentFieldStrength = g3->Gctmed()->FIELDM;
		double currentX = g3->Gctrak()->VECT[0];
		double currentY = g3->Gctrak()->VECT[1];
		double currentZ = g3->Gctrak()->VECT[2];
		HepPoint3D endPos(currentX, currentY, currentZ);
		string material("unk");
		// This is a beauty...
		if (piece) 
			material = piece->physicalVolume()->
				getLogicalVolume()->getMaterial()->getName();

			
		// Right now, we're not bothering to make sure that these numbers
		// are nonzero -- what can we do if one of them is zero? The
		// program would be completely broken.
		step.radiationLength = currentStepLength/currentRadl;
		step.interactionLength = currentStepLength/currentAbsl;
		step.BdL = currentFieldStrength * currentStepLength;
		step.pathLength = currentStepLength;

		if ( repulsiveDebugLevel >= 2 )
			{
				if ( piece == 0 )
					cout << "... No tangible for location: " << location << '\n';
				
				if ( material == "mixed")
					cout << "... Material is mixed for location: (" << location << ")\n";
			}

		if ( repulsiveDebugLevel >= 5 )
			{
				cout << "appending IntegrationData: " << step << '\n';
			}
		
		// **********
		_currentPath().append(location, piece, endPos, step);

		if ( repulsiveDebugLevel >= 6 )
			{
				PathIntegral::index_t last_idx = _currentPath().numLegs() - 1;
				cout << "leg(" << last_idx << ")              : "
						 << _currentPath().leg(last_idx) << endl;
				cout << "coordinates of start: " << _currentPath().startPoint() << endl;
				cout << "coordinates of end:   " << _currentPath().endPoint() << endl;
				cout << "integral so far:      " << _currentPath().integral() << endl;
			}

		// If we've finished with the current particle, but more particles
		// exist in the current event, then we need to prepare for the next
		// track.
		if ( current_particle_done() &&
				 event_has_more_particles() )
			{
				if ( repulsiveDebugLevel >= 5 )
					{
						cout << "done with this particle, and more to go in the event\n";
						cout << "pushing new path for next track\n";
					}
				_addNewPath();
			}
		
		if ( repulsiveDebugLevel >= 2 )
			cout << "Exiting  updateCurrentIntegral\n";

	}
	
	bool
	IntegrationControl::_recurseTangibles(TmpPVID& g3id,
																				const CdfTangible* tang)
  {
		assert ( tang != 0 );
		const CdfPhysicalVolume* vol = tang->physicalVolume();
		assert ( vol != 0 );
		g3id.push_back(PVIDNode(vol->getLogicalShortName(),
														vol->GetCopyNo()));

    if ( debug_level.value() >= 10 ) writeDebugOutput(g3id);

		_associate(pvid(g3id), tang);

		CdfTangible::ConstIterator i = tang->beginDaughters();
		CdfTangible::ConstIterator end = tang->endDaughters();

		bool rc = true;
		while ( rc && (i != end) ) 
			{
				rc = _recurseTangibles(g3id, *i);
				++i;
			};

    g3id.pop_back();
    return rc;
  }

	void
	IntegrationControl::_associate(const pvid& id, 
																 const CdfTangible* tang)
	{
		assert ( tang != 0 );
		_tangibles[id] = tang;
	}

	AppResult IntegrationControl::event( EventRecord* anEvent ) 
	{
		// The following class exists to control the valid lifetime of our
		// file-scope pointer to the IntegrationControl module.
		//
		struct CPSentry
		{
			CPSentry(IntegrationControl* p) { local_control_pointer = p; }
			~CPSentry() { local_control_pointer = 0; }		
		};

		assert ( local_control_pointer == 0 );
		CPSentry sentry(this);
		
    // Initialize Pasha's global event data
    _pashaInitEvent(anEvent);

		repulsiveDebugLevel = debug_level.value();

		// Prepare the collection of paths for this Event.
		_initPaths();
		// Fire off GEANT3
		pokeGeant3();
		// Write out the results
		_writePaths();
				
		return AppResult::OK;
	}


  
  AppResult IntegrationControl::endRun( EventRecord* aRun ) 
	{
    return AppResult::OK;
  }
  
  AppResult IntegrationControl::beginJob( EventRecord* aJob ) {

    // Set callbacks
    G3Callbacks* callbacks = G3Callbacks::instance();
    callbacks->setGustep(&IChandleStep);
    callbacks->setGutrak(&syncWithG3ParticlesAndTrack);
    callbacks->setGukine(&geantGukine); // from gnt

    _pashaBeginJob();
    
		// Is the following really necessary, perhaps to make sure that
		// Geant3GeometryInterface is instantiated at this time? Or is
		// this a leftover from SimulationControl? If it is discovered to
		// be unnecessary, it should be removed.
		Geant3GeometryInterface* g3g = Geant3GeometryInterface::instance();
		assert ( g3g != 0 );

    return AppResult::OK;
  }
  
  AppResult IntegrationControl::endJob( EventRecord* aJob ) {
// 		cout << "IntegrationControl::endJob called; "
// 				 << "dump of PathIntegrals follows\n";
// 		cout << "There are: " 
// 				 << _paths.size()
// 				 << " paths in the dump\n";

// 		copy( _paths.begin(),
// 					_paths.end(),
// 					ostream_iterator<PathIntegral>(cout, "\n\n"));

// 		cout << "---\nDump of PathIntegrals completed\n";
			
    return AppResult::OK;
  }
  
  AppResult IntegrationControl::abortJob( EventRecord* aJob ) {
    return AppResult::OK;
  }

	void
	IntegrationControl::_writePaths() const
	{
		copy( _paths.begin(), _paths.end(),
					ostream_iterator<PathIntegral>(cout, "\n\n"));
	}


  
  
} // namespace sim

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
