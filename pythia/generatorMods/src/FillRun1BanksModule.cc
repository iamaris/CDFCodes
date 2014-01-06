//_____________________________________________________________________________
// FillRun1BanksModule.cc (by P.Murat)

#include "generatorMods/FillRun1BanksModule.hh"
//=============================================================================
// Edm
//=============================================================================
#include "Edm/EventRecord.hh"
#include "Edm/Handle.hh"
#include "Edm/ConstHandle.hh"
//=============================================================================
// StorableBanks
//=============================================================================
#include "Trybos/TRY_Bank_Number.hh"
#include "HeaderObjects/LRIH_StorableBank.hh"
#include "StorableBanks/LRID_StorableBank.hh"
#include "StorableBanks/LRID_StorableBank_utilities.hh"

#include <iostream>
#include <iomanip>

//_____________________________________________________________________________
FillRun1BanksModule::FillRun1BanksModule( const char* const theName,
                                      const char* const theDescription )
  : AppModule( theName, theDescription )
{
}

//_____________________________________________________________________________
FillRun1BanksModule::~FillRun1BanksModule() {
}

//_____________________________________________________________________________
AppResult FillRun1BanksModule::beginJob(AbsEvent* aJob) {
  return AppResult::OK;
}

//_____________________________________________________________________________
AppResult FillRun1BanksModule::beginRun( AbsEvent* aRun ) {
  return AppResult::OK;
}

//_____________________________________________________________________________
AppResult FillRun1BanksModule::event( AbsEvent* anEvent ) {

  TRY_Bank_Number bnkn(1);
  Handle <LRID_StorableBank> lrid(new LRID_StorableBank(bnkn));

  for(EventRecord::ConstIterator i(anEvent,"LRIH_StorableBank");
                                                         i.is_valid(); ++i) {
    ConstHandle<LRIH_StorableBank>  lrih(i);
    lrid->set_run_number(lrih->run_number());
    lrid->set_run_type(LRID_run_type::monte_carlo_run);
    lrid->set_record_type(lrih->record_type());
    lrid->set_experiment_type(LRID_experiment_type::monte_carlo);

 }
    
  anEvent->append(lrid);

  return AppResult::OK;
}

//_____________________________________________________________________________
AppResult FillRun1BanksModule::endRun( AbsEvent* aRun ) {
   return AppResult::OK;
}

//_____________________________________________________________________________
AppResult FillRun1BanksModule::endJob( AbsEvent* aJob ) {
  return AppResult::OK;
}


//_____________________________________________________________________________
AppResult FillRun1BanksModule::abortJob( AbsEvent* aJob ){
  return AppResult::OK;
}


//_____________________________________________________________________________
AppModule* FillRun1BanksModule::clone(const char* cloneName) {
  return new FillRun1BanksModule(cloneName,
			    "this module is a clone of FillRun1BanksModule");
}






