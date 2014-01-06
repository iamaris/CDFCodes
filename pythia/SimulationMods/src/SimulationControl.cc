//
// Purpose: Implementation of the SimulationControl class.
//
// $Id: SimulationControl.cc,v 1.60 2007/05/04 18:42:02 syjun Exp $

/*
	02/2001 jbk: added checks and error logger for inconsistancies between
	the sensitive detector info in gcsets and the current volume in volu.
	This is in response to problems in the CLC with Cerenkov particles.
	Also added the debug flag to the step procedure - value of 7 or
	higher prints Geant info at each step.
*/

#include "SimulationMods/SimulationControl.hh"

#include <list>
#include <iostream>
#include <cassert>
using std::string;
using std::endl;
using std::cout;

#include "ErrorLogger/ErrorLog.h"
#include "Framework/APPFramework.hh"

#include "GeometryBase/CdfDetector.hh"
#include "GeometryBase/CdfDetectorNode.hh"
#include "GeometryBase/CdfLogicalVolume.hh"
#include "GeometryBase/CdfPhysicalVolume.hh"
#include "GeometryBase/CdfDetectorElement.hh"
#include "GeometryBase/CdfTangible.hh"
#include "GeometryBase/Bfield.hh"

#include "geant_i/G3Callbacks.hh"
#include "geant_i/Geant3GeometryInterface.hh"

#include "SimulationBase/SimulationManager.hh"
#include "SimulationBase/AbsEventdataFiller.hh"
#include "SimulationBase/AbsEDFManager.hh"

#include "SimulationMods/G3Data.hh"
#include "SimulationMods/Debug.hh"
#include "SimulationMods/ConfigCmd.hh"
#include "SimulationMods/SimSetup.hh"
#include "SimulationMods/VolumeNamePrinter.hh"
#include "SimulationMods/geant_services.hh"
#include "SimulationUtils/McEvent.hh"
#include "r_n/CdfRn.hh"
#include "ParticleDB/ParticleDb.hh"

#include "GeometryBase/Menu/AbsDetectorNodeSelector.hh"

// The following is TEMPORARY and MUST BE CHANGED...
#include "CalorGeometry/CalorMediaMap.hh" 

#include "SimulationUtils/ShowerInfoMap.hh"

using namespace G3;
using sim::ShowerInfoMap;

namespace {

ErrorLog elog("SimulationControl:handleStep");

	sim::G3SimMgr* repulsiveLocalPointer = NULL;
	int repulsiveDebugLevel = 0;

} // un-named namespace

const long sim::SimulationControl::_defaultRandomSeed1 = 922813451;
const long sim::SimulationControl::_defaultRandomSeed2 = 356767476;

extern "C" {
	void gpcxyz_();
	void gsdk_(int*,float*,int*);
	void gspart_(int*, char*, int*, float*, float*, float*, float*, int*, int);
	void truncString( char* const p, const int l, const std::string& s) {
		int m = s.size();
		if (m>l) m=l;
		char *tp;
		const char *fp;
		int i;
		for (tp=p,fp=s.c_str(),i=0; i<m; ++tp,++fp,++i) *tp=*fp;
		for (;i<l;++tp,++i) *tp=' ';
		*tp='\0';
	}
}

////////////////////////////////////////////////////////////////////////
// From gnt: the callback to load the particles into G3
void geantGukine();
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// This is the function which GEANT3 will call as ``GUTRAK''
#include "geant_i/G3Stepdata.hh"
#include "geant_i/TGeant3.h"
////////////////////////////////////////////////////////////////////////

#include "CotGeometry/CotDetectorNode.hh"

// EMB changed z exit point of COT to inner edge of endplate:
// zHalfLength() -> zHalfLength()-4.12.  10/16/01.
inline bool _insideCot(double radius, double z) {
  return (radius < CotDetectorNode::outerRadius() 
          && fabs(z) < CotDetectorNode::zHalfLength()-4.12 );
}

