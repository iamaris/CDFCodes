//_____________________________________________________________________________
// FillTl2dModule.cc (by P.Murat)
// Purpose:  To fake the trigger bits for the Mock Data Challenge II
//           It fills the bits into TL2D
//           The TL2D output is used by Level3 Prereq module 
//
// Cloned from FillEvclModule written by M.Shapiro and used for MDCI
//
// Meaning of the L1 trigger bits:
//
/*----------------------------------------------------------------------------|
L1 Bit     Process                                 | N(events) | Contact      |
  -------------------------------------------------|-----------|--------------|
  0    TTbar Herwig                                |           |              |
  1    Jet 20 Herwig                               |           |              |
  2    Jet 50 Herwig                               |           |              |
  3    Jet 100 Herwig                              |
  4    Pythia_Zee                                  |
  5    Wgrad_e                                     |
  6    Wgrad_mu                                    |
  7    Wh_pythia                                   |
  8    Zh_pythia                                   |
  9    b-->J/psi X                                 |
 10    chargino-neutralino production              |
 11    Z -> b bbar                                 |
 12    Z -> tau+ tau-                              |
 13    dijets 175 Pythia                           |
 14    dijets 290 Pythia                           |
 15    single top                                  |
 16    W+2jets(VECBOS+HERPRT)                      |
 17    WZ                                          |
 18    WW                                          |
 19    ZZ                                          |
 20    mSUGRA chi2(0) chi1(+)->tau's (70/120)      |
 21    mSUGRA chi2(0) chi1(+)->tau's (150/225)     |
 22    Drell-Yan                                   |
 23    diphoton                                    |


- L2 trigger bits (the same as in MDC1) :

------------------------------------------------------------------------------|
L2 bit          "Trigger"                                                     |
------------------------------------------------------------------------------|
 0              high-pt electron                                              |
 1              high-pt muon                                                  |
 2              dielectron                                                    |
 3              dimuon                                                        |
------------------------------------------------------------------------------|

---------------------------------------------------------------------------*/

#include "generatorMods/Mdc2TriggerBits.hh"
#include "generatorMods/FillTl2dModule.hh"
//=============================================================================
// Edm
//=============================================================================
#include "Edm/EventRecord.hh"
#include "Edm/Handle.hh"
//=============================================================================
// StorableBanks
//=============================================================================
#include "TriggerObjects/TL2D_StorableBank.hh"
#include "SimulationObjects/HEPG_StorableBank.hh"

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

//_____________________________________________________________________________
FillTl2dModule::FillTl2dModule( const char* const theName,
                                      const char* const theDescription )
  : AppFilterModule( theName, theDescription ),
    _L1TriggerBit("L1TriggerBit"   ,this,0),
    _MuHiPtCut   ("MuHiPtCut"      ,this,15.),
    _ElHiPtCut   ("ElHiPtCut"      ,this,15.),
    _MuLoPtCut   ("DiMuPtCut"      ,this,1.5),
    _ElLoPtCut   ("DiElPtCut"      ,this,5.),
    _MuEtaCut    ("EtaCut"         ,this,1.2),
    _EleEtaCut   ("EtaCut"         ,this,4.5),
    _reqSingleMu ("SingleMuTrigger",this,true),
    _reqSingleEl ("SingleElTrigger",this,true),
    _reqDiMu     ("DiMuTrigger"    ,this,true),
    _reqDiEl     ("DiElTrigger"    ,this,true)

