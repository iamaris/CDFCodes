// apr 25 2001 E.Gerchtein - gCdfParticleDatabase initialization is removed.
// apr 30 2001 EG - only gMcEvent initialization is left, geant_i/TGeant3.h
//                  include is moved outside _addCdfParticles().                 

#include "SimulationMods/SimulationControl.hh"

//#include "evt/Event.hh" // extern MC_EVENT* gMcEvent; is here
//#include "evt/CdfDictionary.hh"
//#include "Geometry/TCdfGeometryManager.hh"
#include "ParticleDB/ParticleDb.hh"

//#include "gnt/mc_event.hh"
//#include "evt/evt.h"
//#include "Geometry/G3Volume.hh"
//#include "r_n/r_n.hh"
#ifdef USE_ROOT_
#error USE_ROOT_ is defined! Programming Error!
#endif

#include "geant_i/TGeant3.h"

namespace sim {
	
  void SimulationControl::_pashaInitGlobals() {
    // Taken from InitModule.cc r1.45 1999/08/19 CG
    // Updated for InitiModule.cc r1.58 2000/04/19 CG
    // Need to prevent Pasha's uginit being called during the
    // initialization of TCdfGeometryManager
		
		// TCdfGeometryManager::Instance() is not to be called - comment the following
		// out
		// G3_VOLUME::CallGeant=0;
		
		// only gBcs is referenced if #ifndef USE_CDFEDM2 - comment it out.
    //InitOfflineCommonBlocks();

    //TCdfGeometryManager::Instance();

    //  G3_VOLUME::CallGeant=1;
    // Leave it turned off and see what kills us
		//if (! gCdfDictionary) {
    //  gCdfDictionary = new CDF_DICTIONARY();
    //}

		//    gMcEvent = new MC_EVENT();
    _addCdfParticles();

		//    if (! gEvent) {
		//      gEvent = new CDF_EVENT();
		//    }

    // Need to initialize CDF R_N package - I don't think so
    // TRn::Instance();

  }

  void SimulationControl::_pashaBeginJob() {
    // Taken from InitiModule.cc r1.58 2000/04/19 CG
    // Initialize the CDF particle database
  }

  void SimulationControl::_pashaDestroyGlobals() {
    // Taken from InitModule.cc r1.45 1999/08/19 CG
    // Updated for InitiModule.cc r1.58 2000/04/19 CG
		//    if (gMcEvent) {
		//      delete gMcEvent;
		//      gMcEvent = 0;
		//    }

		//    if (gEvent) {
		//      delete gEvent;
		//      gEvent = 0;
		//    }
  
    //if (gCdfDictionary) {
    //  delete gCdfDictionary;
    //  gCdfDictionary = 0;
    //}
  }

  void SimulationControl::_pashaInitEvent(AbsEvent* event) {
    // Taken from InitModule.cc r1.45 1999/08/19 CG
    // Updated for InitiModule.cc r1.58 2000/04/19 CG
		//    gEvent->Init(event);
    // Initialization of gMcEvent is now done directly from gukine_(). Of
    // course. 2000/04/19 CG 
    //
    // gMcEvent->InitEvent();
  }

  void SimulationControl::_addCdfParticles() {


    int   g3_code;              
    int   itrtyp  = 6;
    float amass   = 0.;
    float charge  = 0.;
    float tlife   = 1.e15;
    float ubuf[1];
    int   nwb     = 0;   

    TGeant3*               gnt = TGeant3::Instance();
    ParticleDb*             db = ParticleDb::Instance();

    g3_code = db->GeantCodeOfCdfParticle(ParticleDb::CDF_EM_SHOWER);
    gnt->Gspart(g3_code,"EM_Shower           ",
	      itrtyp,amass,charge,tlife,ubuf,nwb);

    g3_code = db->GeantCodeOfCdfParticle(ParticleDb::CDF_HAD_SHOWER);
    gnt->Gspart(g3_code,"Had_Shower          ",
	      itrtyp,amass,charge,tlife,ubuf,nwb);

    g3_code = db->GeantCodeOfCdfParticle(ParticleDb::CDF_PHO_SHOWER);
    gnt->Gspart(g3_code,"Pho_Shower          ",
	      itrtyp,amass,charge,tlife,ubuf,nwb);

				// for some reason GEANT doesn't look
				// at the lifetime of geantino's,
				// so trace all the B-mesons etc as
				// muons to be able to set zero lifetime
				// also set mass to 1. to avoid divisions by 0

    gnt->Gspart(54,"dummy               ",5,1.,0.,0.,ubuf,nwb);
	}

}

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