inline bool _exitCot(double radius, double z) {
     return ( (fabs(CotDetectorNode::outerRadius()-radius)<1.e-1) || 
           (fabs(CotDetectorNode::zHalfLength()-4.12-fabs(z))<1.e-1) ) ;
}

inline bool _momNonZero(float p[]) {
  return (p[0]!=0.0 || p[1]!=0.0 || p[2]!=0.0);
}


////////////////////////////////////////////////////////////////////////
// This is the function which GEANT3 will call as ``GUSTEP''
void handleStep() {
	assert (repulsiveLocalPointer != NULL);
	// Get the G3Stepdata thing
	G3Stepdata* data = G3Stepdata::instance();
  assert(data);
  TGeant3* g3      = data->g3();
  assert(g3);

#ifndef NDEBUG
  // When debugging tracing, set this to false in your debugger to turn
  // off tracing of secondaries in GUSTEP
  static bool trace_secondaries = true;
#endif

  bool clcOK = false;

  Gctrak_t *gct = g3->Gctrak();
  assert(gct);


	// Skip tracing particles decayed by generator or unknown to GEANT;
	// these particles are not producing hits but being traced.

  if ( strstr((char*)&g3->Gckine()->NAPART,"dummy") ) {
		gct->ISTOP = 1;
		return;
	}


	// Stop tracing particle outside COT if in fastTrack mode.

	SimSetup* setup = SimSetup::Instance();
	if ( setup->getFastTrack() ) {
		if ( !_insideCot( sqrt(gct->VECT[0]*gct->VECT[0]+gct->VECT[1]*gct->VECT[1]),
											gct->VECT[2]
											) ) {
			gct->ISTOP = 1;
			if ( repulsiveDebugLevel == 21 ) {
				std::cout << "SimulationControl: fastTrack mode: outside COT - tracking stopped";
				std::cout << std::endl;
			}
			return;		
		}	
	}


//   // Debug only: print where we are
//   std::cout << "handleStep in volume: ";
//   data->location().print(std::cout);
//   std::cout << " at (" << gct->VECT[0]
//             << "," << gct->VECT[1]
//             << "," << gct->VECT[2]
//             << ")" << std::endl;

  // Set current position of traced MC particle
  TSimParticle* particle = g3->CurrentParticle();
  assert(particle);
  particle->SetLastPoint(gct->VECT[0], 
                         gct->VECT[1], 
                         gct->VECT[2], 
                         gct->TOFG);

  // Increment the particle's radiation length as reported by G3
  assert(g3->Gcmate());
  assert(g3->Gcmate()->RADL);
  particle->AddRadLength(gct->STEP/gGcmate->RADL);

  // Trace secondaries. We are not interested in backsplash secondaries
  // so we only record secondaries for particles produced within the COT
  // volume
  int nSecondaries = 0;
  Gcking_t *king = g3->Gcking();
  assert(king);

#ifndef NDEBUG
  if (trace_secondaries) {
#endif
    Gckin3_t *k3 = g3->Gckin3();
    assert(k3);
    for (int i=0; i<king->NGKINE; ++i) {
      king->IFLGK[i] = 0;
      // Assemble the radius and z from the GCKIN3 COMMON block
      float* pos = k3->GPOS[i];
      double radius = sqrt(pos[0]*pos[0]+pos[1]*pos[1]);
      double z = pos[2];


      // Trace secondaries only if:
			if ( setup->getTraceSecForClc() &&
					 (radius < 50. ) &&
					 (abs(pos[2]) < 400. ) &&
					 ( _momNonZero(king->GKIN[i]))
					 ) {
        clcOK = true;
				//				if(particle->Number() >= 0) {            
					//					king->IFLGK[i]=1; // magic: tell G3 to trace secondaries
					//					++nSecondaries;
				//				}		
			}
			else
      if (_insideCot(radius,z) && 
          _momNonZero(king->GKIN[i])) { 
          // Mother of particle has been produced in tracking volume
          if(particle->Number() >= 0) {            
             king->IFLGK[i]=1; // magic: tell G3 to trace secondaries
             ++nSecondaries;
					}
      }
    }
#ifndef NDEBUG    
  } else { // don't trace secondaries at all
    for (int i=0; i<king->NGKINE; ++i) {
      king->IFLGK[i]=-1;
    }
  }
#endif

  int np = McEvent::Instance()->NParticles();
  // Update our list of particles to match Geant's
  if (nSecondaries>0) {
    McVertex* v = McEvent::Instance()->NewVertex(nSecondaries);
    if (v->ParentNumber() == -1) {
      // differentiate between secondary and primary vertices: secondary
      // vertices with unknown parent have parent code -2
      v->SetParentNumber(-2);
    }
    int iv = v->Number();
    int n_added=0;
    Gcnum_t *gcn = g3->Gcnum();
    assert(gcn);
    for (int i=0; i<king->NGKINE; ++i) {
      // set particle's 4-momentum and type `iv' is particle's vertex
      // number in McEvent vertex list
      if (king->IFLGK[i] == 1) {
        McParticle *p = McEvent::Instance()->NewParticle();
        p->InitWithGkin(king->GKIN[i],iv);
        p->SetGeantNumber(gcn->NTRACK+n_added);
        ++n_added;
      }  
    }
  }
	
  // Tell Geant to add the temporary particles into JKINE
	
	// The following is TEMPORARY and MUST be changed...
	CalorMediaMap* map = CalorMediaMap::instance();
	int icalo = map->findCalNum(g3->Gctmed()->NUMED);
	// EMB added "||icalo==10" to track secondaries from PPR volume.
	if (icalo==0 || icalo==10) {
		g3->Gsking(0);
	}
	//  Shouldn't keep Cherenkov photons except in clc...
	if(clcOK)  g3->Gskpho(0);
	
  // For those particles that have been saved, save the new particle
  // number (from the CDF side) as a user word
  int ip=np;
  // User word array for G3
  float ubuf[1];
  for (int i=0; i<king->NGKINE; ++i) {
    if (king->IFLGK[i]>0) {
      McParticle* p=McEvent::Instance()->Particle(ip);
      ubuf[0]=p->Number();
      int loc=0; // gets modified by GSKINU call
      g3->Gskinu(p->GeantNumber()+1,1,ubuf,loc);
      ++ip;
    }
  }

  // This could be a problem for calorimeter only. 
  // Let's pass this cut to the calorimeter digitizer.

  // OK, if we're now dealing with a stopped particle, bail out:
  // Not sure whether this should be && or ||. Check with Pasha 2000/04/25
	if (gct->ISTOP!=0 // tracking stopped
      && gct->VECT[6]==0. // zero momentum
				)  return;
	//		{ 
	//			std::cout << " handleStep(): gct->ISTOP!=0 && gct->VECT[6]==0. "<< std::endl;
	//		}


	// As we exit the COT, fill the map to be used for calorimeter later...
  if(gct->INWVOL==2 ) {
    double radius=sqrt(gct->VECT[0]*gct->VECT[0]+gct->VECT[1]*gct->VECT[1]);
		if(_exitCot(radius,gct->VECT[2]) && (g3->Gckine()->ISTAK==0)) {
			//      const CotDetectorNode* cot = CdfDetector::instance()->getCotDetector();
      Hep3Vector pos(gct->VECT[0],gct->VECT[1],gct->VECT[2]);
      Hep3Vector dir(gct->VECT[3],gct->VECT[4],gct->VECT[5]);
			// EMB added charge to ShowerInfoMap. 10/16/01.
			ShowerInfoMap::instance()->addTrack(g3->Gckine()->ITRA,pos,dir,
																					gct->VECT[6], g3->Gckine()->CHARGE);
		}
	}

 	// If this isn't an active volume, we don't bother dispatching
 	Gcsets_t * gcs = g3->Gcsets();
  assert(gcs);

	// old way
  // if (gcs->ISET==0) return;

	if(repulsiveDebugLevel > 7) gpcxyz_();
	// this check is probably overkill
	if (g3->Gclink()->JSET > 0 && g3->Gctmed()->ISVOL > 0 && gcs->ISET !=0)
	{
		// we should probably leave this is and log a message each
		// time the sensitive detector is outside the volume we are
		// currently in.

		Gcvolu_t* volu = g3->Gcvolu();
		int* vname = &g3->Gcvolu()->NAMES[volu->NLEVEL-1];
		int* ihdet = &g3->Gcsets()->IHDET;

		if(*ihdet != *vname)
		{
			elog(ELwarning,"gustep")
				<< "sensitive detector / volume inconsistancy: "
				<< sim::VolumeNamePrinter(ihdet) 
				<< "!=" 
				<< sim::VolumeNamePrinter(vname) << "\n"
				<< "Full path from volu = " << data->location() << "\n"
				<< "The position (of the particle) reported in gcvolu is not "
				<< "within the sensitive detector that gcsets reports.\n"
				<< "No dispatching will occur at this step."
				<< endmsg;
#if 0
			// tried to make the call to correct, not good idea
			std::cout << "Attempting to correct it" << std::endl;
			gfinds_();
			vname = &g3->Gcvolu()->NAMES[volu->NLEVEL-1];
			ihdet = &g3->Gcsets()->IHDET;

			if(*ihdet!=*vname)
			{
				std::cout << "Unable to correct it." << std::endl;
				return;
			}
#endif
		}
		else
		{
			// Dispatch for this step. Note that we're doing nothing special
			// according to the charge or characteristics of the
			// particle. Whatever detector ends up getting called is supposed to
			// do the checking itself.

 			repulsiveLocalPointer->handleStep(*data, data->location());
		}
	}
}
////////////////////////////////////////////////////////////////////////

