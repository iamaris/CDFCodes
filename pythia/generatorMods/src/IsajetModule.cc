//--------------------------------------------------------------------------
// File and Version Information:
// 	IsajetModule.cc, $Id: IsajetModule.cc,v 1.3 2002/01/28 00:13:52 rico Exp $
//
// Description:
//	Class IsajetModule
//
//      This is the wrapper for the Isajet Generator
//
// Environment:
//	Software developed for the CDFII Detector
//
// Author List:
//	Marjorie Shapiro		Original Author
// Modifications:
//      001 M. Shapiro Change to use pointer to bcs rather than bcs 
//
//  revision history:
//  -----------------
// *0001 Jan 24 1998 P.Murat: move bank_interface.hh from bnk/ to ybos/
// *0002 Jan 28 1998 P.Murat: don't copy banks in between YBOS and TRYBOS 
//                            any more - always work in TRYBOS record
// *0003 May 08 1998 P.Murat: remove YBOS-->TRYBOS bank copy once again
// *0004 Sep 08 1998 P.Murat: remove YBOS-->TRYBOS bank copy the 3rd time
// *0005 Jan 08 1999 P.Murat: clean the code up: 
//                            initialize Isajet on module construction
//                            rather than in module::event method
// *0006 Feb 07 1999 P.Murat: introduce new base class (GeneratorModule)
//                            to allow generation of piled up events
// Aug 30 2001 lena: add genId
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "generatorMods/IsajetModule.hh"

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
#include "evt/Event.hh"
#include "ParticleDB/hepevt.hh"
// #include "Framework/APPFramework.hh"
//------------------------------------------
// Fortran routines called from this code --
//--------------------------------------------
extern "C" {
  void isajet_i_init_();
  void isajet_i_event_();
  void isajet_i_fin_();
  void isahep_(int*);
}

const char* IsajetModule::genId = "Isajet";

//______________________________________________________________________________
IsajetModule::IsajetModule()
  : AbsGenModule( IsajetModule::genId, "Isajet AC++ module") {}

//_____________________________________________________________________________
IsajetModule::~IsajetModule() {}

//______________________________________________________________________________
int IsajetModule::callGenerator(AbsEvent* event) {
  isajet_i_event_();
  return 0;
}

//_____________________________________________________________________________
AppResult IsajetModule::genBeginRun(AbsEvent* run) {
  return AppResult::OK;
}

//_____________________________________________________________________________
AppResult IsajetModule::genEndRun(AbsEvent* run) {
  isajet_i_fin_();
  return AppResult::OK;
}

//_____________________________________________________________________________
AppResult IsajetModule::genBeginJob() {
					// initialize FORTRAN Isajet code
  isajet_i_init_();
  return AppResult::OK;
}

//_____________________________________________________________________________
AppResult IsajetModule::genEndJob() {
  isajet_i_fin_();
  return AppResult::OK;
}

//_____________________________________________________________________________
void IsajetModule::fillHepevt() {
  int i = 1;
  isahep_(&i);
}