{
  // Define parameters for talk-to
  // -----------------------------
  _L1TriggerBit.addDescription(
" \t MDCII Level-1 Trigger Bit for the simulated process");
  commands()->append(&_L1TriggerBit);

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

//_____________________________________________________________________________
FillTl2dModule::~FillTl2dModule() {
}

//_____________________________________________________________________________
AppResult FillTl2dModule::beginJob(AbsEvent* aJob) {
  _sumSingleMu = 0;
  _sumSingleEl = 0;
  _sumDiMu     = 0;
  _sumDiEl     = 0; 
  return AppResult::OK;
}

//_____________________________________________________________________________
AppResult FillTl2dModule::beginRun( AbsEvent* aRun ) {
  return AppResult::OK;
}

//_____________________________________________________________________________
AppResult FillTl2dModule::event( AbsEvent* anEvent ) {

  bool Pass  = false;
  int  nHiMu = 0;
  int  nHiEl = 0;
  int  nLoMu = 0;
  int  nLoEl = 0;

					// Create TL2D Bank - follow the example
					// in TriggerMods/src/Writer.cc

  Handle <TL2D_StorableBank> tl2d(new TL2D_StorableBank );
  
  tl2d->set_number_of_blocks();
    
  tl2d->set_n_words_in_block1();
  tl2d->set_n_words_in_block2(10);
  tl2d->set_n_words_in_block3() ;
  tl2d->set_n_words_in_block4(10);
  tl2d->set_n_words_in_block5(10);
  tl2d->set_n_words_in_block6(10);
  tl2d->set_n_words_in_block7(10);
  tl2d->set_n_words_in_block8(10);
  tl2d->set_n_words_in_block9(9);
  tl2d->set_n_words_in_block10(10);
  tl2d->set_n_words_in_block11(10);

  tl2d->set_max_size();
					// set all the words in L1 and L2 
					// decision blocks to 0

  uint4   data[100];
  for (int i=0; i<100; i++) {
    data[i] = 0;
  }

  tl2d->add_block1(data) ;
  tl2d->add_block2(data) ;
  tl2d->add_block3(data) ;
  tl2d->add_block4(data) ;
  tl2d->add_block5(data) ;
  tl2d->add_block6(data) ;
  tl2d->add_block7(data) ;
  tl2d->add_block8(data) ;
  tl2d->add_block9(data) ;
  tl2d->add_block10(data) ;
  tl2d->add_block11(data) ;
  tl2d->commit();
					// set L1 bit - by definition for MC
					// there is only one bit set

  tl2d->setL1TriggerBit(_L1TriggerBit.value());

  for(EventRecord::ConstIterator i2(anEvent,"HEPG_StorableBank"); 
                                                         i2.is_valid(); ++i2) {
    ConstHandle<HEPG_StorableBank>  hepg_h(i2);

					// Look at final particles and see how
					// many e's and mu's pass cut

    for (HEPG_StorableBank::particleIter i(*hepg_h); i.is_valid(); i++) {
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
  }
					// Fill L2 trigger bits based on HEPG.
					// Also, provide filtering capability 
					// based on (sub)set of the bits
					// dimuon bit
  if (nLoMu >= 2) {
    tl2d->setL2TriggerBit(Mdc2TriggerBits::L2::DiMuon);
    if(_reqDiMu.value())  {
      Pass=true;
    }
    _sumDiMu++;
  }

  if (nHiMu > 0) {
    tl2d->setL2TriggerBit(Mdc2TriggerBits::L2::HptMuon);
    if(_reqSingleMu.value())  Pass=true;
    _sumSingleMu++;
  }

  if (nLoEl > 1) {
    tl2d->setL2TriggerBit(Mdc2TriggerBits::L2::DiElectron);
    if(_reqDiEl.value())  Pass=true;
    _sumDiEl++;
  }

  if (nHiEl > 0) {
    tl2d->setL2TriggerBit(Mdc2TriggerBits::L2::HptElectron);
    if(_reqSingleEl.value())  Pass=true;
    _sumSingleEl++;
  }

  anEvent->append(tl2d);
  this->setPassed(Pass);
  return AppResult::OK;
}

//_____________________________________________________________________________
AppResult FillTl2dModule::endRun( AbsEvent* aRun ) {
   return AppResult::OK;
}

//_____________________________________________________________________________
AppResult FillTl2dModule::endJob( AbsEvent* aJob ) {
  std::cout << "\n\n----Summary from FillTl2dModule: --------\n"<<
          "\tSingle Mu    triggers: "<< _sumSingleMu << "\n"<<
          "\tSingle El    triggers: "<< _sumSingleEl << "\n"<<
          "\tDiMuon       triggers: "<< _sumDiMu << "\n"<<
          "\tDiElelectron triggers: "<< _sumDiEl << 
          "\n------------------------------------------\n" ;
  return AppResult::OK;
}


//_____________________________________________________________________________
AppResult FillTl2dModule::abortJob( AbsEvent* aJob ){
  return AppResult::OK;
}


//_____________________________________________________________________________
AppModule* FillTl2dModule::clone(const char* cloneName) {
  return new FillTl2dModule(cloneName,
			    "this module is a clone of FillTl2dModule");
}