namespace sim {
  
  SimulationControl::SimulationControl(const char* const name, 
																			 const char* const desc)
    :
    AppModule(name,desc),
    is_made(false),
    debug_level("DebugLevel", this,0),
		_pvCentralX( "pv_central_x", this, 0., -10000., 10000.),
		_pvCentralY( "pv_central_y", this, 0., -10000., 10000.),
		_pvCentralZ( "pv_central_z", this, 0., -10000., 10000.),	  
    _pvSlope_dXdZ("pv_slope_dxdz",this,0.0,-10000., 10000.),
    _pvSlope_dYdZ("pv_slope_dydz",this,0.0,-10000., 10000.),
    _pvSigmaZ("pv_sigma_z",this,30., -200., 200.),
  	_pvSigmaX("pv_sigma_trans",this,0.0025,0.,1.),
    _pvSigmaT("pv_sigma_time",this,2.0,0.,20.),
		_fastTrack("fastTrack",this,false),
		_hshPackage("hadronicShowerPackage",this,0),
		_traceSecForClc("traceSecForClc",this, 0),
    conf_cmd(0),
    process_cmd(new ProcessCmd(this)),
		gccuts_cmd(new GccutsCmd(this)),
    dump_cmd(new DumpFactoryCmd(this)) 
    , _showAVolumes("showActiveVolumes",this,false)
		, _randomSeed1("RandomSeed1",this,SimulationControl::_defaultRandomSeed1)
		, _randomSeed2("RandomSeed2",this,SimulationControl::_defaultRandomSeed2)
		, _itrtyp_com("ITRTYP",this,5,0,8)
		, _umass_com("Mass", this, 0.)
		, _ucharge_com("Charge", this, 0., -999., 999.)
		, _ulife_com("Lifetime",this, 10. )
		, _bratio1_com("Bratio1",this,0.)
		, _bratio2_com("Bratio2",this,0.)
		, _bratio3_com("Bratio3",this,0.)
		, _bratio4_com("Bratio4",this,0.)
		, _bratio5_com("Bratio5",this,0.)
		, _bratio6_com("Bratio6",this,0.)
		, _dmode1_com("Dmode1",this,0.)
		, _dmode2_com("Dmode2",this,0.)
		, _dmode3_com("Dmode3",this,0.)
		, _dmode4_com("Dmode4",this,0.)
		, _dmode5_com("Dmode5",this,0.)
		, _dmode6_com("Dmode6",this,0.)
  {
    _dynamicCommands.push_back(dump_cmd);
    conf_cmd=new ConfigCmd(this, 
                           mgr, 
                           config_map,
                           &_configMenu);
    assert(conf_cmd);
    _dynamicCommands.push_back(conf_cmd);
    _initializeTalkTo();

    _pashaInitGlobals();

		// This will force calling uginit and initialization of gcphys
    Geant3GeometryInterface* geom = Geant3GeometryInterface::instance();
    geom->callUginit();
  }

