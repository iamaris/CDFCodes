// apr 25 2001 E.Gerchtein gCdfParticleDatabase initialization is removed.
// may 18 2001 E.Gerchtein gMcEvent

#include "SimulationMods/IntegrationControl.hh"

//#include "evt/Event.hh"
//#include "evt/CdfDictionary.hh"
#include "Geometry/TCdfGeometryManager.hh"
//#include "evt/evt.h"
#include "Geometry/G3Volume.hh"
//#include "r_n/r_n.hh"
#ifdef USE_ROOT_
#error USE_ROOT_ is defined! Programming Error!
#endif

namespace sim {

  void IntegrationControl::_pashaInitGlobals() {
    // Taken from InitModule.cc r1.45 1999/08/19 CG
    // Updated for InitiModule.cc r1.58 2000/04/19 CG
    // Need to prevent Pasha's uginit being called during the
    // initialization of TCdfGeometryManager
    G3_VOLUME::CallGeant=0;
    //InitOfflineCommonBlocks();
    TCdfGeometryManager::Instance();
    //  G3_VOLUME::CallGeant=1;
    // Leave it turned off and see what kills us
    //if (! gCdfDictionary) {
    //  gCdfDictionary = new CDF_DICTIONARY();
    //}

		//    gMcEvent = new MC_EVENT();

		//   if (! gEvent) {
    //  gEvent = new CDF_EVENT();
    //}

    // Need to initialize CDF R_N package
    //TRn::Instance();

  }

  void IntegrationControl::_pashaBeginJob() {
    // Taken from InitiModule.cc r1.58 2000/04/19 CG
    // Initialize the CDF particle database
  }

  void IntegrationControl::_pashaDestroyGlobals() {
    // Taken from InitModule.cc r1.45 1999/08/19 CG
    // Updated for InitiModule.cc r1.58 2000/04/19 CG
    //if (gMcEvent) {
    //  delete gMcEvent;
    //  gMcEvent = 0;
    //}

    //if (gEvent) {
    //  delete gEvent;
    //  gEvent = 0;
    //}
  
    //if (gCdfDictionary) {
    //  delete gCdfDictionary;
    //  gCdfDictionary = 0;
    //}

  }

  void IntegrationControl::_pashaInitEvent(AbsEvent* event) {
    // Taken from InitModule.cc r1.45 1999/08/19 CG
    // Updated for InitiModule.cc r1.58 2000/04/19 CG
    //gEvent->Init(event);
    // Initialization of gMcEvent is now done directly from gukine_(). Of
    // course. 2000/04/19 CG 
    //
    // gMcEvent->InitEvent();
  }

}

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
