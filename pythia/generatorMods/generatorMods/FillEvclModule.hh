#ifndef __FILLEVCLMODULE_HH
#define __FILL EVCLMODULE_HH

#include "Trybos/TRY_Bank_Type.hh"
#include "Framework/APPFilterModule.hh"
#include "Framework/AbsParmBool.hh"
#include "Framework/AbsParmGeneral.hh"
#include "Framework/AbsParmEnum.hh"

class FillEvclModule : public AppFilterModule {

public:
  
  FillEvclModule( const char* const theName="FillEvcl", 
                     const char* const theDescription="Fake Trigger in EVCL" );
  
  virtual AppResult      beginJob( AbsEvent* aJob );
  virtual AppResult      beginRun( AbsEvent* aRun );
  virtual AppResult      event( AbsEvent* anEvent );
  virtual AppResult      endRun( AbsEvent* aRun );
  virtual AppResult      endJob( AbsEvent* aJob );
  virtual AppResult      abortJob( AbsEvent* aJob );

  virtual AppModule*     clone(const char* cloneName);

  virtual ~FillEvclModule( );
    
private:
  // Level-1 Trigger Mask Bits.
  // These will be set to indicate the generator and generator process
  AbsParmGeneral<int4> _L1TriggerMask;

  // Cuts for Leptons
  AbsParmGeneral<float> _MuHiPtCut;
  AbsParmGeneral<float> _ElHiPtCut;
  AbsParmGeneral<float> _MuLoPtCut;
  AbsParmGeneral<float> _ElLoPtCut;
  AbsParmGeneral<float> _MuEtaCut;
  AbsParmGeneral<float> _EleEtaCut;

  // Filter on HEPG results
  AbsParmGeneral<bool> _reqSingleMu;
  AbsParmGeneral<bool> _reqSingleEl;
  AbsParmGeneral<bool> _reqDiMu;
  AbsParmGeneral<bool> _reqDiEl;
  // Statistics
  int _sumSingleMu;
  int _sumSingleEl;
  int _sumDiMu;
  int _sumDiEl;

};

#endif 
