//--------------------------------------------------------------------------
// AbsDecpackModule
//
// created Dec 07 2001, Elena Gerchtein (CMU)
//
//------------------------------------------------------------------------

#include "generatorMods/AbsDecpackModule.hh"

#include "Experiment/Experiment.hh"
#include "AbsEnv/AbsEnv.hh"
#include "evt/evt.h"

#include "stdhep_i/CdfHepevt.hh"
#include "stdhep_i/Hepevt.hh"
#include "stdhep_i/Tauevt.hh"
#include "SimulationObjects/HEPG_StorableBank.hh"
#include "Edm/ConstHandle.hh"
#include "Trybos/TRY_Bank_Number.hh"
#include "ErrorLogger_i/gERRLOG.hh"

//______________________________________________________________________________
AbsDecpackModule::AbsDecpackModule(const char* name, const char* title)
  : AppModule(name,title),
    _modeval(0),
    _mode("mode", this, 0)
{
  commands( )->append( &_mode );
  
  _mode.addDescription("mode: 0 - default, generator fills hepevt;  1 - QQ fills hepevt from hepg");
}

//______________________________________________________________________________
AbsDecpackModule::~AbsDecpackModule() 
{ 
}

//______________________________________________________________________________
AppResult AbsDecpackModule::beginJob( AbsEvent* job ) {
  _modeval = _mode.value();
  return genBeginJob();  
}

//______________________________________________________________________________
AppResult AbsDecpackModule::beginRun( AbsEvent* aRun ) 
{
  return genBeginRun(aRun);
}

AppResult AbsDecpackModule::endRun( AbsEvent* aRun ) 
{
  return genEndRun(aRun);
}

AppResult AbsDecpackModule::endJob( AbsEvent* aJob ) 
{
  return genEndJob();
}

//______________________________________________________________________________
AppResult AbsDecpackModule::event(AbsEvent* event) {

  CdfHepevt* hepevt = CdfHepevt::Instance();
  if ( _modeval ) {
    hepevt->initFromHepg();
  }

  int rc = 0;
  
  hepevt->clearCommon();                // overkill...

  for( std::list<Hepevt*>::iterator i = hepevt->contentHepevt().begin();
       i != hepevt->contentHepevt().end(); ++i ) {
    
    hepevt->list2common(i);
    rc = this->callGenerator(event);
    if ( !rc ) { 
      std::cerr << "AbsDecpackModule: Error in decay package. ";
      std::cerr << std::endl;
    }
    hepevt->common2list(i);
    Tauevt* tau = new Tauevt( *(hepevt->TauevtPtr()) );
    hepevt->contentTauevt().push_back(tau);
    hepevt->clearCommon();
  }  

  return AppResult::OK;
}