  SimulationControl::~SimulationControl() {
    _pashaDestroyGlobals();
    for (CommandList::const_iterator i=_dynamicCommands.begin();
         i!=_dynamicCommands.end(); ++i) {
      delete (*i);
    }
  }
	
  AppResult SimulationControl::beginRun( AbsEvent* aRun )
  {

		// If more than one run per job, don't reinitialize
		//
    if (is_made == true)        return AppResult::OK;

	


		//-->--> GEANT settings
		
		TGeant3::Instance()->SetHadronicShowerPackage(_hshPackage.value());

		float dummy = 0.;
		int   lendummy  = 0;   
		char particleName[21];
		int   g3_code = ParticleDb::GEANT_USER_PART;
		int itrtyp = _itrtyp_com.value();
		float umass = _umass_com.value();
		float ulife = _ulife_com.value();
		float ucharge = _ucharge_com.value();
		std::string  partName = "GEANT_USER_PARTICLE";
		truncString(particleName,20,partName);
		gspart_( &g3_code, 
						 particleName, 
						 &itrtyp,
						 &umass,
						 &ucharge,
						 &ulife,
						 &dummy,
						 &lendummy,
						 20);
		float bratio[6] = {_bratio1_com.value(),
											 _bratio2_com.value(),
											 _bratio3_com.value(),
											 _bratio4_com.value(),
											 _bratio5_com.value(),
											 _bratio6_com.value()};
		int dmode[6]    = {_dmode1_com.value(),
											 _dmode2_com.value(),
											 _dmode3_com.value(),
											 _dmode4_com.value(),
											 _dmode5_com.value(),
											 _dmode6_com.value()};
		gsdk_(&g3_code, bratio, dmode ) ; 

		//-->--> Start by setting parameters in mcEvent

    McEvent::Instance()->SetBeamCentralX(_pvCentralX.value());
    McEvent::Instance()->SetBeamCentralY(_pvCentralY.value());
    McEvent::Instance()->SetBeamCentralZ(_pvCentralZ.value());

    McEvent::Instance()->SetBeamSlope_dXdZ(_pvSlope_dXdZ.value());
    McEvent::Instance()->SetBeamSlope_dYdZ(_pvSlope_dYdZ.value());

    McEvent::Instance()->SetBeamSigmaZ(_pvSigmaZ.value());
    McEvent::Instance()->SetBeamSigmaX(_pvSigmaX.value());
    McEvent::Instance()->SetBeamSigmaY(_pvSigmaX.value()); // assume beam is round
    McEvent::Instance()->SetSigmaT(_pvSigmaT.value());

    AppResult rc = AppResult::OK;
		DNPtrList activeDetectors;
		_getActiveDNs(activeDetectors);
		DNPtrList::const_iterator it, end = activeDetectors.end();
		for (it = activeDetectors.begin(); it != end; ++it)
			{
				CdfDetectorNode::DEConstIterator currentDE;
				CdfDetectorNode::DEConstIterator endDE = 
					(*it)->endingDetectorElement();
				for ( currentDE = (*it)->beginningDetectorElement();
							currentDE != endDE;
							++currentDE)
					{
						rc = processDE(*currentDE);
						if (rc != AppResult::OK) return rc;
					}
			}
//     CdfDetector* det = CdfDetector::instance();
	
//     for(CdfDetector::ConstIterator i = det->beginningChild();
//         i!=det->endingChild();
//         ++i)
//       {
//         for(CdfDetectorNode::DEConstIterator n = 
//               (*i)->beginningDetectorElement();
//             n!=(*i)->endingDetectorElement();
//             ++n)
//           {
//             rc = processDE(*n);
					
//             // blow out quick upon error
//             if(rc!=AppResult::OK) return rc;
//           }
//       }

		is_made = (rc == AppResult::OK);

		// Configure all digitizers
		mgr.configureAll();

		SimSetup* setup = SimSetup::Instance();

		setup->setFastTrack( _fastTrack.value() );
		setup->setTraceSecForClc( _traceSecForClc.value() );

    // DEBUG: print manager information
    if (debug_level.value()>=10) {
      mgr.print(std::cout);
      mgr.dumpPvidMap(std::cout);
    }
    return rc;
  }
	
