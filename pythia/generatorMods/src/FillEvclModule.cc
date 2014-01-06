//
// FillEvclModule.cc
// Purpose:  To fake the trigger bits for Mock Data Challenge I
//           It fills the bits into EVCL
//           The EVCL output is used by Level3 Prereq module 
//
#include "generatorMods/FillEvclModule.hh"
#include "generatorMods/FillEvclBitDefs.hh"
#ifdef USE_CDFEDM2 
//=============================================================================
// Edm
//=============================================================================
#include "Edm/EventRecord.hh"
#include "Edm/ConstHandle.hh"

//=============================================================================
// StorableBanks
//=============================================================================
#include "StorableBanks/EVCL_StorableBank.hh"
#include "SimulationObjects/HEPG_StorableBank.hh"
#else
#include "Banks/EVCL_Bank.hh"
#include "Banks/HEPG_Bank.hh"
#include "Trybos/TRY_Record_Iter_Same.hh"
#include "Trybos/TRY_Generic_Bank.hh"
#include "Trybos/TRY_Bank_Type.hh"
#endif

#include <iostream>
#include <iomanip>
using std::dec;
using std::setw;
using std::hex;
using std::cout;
using std::endl;

const int id_e_plus =   -11;
const int id_e_minus =   11;
const int id_mu_plus =  -13;
const int id_mu_minus =  13;

FillEvclModule::FillEvclModule( const char* const theName,
                                      const char* const theDescription )
  : AppFilterModule( theName, theDescription ),
    _L1TriggerMask("L1Triggerbits",this,0),
    _MuHiPtCut("MuHiPtCut",this,15.),
    _ElHiPtCut("ElHiPtCut",this,15.),
    _MuLoPtCut("DiMuPtCut",this,1.5),
    _ElLoPtCut("DiElPtCut",this,5.),
    _MuEtaCut("EtaCut",this,1.2),
    _EleEtaCut("EtaCut",this,4.5),
    _reqSingleMu("SingleMuTrigger",this,true),
    _reqSingleEl("SingleElTrigger",this,true),
    _reqDiMu("DiMuTrigger",this,true),
    _reqDiEl("DiElTrigger",this,true)

{
  // Define parameters for talk-to
  // -----------------------------
  _L1TriggerMask.addDescription(
" \tLevel-1 Trigger Mask (filled in highest 16 bits)");
  commands()->append(&_L1TriggerMask);

  _MuHiPtCut.addDescription("\tPt Cut on Single Muons");
  commands()->append(&_MuHiPtCut);

  _ElHiPtCut.addDescription("\tPt Cut on Single Electrons");
  commands()->append(&_ElHiPtCut);

  _MuLoPtCut.addDescription("\tPt Cut on diMuons");
  commands()->append(&_MuLoPtCut);

  _ElLoPtCut.addDescription("\tPt Cut on diElectrons");
  commands()->append(&_ElLoPtCut);

  _MuEtaCut.addDescription("\tMuon Eta Cut on leptons");
  commands()->append(&_MuEtaCut);

  _EleEtaCut.addDescription("\tElectron Eta Cut on leptons");
  commands()->append(&_EleEtaCut);

  _reqSingleMu.addDescription("\tFilter on Single Muons");
  commands()->append(&_reqSingleMu);

  _reqSingleEl.addDescription("\tFilter on Single Electrons");
  commands()->append(&_reqSingleEl);

  _reqDiMu.addDescription("\tFilter on DiMuons");
  commands()->append(&_reqDiMu);

  _reqDiEl.addDescription("\tFilter on DiElectrons");
  commands()->append(&_reqDiEl);
}

FillEvclModule::~FillEvclModule() {}

AppResult FillEvclModule::beginJob( AbsEvent* aJob )
{
  _sumSingleMu=0;
  _sumSingleEl=0;
  _sumDiMu=0;
  _sumDiEl=0; 
#ifdef USE_CDFEDM2
    std::cout << "FillEvclModule: EDM2 Version does not set EVCL\n";
    std::cout << "But can be used as a filter!\n";
#endif
   return AppResult::OK;
}

AppResult FillEvclModule::beginRun( AbsEvent* aRun )
{
   return AppResult::OK;
}

