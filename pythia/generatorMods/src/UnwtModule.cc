//--------------------------------------------------------------------------
// Description:
//	Class VecunwModule
//
//      This is the wrapper of the module for the unweighting 
//      VECBOS/PAPAGENO monte-carlo events   
//
// Environment:
//	Software developed for the CDFII Detector
//
// Author List:
//	George Velev
// Modifications:
//
//  revision history:
//  -----------------
//  Mar. 3, 1999, G.Velev,  go to App classes  
//  Jul  9, 2001, G.Velev, Add talk-to
//  Aug 30 2001 lena: added ct and genId
//  Oct 31 2001 lena: renamed the class
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "generatorMods/UnwtModule.hh"

//-------------
// C Headers --
//-------------
#include <assert.h>

//---------------
// C++ Headers --
//---------------
#include <iostream>

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "BaBar/Cdf.hh"
#include "inc/bcs.h"
#include "evt/evt.h"
#include "vecbos_i/vecbos_i_common_blocks.hh"
#include "evt/Event.hh"
#include "ParticleDB/hepevt.hh"
#include "Framework/APPFramework.hh"
#include "Framework/APPFilterModule.hh"
#include "AbsEnv/AbsEnv.hh"
#include "Trybos/TRY_Abstract_Record.hh"
#include "Bbos/bank_interface.hh"

//------------------------------------------
// Fortran routines called from this code --
//--------------------------------------------
extern "C" {
  void unwt_init_();
  void unwt_event_( int* );
  void unwt_fin_();
  void unwt_runbeg_();
  void unwt_runend_();  
}

const char* VecunwModule::genId = "VecunwModule";

//----------------
// Constructors --
//----------------

VecunwModule::VecunwModule()
    : AppModule( VecunwModule::genId, "UnWeighted Module for VECBOS" ),
    _summary("EndOfJobSummary", this, false),
    _max_weight("Weight",this,0.00167778966), 
    _rndmSeed("Seed",this,65307836)  
{


  _max_weight.addDescription("\t\t\tMaxWeight\n\
    \t\t\t Specify maximal weigh for this run  \n\
    \t\t\t Syntax: Weight set <value>");
  
  _rndmSeed.addDescription("\t\t\tRandomSeed\n\
    \t\t\t Specify the seed to init the random generator [65307836] \n\
    \t\t\t Syntax: Seed set <value>");

  commands( )->append( &_summary );
  commands( )->append(&_max_weight);
  commands( )->append(&_rndmSeed);

                                        // initialize FORTRAN UNWT code
  unwt_init_();
  common = new  VECBOS_I_COMMON_BLOCKS();
  putParametersToUnwt(common);
  getParametersFromUnwt(common);
}

//--------------
// Destructor --
//--------------

VecunwModule::~VecunwModule()
{
  if ( common ) {
    delete common;
    common = NULL;
  }
}

//--------------
// Operations --
//--------------

AppResult VecunwModule::beginJob(AbsEvent* event) {

  _events = 0;

  putParametersToUnwt(common);
  
  return AppResult::OK;
}

AppResult VecunwModule::endJob(AbsEvent* event) {
  unwt_fin_();
  return AppResult::OK;
}

AppResult VecunwModule::endRun(AbsEvent* event) {
  unwt_runend_();
  return AppResult::OK;
}

AppResult VecunwModule::beginRun(AbsEvent* event) {
  unwt_runbeg_();
  return AppResult::OK;
}

AppResult VecunwModule::event(AbsEvent* event) {

  ybos_init_framework(event);
  int iret = 0;
  unwt_event_(&iret);
  ++_events;
  ybos_init_framework(0);

  if (iret==0 ) {
     _passed = 1;      
     return AppResult::OK;
  }
  else{
     _passed = 0; 
     return AppResult::ERROR;
  }
}
void VecunwModule::putParametersToUnwt(VECBOS_I_COMMON_BLOCKS *common) {
   common->Unwght()->ISEED =  _rndmSeed.value();
   common->Unwght()->WTMAX = _max_weight.value(); 
}

void  VecunwModule::getParametersFromUnwt(VECBOS_I_COMMON_BLOCKS *common) {
  // takes the AbsParms from the Unwg common blocks
  _rndmSeed.set(common->Unwght()->ISEED);
  _max_weight.set(common->Unwght()->WTMAX);  
}