  AppResult SimulationControl::processDE(const CdfDetectorElement* de)
  {
    // locate list of config entries for this detector element
    G3Factory* fact = G3Factory::instance();
    ConfigInfoMap::iterator cit = config_map.find(de->elementType());
	
    if(cit==config_map.end() || cit->second.empty()==true)
      {
        errlog(ELfatal,"sim")
          << "No digitizer defined for detector element: "
          << de->elementType() 
					<< ".There must be a digitizer defined for every detector "
					<< "element, so that the system can call the correct digitizer."
					<< "\n\nThis SimulationControl only knows about the following "
					<< "detector element types:\n\n"
					<< config_map
          << endmsg;
        return AppResult::ERROR;
      }
	
    if(cit->second.size()>1)
      {
        errlog(ELwarning,"sim") 
          << "Attempt to use unsupported feature.\n"
          << "Only one digitizer can be active for a detector element.\n"
          << "detector element: " << cit->first << " has "
          << cit->second.size() << " digitizers defined.\n"
          << "Only first one will be used."
          << endmsg;
      }

    se_type* se = 0;

    ConfigInfoList::iterator iit = cit->second.begin();
		//    string uni = iit->digi_class + iit->group_name;
    string uni = iit->group_name + '_' + de->elementType();
    APPMenu* menu = mgr.findMenu(uni);

    if(menu==0)
      {
        errlog(ELfatal,"sim")
          << "No configuration menu found for detector element: "
          << cit->first << ", digi class " << iit->digi_class
          << ", group " << iit->group_name
					<< ".\nThe key we looked up is:"
					<< "\n\t" << uni
					<< "\nThe current status of the SimulationManager is:\n"
					<< mgr
          << endmsg;
        return AppResult::ERROR; // in case fatal message doesn't abort
      }

    AbsEDFManager* edf = mgr.findEDFManager(iit->group_name);

    if(edf == NULL)
      {
        edf = fact->makeEDFManager(iit->digi_class,iit->group_name);
				if (edf == NULL)
					{
						errlog(ELfatal, "sim")
							<< "Could not make EDFManager for digi class: "
							<< iit->digi_class
							<< ", group name: "
							<< iit->group_name
							<< "\nThe current status of the SimulationManager is:\n"
							<< mgr
							<< endmsg;

						return AppResult::ERROR; // in case fatal message doesn't abort
					}

        mgr.addEDFManager(iit->group_name,edf);
      }

    se = fact->makeDigi(iit->digi_class,iit->group_name,menu,de);
	
    if(se == NULL)
      {
        errlog(ELfatal,"sim")
          << "Could not make SimElement for digi of type " << iit->digi_class
					<< "\nThe current status of the SimulationManager is:\n"
					<< mgr
					<< endmsg;

        return AppResult::ERROR; // in case fatal message doesn't abort
      }

    mgr.addSimElement(de->instanceID(),se);

    AbsEventdataFiller* filler = dynamic_cast<AbsEventdataFiller*>(se);
    assert(filler!=0);

    edf->addFiller(filler);

    AppResult rc = AppResult::OK;
    // populate the map of tangible ID to sim element

    TmpPVID g3id;

    // start at pv and walkd up from there
    const CdfPhysicalVolume* pv = de->geom()->physicalVolume();
    if (debug_level.value()>=10) {
      std::cout << "ProcessDE: building TmpPVID" << std::endl;
    }
    while( (pv=pv->getParentPV())!=0 )
      {
        g3id.push_front(PVIDNode(pv->getLogicalShortName(), pv->GetCopyNo()));
        if (debug_level.value()>=10) {
          for (TmpPVID::const_iterator i=g3id.begin();
                 i!=g3id.end(); ++i) {
            if (i!=g3id.begin()) std::cout << '/';
            std::cout << *i;
          }
          std::cout << std::endl;
        }
      }
    
    if((rc=recurseTangible(g3id,de,de->geom())) != AppResult::OK) return rc;

    return AppResult::OK;
  }

