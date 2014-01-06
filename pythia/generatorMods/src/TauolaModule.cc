/*************************************************************************
* A module to handle tau decays using TAUOLA package
* Created  Jul 05 2001 .. T.Moulik 
**************************************************************************/
// rev: 29 aug 2001 lena: removed HEPG output - this is done by
//                        GenOutputModule
//      30 aug 2001 lena: added ct and genId
//      07 dec 2001 lena: inherited AbsDeckpackModule to handle /HEPEVT/
//                        replaced CdfHepEvt by CdfHepevt

#include "generatorMods/TauolaModule.hh"
#include "tauola_i/tauola_i.hh"
#include "ErrorLogger_i/gERRLOG.hh"
#include "ParticleDB/hepevt.hh"

#include "stdhep_i/CdfHepevt.hh"
#include "SimulationObjects/HEPG_StorableBank.hh"
#include "Edm/Handle.hh"
#include "Edm/ConstHandle.hh"
#include "Edm/EventRecord.hh"
#include "r_n/CdfRn.hh"

extern "C" {
  void tauini_();
  void taumain_();
  void tauola_disable_leptonic_decays_();
}

// random engine for TauolaModule is registered as "TAUOLA"
// instead of TauolaModule::genId

const char* TauolaModule::genId = "TauolaModule";
const long TauolaModule::_defaultRandomSeed1 = 883591;
const long TauolaModule::_defaultRandomSeed2 = 9735476;

TauolaModule::TauolaModule()
  : AbsDecpackModule( TauolaModule::genId,"AC++ Tauola Interface Module")
  , _nevprt("Display_max_taudecays",this,0)
  , _nprt("Display_tauola_output",this,0)
  , _jak1("decay_mode_tau+",this,0)
  , _jak2("decay_mode_tau-",this,0)
  , _xk0dec("infrared_cutoff",this,0.001)
  , _itdkrc("qedcorr_switch",this,1)
  , _tauchirality("tau_chirality",this,0,-1,1)
  , _randomSeed1("RandomSeed1",this,TauolaModule::_defaultRandomSeed1)
  , _randomSeed2("RandomSeed2",this,TauolaModule::_defaultRandomSeed2)
  , _disableLeptonicDecays("disable_leptonic_decays",this,false)
{
  _initializeTauolaTalkTo();
}

TauolaModule::~TauolaModule() 
{
}

AppResult TauolaModule::genBeginJob() {
  if ( verbose() ) {
    std::cout << " Initializing TauolaModule and TAUOLA " << std::endl; }

  InitTDCommonBlocks();
  tauini_();
  if (_disableLeptonicDecays.value()) tauola_disable_leptonic_decays_();
  _absParm2Tauola();

  CdfRn* rn = CdfRn::Instance();
  if ( !rn->isReadingFromFile() ) {
    rn->SetEngineSeeds(_randomSeed1.value(), _randomSeed2.value(),"TAUOLA");
  }

  return AppResult::OK;
}

int TauolaModule::callGenerator(AbsEvent *anEvent) {

  CdfHepevt* hepevt = CdfHepevt::Instance(); // overkill..
  taumain_();
  return 1;
}

AppResult TauolaModule::genEndJob() {
  std::cout << "GOODBYE from TauolaModule " << std::endl;
  return AppResult::OK;
}

AppResult TauolaModule::genBeginRun( AbsEvent* anEvent ) {
  return AppResult::OK;
}

AppResult TauolaModule::genEndRun( AbsEvent* anEvent ) {
  return AppResult::OK;
}



void TauolaModule::_tauola2AbsParm() {
    
  // Convert Tauola common blocks to the AbsParms of TauolaModule
    
  _nevprt.set  (FORTRAN_tauola_i ->NEVPRT);
  _nprt.set   (FORTRAN_tauola_i ->NPRT);
  _tauchirality.set(FORTRAN_tauola_i->TAUCHIRALITY);
  
  _jak1.set   (FORTRAN_jaki ->JAK1);
  _jak2.set   (FORTRAN_jaki ->JAK2);
  
  _xk0dec.set (FORTRAN_taurad ->XK0DEC);   
  _itdkrc.set (FORTRAN_taurad ->ITDKRC);
  
}

// This routine takes the tauola-specific AbsParm values and transfers
// them to the Tauola COMMON blocks

void TauolaModule::_absParm2Tauola() {

  std::cout << FORTRAN_tauola_i->NEVPRT << " - tauola par " << std::endl;

  FORTRAN_tauola_i->NEVPRT   = _nevprt.value();
  FORTRAN_tauola_i->NPRT    = _nprt.value();
  FORTRAN_tauola_i->TAUCHIRALITY = _tauchirality.value();

  FORTRAN_jaki->JAK1 = _jak1.value();
  FORTRAN_jaki->JAK2 = _jak2.value();

  FORTRAN_taurad->XK0DEC = _xk0dec.value();
  FORTRAN_taurad->ITDKRC = _itdkrc.value();
}