AppResult FillEvclModule::event( AbsEvent* anEvent ) {

  bool Pass = false;
  int nHiMu = 0;
  int nHiEl = 0;
  int nLoMu = 0;
  int nLoEl = 0;

#ifdef USE_CDFEDM2
  //Find the EVCL Bank
  for(EventRecord::ConstIterator i1(anEvent,"EVCL_StorableBank"); 
      i1.is_valid(); ++i1) {
  ConstHandle<EVCL_StorableBank> evcl_h(i1);
    for(EventRecord::ConstIterator i2(anEvent,"HEPG_StorableBank"); 
        i2.is_valid(); ++i2) {
       ConstHandle<HEPG_StorableBank>  hepg_h(i2);
       //
       // Look at final particles and see how many e's and mu's pass cut
       //
        for (HEPG_StorableBank::particleIter i(*hepg_h); i.is_valid(); i++) {
#else
    // Find the EVCL Bank
   TRY_Record_Iter_Same i1(anEvent,"EVCL") ;
   if  ( i1.is_valid() ) {
     EVCL_Bank evcl(i1);
     //
     // First thing we do if fill the Level 1 Trigger word as requested
     // Note ugly hard coding...
     // 
     evcl.set_trigger_summary1((_L1TriggerMask.value()&0x7fff) << 16);
     //
     // Now find the HEPG bank and fill Level 2 triggers
     //
     TRY_Record_Iter_Same i2(anEvent,"HEPG") ;
     if(i2.is_valid()) {
        HEPG_Bank hepg(i2);
        //
        // Look at final particles and see how many e's and mu's pass cut
        //
         for (HEPG_Bank::particleIter i(hepg); i.is_valid(); i++) {
#endif
         if (i.is_final()) {
	   if(i.idhep() == id_e_plus || i.idhep() == id_e_minus) {
             float eta;
	     float pt = sqrt(i.Px()*i.Px()+i.Py()*i.Py());
             if(fabs(i.E() - i.Pz()) < 1e-6)      { eta =  20.0;}
             else if(fabs(i.E() + i.Pz()) < 1e-6) { eta = -20.0;}
             else {    eta = 0.5*log((i.E() + i.Pz())/(i.E() - i.Pz()));}
             if((pt>_ElHiPtCut.value())&&(fabs(eta)<_EleEtaCut.value())) nHiEl++;
             if((pt>_ElLoPtCut.value())&&(fabs(eta)<_EleEtaCut.value())) nLoEl++;
           } 
           else if(i.idhep() == id_mu_plus || i.idhep() == id_mu_minus) {
             float eta;
	     float pt = sqrt(i.Px()*i.Px()+i.Py()*i.Py());
             if(fabs(i.E() - i.Pz()) < 1e-6)      { eta =  20.0;}
             else if(fabs(i.E() + i.Pz()) < 1e-6) { eta = -20.0;}
             else {    eta = 0.5*log((i.E() + i.Pz())/(i.E() - i.Pz()));}
             if((pt>_MuHiPtCut.value())&&(fabs(eta)<_MuEtaCut.value())) nHiMu++;
             if((pt>_MuLoPtCut.value())&&(fabs(eta)<_MuEtaCut.value())) nLoMu++;
           }
	 }
	}

    //
    // Fill L2 trigger bits based on HEPG.
    // Also, provide filtering capability based on (sub)set of the bits
    //
    int L2Trig = 0;
    if(nLoMu > 1) {
       L2Trig = L2Trig | (1 << FillEvclBitDefs::DiMu) ;
       if(_reqDiMu.value())  Pass=true;
       _sumDiMu++;
    }
    if(nHiMu > 0) {
      L2Trig = L2Trig | (1 << FillEvclBitDefs::SingleMu) ;
       if(_reqSingleMu.value())  Pass=true;
       _sumSingleMu++;
    }
    if(nLoEl > 1) {
      L2Trig = L2Trig | (1 << FillEvclBitDefs::DiEle) ;
       if(_reqDiEl.value())  Pass=true;
       _sumDiEl++;
    }
    if(nHiEl > 0) {
      L2Trig = L2Trig | (1 << FillEvclBitDefs::SingleEle) ;
       if(_reqSingleEl.value())  Pass=true;
       _sumSingleEl++;
    }
#ifdef USE_CDFEDM2 
#else
    evcl.set_trigger_summary3(L2Trig & 0x3ff);
#endif
     }
  }
  this->setPassed(Pass);
  return AppResult::OK;
}

AppResult FillEvclModule::endRun( AbsEvent* aRun )
{
   return AppResult::OK;
}

AppResult FillEvclModule::endJob( AbsEvent* aJob )
{
  std::cout << "\n\n----Summary from FillEvclModule: --------\n"<<
          "\tSingle Mu triggers: "<< _sumSingleMu << "\n"<<
          "\tSingle El triggers: "<< _sumSingleEl << "\n"<<
          "\tDiMu triggers: "<< _sumDiMu << "\n"<<
           "\tDiEl triggers: "<< _sumDiEl << 
           "\n------------------------------------------\n" ;
   return AppResult::OK;
}

AppResult FillEvclModule::abortJob( AbsEvent* aJob )
{
   return AppResult::OK;
}

AppModule*
FillEvclModule::clone(const char* cloneName)
{
  return new FillEvclModule(cloneName,"this module is a clone of FillEvclModule");
}






