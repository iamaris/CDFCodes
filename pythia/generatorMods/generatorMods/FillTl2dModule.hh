#ifndef __FILLTL2DMODULE_HH
#define __FILLTL2DMODULE_HH
//-----------------------------------------------------------------------------
//  MDCII kludge for the trigger simulation: fill TL2D bank with the 
//  L1 and L2 trigger masks - see the source file for the meaning of L1 and L2 
//  bits used
//-----------------------------------------------------------------------------
#include "Trybos/TRY_Bank_Type.hh"
#include "Framework/APPFilterModule.hh"
#include "Framework/AbsParmBool.hh"
#include "Framework/AbsParmGeneral.hh"
#include "Framework/AbsParmEnum.hh"

class FillTl2dModule : public AppFilterModule {

public:
  
  FillTl2dModule(const char* theName        = "FillTl2d", 
		 const char* theDescription = "Fake Trigger in TL2D" );
  
  virtual AppResult      beginJob( AbsEvent* aJob );
  virtual AppResult      beginRun( AbsEvent* aRun );
  virtual AppResult      event( AbsEvent* anEvent );
  virtual AppResult      endRun( AbsEvent* aRun );
  virtual AppResult      endJob( AbsEvent* aJob );
  virtual AppResult      abortJob( AbsEvent* aJob );

  virtual AppModule*     clone(const char* cloneName);

  virtual ~FillTl2dModule( );
    
private:
					// Level-1 Trigger Mask Bits.
					// These will be set to indicate the 
					// generator and generator process
					// each generated process has its own 
					// L1 bit
  AbsParmGeneral<int4> _L1TriggerBit;
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
  int                  _sumSingleMu;
  int                  _sumSingleEl;
  int                  _sumDiMu;
  int                  _sumDiEl;

};

#endif 