  AppResult SimulationControl::recurseTangible(TmpPVID& g3id,
                                               const CdfDetectorElement* de,
                                               const CdfTangible* tang)
  {
    AppResult rc = AppResult::OK;

    // Jim: you put tang->instanceID() where addSimElement() expects an
    // se_type*. What do you *really* mean? 2000/02/16 CG
    // const deid&, const pvid&

    // could put this in loop, then it would work for the highest level
    // or detector element level

    const CdfPhysicalVolume* vol = tang->physicalVolume();
    g3id.push_back(PVIDNode(vol->getLogicalShortName(),tang->getCopyNumber()));
    if (debug_level.value()>=10) {
      for (TmpPVID::const_iterator i=g3id.begin();
           i!=g3id.end(); ++i) {
        if (i!=g3id.begin()) std::cout << '/';
        std::cout << *i;
      }
      std::cout << std::endl;
      std::cout << "***" << pvid(g3id) << "***" << std::endl;
    }
    mgr.associate(de->instanceID(), pvid(g3id), tang );

    for(CdfTangible::ConstIterator i = tang->beginDaughters();
        i!=tang->endDaughters() && rc==AppResult::OK;
        ++i)
      {
        rc=recurseTangible(g3id,de,*i);
      }

    g3id.pop_back();
    return rc;
  }

	AppResult SimulationControl::event( EventRecord* anEvent ) {
		assert (repulsiveLocalPointer == NULL); // safety check
		
		// Clean up from previous event
		mgr.clearAll();

		// Get ready for the new event, creating all eventdata objects
		mgr.prepareAll();

    // Initialize Pasha's global event data
    _pashaInitEvent(anEvent);

		// Clear shower information
		ShowerInfoMap::instance()->clear();
		
		repulsiveLocalPointer = &mgr; // needed by handleStep()
		repulsiveDebugLevel = debug_level.value();
		pokeGeant3();
		// so it won't be used at the wrong time, and so that two
		// SimulationControl instances don't interfere, we make sure to
		// clear this pointer after every use.
		repulsiveLocalPointer = NULL; 
		
		if (! mgr.completeAll() ) return AppResult::ERROR;
		if (! mgr.populateEvent(anEvent) ) return AppResult::ERROR;
		
		// Now, make sure the Monte Carlo truth (OBSP OBSV) is written out
    McEvent::Instance()->WriteObspObsvBanks();
		return AppResult::OK;
	}

	void SimulationControl::pokeGeant3() {
    
//     std::cout << "pokeGeant3() entered" << std::endl;
    TGeant3 *g3 = TGeant3::Instance();
    assert(g3);
    g3->Gtrigc(); // Clear
    g3->Gtrigi(); // Initialize
    g3->Gtrig(); // Simulate
// 		std::cout << "pokeGeant3() exited" << std::endl;
	}

	void SimulationControl::_getActiveDNs(DNPtrList& fill_me) const {
		APPCommand** ppCommand;
		APPListIterator<APPCommand*> i(*_detectorMenu->commands());
 		sim::AbsDetectorNodeSelector* pSelector;
		while ( (ppCommand = i()) != NULL )
			{
				pSelector = dynamic_cast<sim::AbsDetectorNodeSelector*>(*ppCommand);
				if ( pSelector != NULL &&          // is is an AbsDectorNodeSelector
						 pSelector->isUserEnabled() && // it is possible to simulate
						 pSelector->value() )          // user wants to simulate
					{
						fill_me.push_back(pSelector->node());
					}
						
			}
	}
  
  AppResult SimulationControl::endRun( EventRecord* aRun ) {
    return AppResult::OK;
  }
  
  AppResult SimulationControl::beginJob( EventRecord* aJob ) {

		CdfRn* rn = CdfRn::Instance();
		rn->SetEngineSeeds(_randomSeed1.value(), _randomSeed2.value(),"SIMULATION");

    CdfRn::simulationEngine = CdfRn::Instance()->GetEngine("SIMULATION");

    // Set callbacks
    G3Callbacks* callbacks = G3Callbacks::instance();
    assert (callbacks != NULL);
    callbacks->setGustep(&handleStep);
    callbacks->setGutrak(&syncWithG3ParticlesAndTrack);
    callbacks->setGukine(&geantGukine); // from gnt

		// for fluka - geisha switch
		callbacks->setGuphad(&geantGuphad); 
		callbacks->setGuhadr(&geantGuhadr);

    _pashaBeginJob();
    
		Geant3GeometryInterface* g3g = Geant3GeometryInterface::instance();
		assert (g3g != 0);

    // Iterate over commands and declare active detectors for each
    // detector node in which to simulate hits
		DNPtrList activeDetectors;
		_getActiveDNs(activeDetectors);

		DNPtrList::const_iterator currentDN;
		DNPtrList::const_iterator endDN = activeDetectors.end();
		for (currentDN = activeDetectors.begin();
				 currentDN != endDN; 
				 ++currentDN)
			{
				std::string simSetTag((*currentDN)->detectorSetName());
				if ( simSetTag.empty() )
					{
						CdfDetectorNode::PVConstIterator toplevel = 
							(*currentDN)->beginningPhysicalVolume();
						simSetTag = (*toplevel)->getLogicalVolume()->getShortName();
					}

				CdfDetectorNode::DEConstIterator currentDE;
				CdfDetectorNode::DEConstIterator endDE = 
					(*currentDN)->endingDetectorElement();
				for (currentDE = (*currentDN)->beginningDetectorElement();
						 currentDE != endDE;
						 ++currentDE)
					{
						g3g->declareActiveVolumes(*currentDE, simSetTag);
					}

			}

//     APPCommand** command;
//     APPListIterator<APPCommand*> i(*_detectorMenu->commands());
//     sim::AbsDetectorNodeSelector* selector;
//     while (command=i() ) 
// 			{
// 				selector = dynamic_cast<sim::AbsDetectorNodeSelector*>(*command);
// 				if (selector && selector->isUserEnabled() && selector->value()) {
// 					const CdfDetectorNode* n = selector->node();
// 					// Does the detector node have a specially defined set name?
// 					std::string simSetTag(n->detectorSetName());
// 					if (simSetTag.empty()) { // If no, kludge one together:
// 						simSetTag = (*(n->beginningPhysicalVolume()))->
// 							getLogicalVolume()->getShortName();
// 					}
// 					for (CdfDetectorNode::DEConstIterator de = 
// 								 n->beginningDetectorElement();
// 							 de!=n->endingDetectorElement(); 
// 							 ++de) 
// 						{
// 							g3g->declareActiveVolumes(*de, simSetTag);
// 						}
// 				} // if (selector&&selector->value())
// 			} // while (command=i())
		
    if (_showAVolumes.value()) g3g->showActiveVolumes();


    return AppResult::OK;
  }
  
  AppResult SimulationControl::endJob( EventRecord* aJob ) {
    return AppResult::OK;
  }
  
  AppResult SimulationControl::abortJob( EventRecord* aJob ) {
    return AppResult::OK;
  }

  
  
} // namespace sim

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
