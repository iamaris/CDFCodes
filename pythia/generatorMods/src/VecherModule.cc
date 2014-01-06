//--------------------------------------------------------------------------
// Description:
//	Class UnwtModule
//
//      This is the wrapper of the module for the unweighting 
//      VECBOS_>HERWIG interface    
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
//  May. 12, 1999, G.Velev,  created
//  Jul. 10, 2001, G.Velev, add herprt interface calls
//  Jul. 11, 2001, G.Velev, copy all the talk-to parameters from herwig to 
//                  support herprt talk
//  Jul. 18, 2001, G.Velev, move talk-to to VecherModule_initializeTalkTo.cc
//                          add a control over sudakov read/write
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "generatorMods/VecherModule.hh"
#include "herwig_i/Herwig.hh"
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
#include "evt/Event.hh"

#include "ParticleDB/hepevt.hh"
#include "stdhep_i/CdfHepevt.hh"
#include "stdhep_i/Hepevt.hh"

#include "Framework/APPFramework.hh"
#include "Framework/APPFilterModule.hh"
#include "Trybos/TRY_Abstract_Record.hh"
#include "Bbos/bank_interface.hh"
#include "AbsEnv/AbsEnv.hh"
#include "stdhep_i/CdfHepEvt.hh"
#include "SimulationObjects/HEPG_StorableBank.hh"
#include "Edm/Handle.hh"
#include "Edm/ConstHandle.hh"
#include "Edm/EventRecord.hh"

//void InitHerwigCommonBlocks();

//------------------------------------------
// Fortran routines called from this code --
//--------------------------------------------
extern "C" {
  void vecher_interface_();
  void vecher_init_();
  void vecher_event_( int* );
  void vecher_fin_();
  void vecher_runbeg_();
  void vecher_runend_();  
                   // add the herprt interface
  void herprt_interface_();
  void herprt_event_( int* );
  void herprt_init_();
  void herprt_runbeg_();
  void herprt_runend_();
                   // from StdHep
  void  hwghep_(int *mconv);
}

const char* VecherModule::genId = "VecherModule";

//----------------
// Constructors --
//----------------

VecherModule::VecherModule(const char* const theName, 
    const char* const theDescription )
    : AppModule( theName, theDescription ),
    _summary("EndOfJobSummary", this, false),
      
    // Vecher parameters, change type of  the leptons
    _lepton_type("leptonType",this,1),

    //  Starts herwig part, copy from HerwigModule

    //    _decayTable_Parm( "DECAY_TABLE", this, "MyFavoriteTable.txt"),    
    //    _scaleChoiceParm( "SCALE_CHOICE", this, 1),
    _process("Process",this,0),    // set for herprt 

    _maxer_Parm("Maxer",this,100),
    _randomseed1_Parm("Randseed1",this,0),
    _randomseed2_Parm("Randseed2",this,0),
    _lambdaQCD_Parm( "Lambda_QCD", this, 0.18),
   
      common( new  VECBOS_I_COMMON_BLOCKS() ),
  
     //
     // commands of Menu 01: The Beam menu
     // ----------------------------------
     _Beamtype1_Parm("Beam1", this, "12345678"),
     _Beamtype2_Parm("Beam2", this , "12345678"),
     _Energy1_Parm("Pbeam1", this,980.),
     _Energy2_Parm("Pbeam2", this,980.),
     //
     // commands of menu 02: The Sudakov menu
     // -------------------------------------
     _sudord_Parm("Sudakov_order",this,2),
     //
     // commands of menu 03: The Structure Function Menu
     // ------------------------------------------------
     _autpdf("Autpdf",this,"MRS"),
     _modpdf("Modpdf",this,28),
     //
     // commands of menu 04: The Hard Scattering Menu
     // ---------------------------------------------
//cjl     _rapiditiesm_Parm("Ymin",this,-8.0),    
     _rapiditiesM_Parm("Ymax",this,8.0),    
     _ptm_Parm("Ptmin",this,0),
     _ptM_Parm("Ptmax",this,0),
     _invarmassm_Parm("MassMin",this,0),
     _invarmassM_Parm("MassMax",this,0),     
     _thrust_Parm("ThrustMax",this,0.9),
     _ptpow_Parm("Ptpow",this,4.0),
     _empow_Parm("Empow",this,4.00),
     _q2pow_Parm("Q2power",this,2.5),
     _qlim_Parm("Qlim",this,0),
     _q2dilsm_Parm("Q2dilsmin",this,0),
     _q2dilsM_Parm("Q2dilsmax",this,0),
     _bgshat_Parm("Bgshat",this, false),
     //    
     // commands of Menu 05: Parton Shower Menu
     // ---------------------------------------
     _azsoft_Parm("Azsoft",this,true),
     _azspin_Parm("Azspin",this,true),
     _qspac_Parm("Spacelike_evolution",this,2.5),
     _ispac_Parm("Ispac",this,0),
     _nospac_Parm("Nospac",this,0),
     _vqcut_Parm("Qcutoff",this,0.48),
     _vgcut_Parm("Gcutoff",this,0.10),
     _vpcut_Parm("Pcutoff",this,0.40),
     _intrinsicpt_Parm("Pt_incoming_hadrons",this,0),
     //
     // commands of Menu 06: Hadronization Menu
     // ---------------------------------------- 
     _clmax_Parm("Clmax", this,3.35), 
     _clpow_Parm("Clpow",this,2.0),  
     _psplt1_Parm("Psplt1",this,1.0),  
     _psplt2_Parm("Psplt2",this,1.0),  
     _cldir1_Parm("Cldir1",this,1),  
     _cldir2_Parm("Cldir2",this,1),  
     _clsmr1_Parm("Clsmr1",this,0.0),  
     _clsmr2_Parm("Clsmr2",this,0.0),  
     _qdiqk_Parm("Qdiqk",this,0.00), 
     _pdiqk_Parm("Pdiqk",this,5.00), 
     _decwt_Parm("Decwt",this,0),
     _pwt_Parm("Q_dq_weight",this,1),
     //
     // commands of Menu 07: Underlying Event Menu
     // -------------------------------------------
     _pmbn1_Parm("Pmbn1",this,9.11),
     _pmbn2_Parm("Pmbn2",this,0.115),
     _pmbn3_Parm("Pmbn3",this,9.50),
     _pmbk1_Parm("Pmbk1",this,0.029),
     _pmbk2_Parm("Pmbk2",this,0.104),
     _pmbm1_Parm("Pmbm1",this,0.4),
     _pmbm2_Parm("Pmbm2",this,2.0),
     _pmbp1_Parm("Pmbp1",this,5.2),
     _pmbp2_Parm("Pmbp2",this,3.0),
     _pmbp3_Parm("Pmbp3",this,5.2),
     _ensof_Parm("Ensof",this,1.00),
     //
     // commands of Menu 08: Decays Menu
     // --------------------------------
     _pizstable_Parm("Pizstable",this,0),
     _pltcut_Parm("Pltcut",this,0.),
     //
     // commands of Menu 09: Generation Menu
     // ------------------------------------
     _eventweight_Parm("No_weights",this,1),
      _maxweight_Parm("Maxwt",this,1.),         // this is for HERPRT
     _effmin_Parm("Effmin",this,0.001),
     _noshower_Parm("No_shower",this,0),
     _nohadroniz_Parm("No_hadronization",this,0),
     _tlout_Parm("tlout",this,10.0),
     //
     // commands of Menu 10: Parameters Menu
     // ------------------------------------
     _colors_Parm("Colors",this,3),
     _flavors_Parm ("Flavors", this,6),
     _weiangle_Parm("Weinberg_angle", this,0.2319),  
     _cabangle_Parm("Cabbibo_angle",this,0.0488),
     _wwidth_Parm("W_width", this,2.0),
     _zwidth_Parm("Z_width", this,2.5),
     _hwidth_Parm("Higgs_width", this,0.02),
     //
     // commands of Menu 11: Masses Menu
     //---------------------------------
     _down_Parm("down", this, 0.32),
     _up_Parm("up", this, 0.32),
     _strange_Parm("strange", this, 0.50),
     _charm_Parm("charm",this, 1.55),
     _bottom_Parm("bottom", this,4.95), 
     _top_Parm("top",this, 175),
     _gluonmass_Parm("gluon",this,0.75),  
     _W_Parm("W", this,80.4),
     _Z0_Parm("Z0",this,91.2),
     _higgs_Parm("higgs",this,150), 
     //
     // commands of Menu 12: Readsusy Menu
     //-----------------------------------
     _read_Filesusy("File_Readsusy",this,"$HERWIG_SUSY"),
     //
     // commands of Menu 13: Zprime Menu
     //---------------------------------
     _zprime_Parm("Zprime",this,0),
     _ZP_Parm("ZP",this,500.),
     _zpwidth_Parm("ZPwidth",this,5.),
     //
     // commands of Menu 14: prints Menu
     //---------------------------------
     _iprint_Parm("iprint",this,1),
     _maxpr_Parm("maxpr",this,0),
     _prvtx_Parm("prvtx",this,1),
     _prndef_Parm("prndef",this,1),
     _prntex_Parm("prntex",this,0),
     _prnweb_Parm("prnweb",this,0),
     _nprfmt_Parm("nprfmt",this,1),
     //
     // commands of Menu 15: Controls read/write Sudakov table
     //-------------------------------------------------------
     _read_Sudakov_Table("readTable",this,0),
     _write_Sudakov_Table("writeTable",this,0)

{
                                   //initialize talk-to
  _initializeTalkTo();
  common = NULL;
}

//--------------
// Destructor --
//--------------

VecherModule::~VecherModule()
{
  if ( common ) {
    delete common;
    common = NULL;
  }
}

//--------------
// Operations --
//--------------

AppResult VecherModule::beginJob(AbsEvent* event) {

  // have to init Herwig common blocks in Vecher module:
  Herwig* herwig = Herwig::Instance();

  // and put abs.parms to herwig common blocks in vecbos module too.
  common = new  VECBOS_I_COMMON_BLOCKS();
  putParametersToVecher(common);

  // initialize FORTRAN VECHER code
  vecher_init_();
  herprt_init_();

  getParametersFromVecher(common);  
  return AppResult::OK;
}

AppResult VecherModule::endJob(AbsEvent* event) {
  if(event) {
     vecher_fin_();
  }
  return AppResult::OK;
}

AppResult VecherModule::endRun(AbsEvent* event) {
    if(event) {
      vecher_runend_();
      herprt_runend_();
    }
  return AppResult::OK;
}

AppResult VecherModule::beginRun(AbsEvent* event) {

  _events = 0;
  //  putParametersToVecher(common);

  if(event) {
    ybos_init_framework(event);
    vecher_runbeg_();
    herprt_runbeg_();
    ybos_init_framework(0);
  }

  return AppResult::OK;
  
}

AppResult VecherModule::event(AbsEvent* event) {
  
  CdfHepevt* hepevt = CdfHepevt::Instance();
  hepevt->clearCommon();  
  
  int iret = 0;
  if(event) {
    ybos_init_framework(event);
    vecher_event_(&iret);
    herprt_event_(&iret);
    if (iret == 0) {
      fillHepevt(event);
      ++_events;
      return AppResult::OK;
    }
    return AppResult::ERROR;
  }
  else 
    return AppResult::ERROR;
}

void VecherModule::fillHepevt(AbsEvent* event) {
  int mconv = 1;
  hwghep_(&mconv);
  //  int next = 1;

  // You are not supposed to write HEPG bank,
  // this is done in GenOutputManager
  //hepevt2Hepg(event,next);
  CdfHepevt* hepevt = CdfHepevt::Instance();
  Hepevt* cur;
  cur = new Hepevt( *hepevt->HepevtPtr(), *hepevt->Hepev4Ptr() );
  hepevt->contentHepevt().push_back( cur );
  return;
}

void VecherModule::putParametersToVecher(VECBOS_I_COMMON_BLOCKS *common) {

  // puts the AbsParms to the Vecher common blocks
  // debug flag

  common->Sumpro()->ILEPTON = _lepton_type.value();

  // herwig part 

  Herwig* herwig = Herwig::Instance();

  int nch;
  nch = (_Beamtype1_Parm.value().size()<8) ? _Beamtype1_Parm.value().size() : 7;
  for(int i=0; i<7; i++) {
    if(i<nch) {
      herwig->Hwbmch()->part1[i] = _Beamtype1_Parm.value().c_str()[i];
    }
    else {
      herwig->Hwbmch()->part1[i] = ' ';
    }
  }
  nch = (_Beamtype2_Parm.value().size()<8) ? _Beamtype2_Parm.value().size() : 7;
  for(int i=0; i<7; i++) {
    if(i<nch) {
      herwig->Hwbmch()->part2[i] = _Beamtype2_Parm.value().c_str()[i];
    }
    else {
      herwig->Hwbmch()->part2[i] = ' ';
    }
  }
  nch = (_autpdf.value().size()<20) ? _autpdf.value().size() : 19;
  for(int i=0; i<19; i++) {
    if(i<nch) {
      herwig->Hwprch()->autpdf[0][i] = _autpdf.value().c_str()[i];
      herwig->Hwprch()->autpdf[1][i] = _autpdf.value().c_str()[i];
    }
    else {
      herwig->Hwprch()->autpdf[0][i] = ' ';
      herwig->Hwprch()->autpdf[1][i] = ' ';
    }
  }
  
  herwig->Hwpram() ->modpdf[0]=_modpdf.value();
  herwig->Hwpram() ->modpdf[1]=_modpdf.value();
  herwig->Hwproc() ->pbeam1=_Energy1_Parm.value();
  herwig->Hwproc() ->pbeam2=_Energy2_Parm.value();
  herwig->Hwproc() ->iproc=_process.value();
  herwig->Hwevnt() ->maxer=_maxer_Parm.value();
  herwig->Hwevnt() ->nrn[0]=_randomseed1_Parm.value();
  herwig->Hwevnt() ->nrn[1]=_randomseed2_Parm.value();
  herwig->Hwpram() -> qcdlam=_lambdaQCD_Parm.value();
  herwig->Hwusud() ->sudord=_sudord_Parm.value();

  //cjl re ymin
  //gHwhard ->yjmin=_rapiditiesm_Parm.value();
  float yjmin1;
  yjmin1 = -_rapiditiesM_Parm.value();
  //std::cout << " yjmin in AbsParm2Her in HM.cc  " << yjmin1  <<std::endl;
  herwig->Hwhard() ->yjmin=yjmin1;
  //cjl_e
  herwig->Hwhard() ->yjmax=_rapiditiesM_Parm.value();
  herwig->Hwhard() ->ptmin=_ptm_Parm.value();
  herwig->Hwhard() ->ptmax=_ptM_Parm.value();
  herwig->Hwhard() ->emmin=_invarmassm_Parm.value();
  herwig->Hwhard() ->emmax=_invarmassM_Parm.value();
  herwig->Hwhard() ->thmax=_thrust_Parm.value();
  herwig->Hwhard() ->ptpow=_ptpow_Parm.value();
  herwig->Hwhard() ->empow=_empow_Parm.value();
  herwig->Hwhard() ->q2pow=_q2pow_Parm.value();
  herwig->Hwhard() ->qlim=_qlim_Parm.value();
  herwig->Hwhard() ->q2min=_q2dilsm_Parm.value();
  herwig->Hwhard() ->q2max=_q2dilsM_Parm.value();
  herwig->Hwhard() ->bgshat=_bgshat_Parm.value();
 
  herwig->Hwpram() ->azsoft=_azsoft_Parm.value();
  herwig->Hwpram() -> azspin=_azspin_Parm.value();
  herwig->Hwpram() -> qspac=_qspac_Parm.value();
  herwig->Hwpram() -> ispac=_ispac_Parm.value();
  herwig->Hwpram() -> nospac=_nospac_Parm.value();
  herwig->Hwpram() -> vqcut=_vqcut_Parm.value();
  herwig->Hwpram() -> vgcut=_vgcut_Parm.value();
  herwig->Hwpram() -> vpcut=_vpcut_Parm.value();
  herwig->Hwpram() -> ptrms=_intrinsicpt_Parm.value();

  herwig->Hwpram() -> clmax=_clmax_Parm.value(); 
  herwig->Hwpram() -> clpow=_clpow_Parm.value();
  herwig->Hwpram() -> psplt[0]=_psplt1_Parm.value();
  herwig->Hwpram() -> psplt[1]=_psplt2_Parm.value();
  herwig->Hwpram() -> cldir[0]=_cldir1_Parm.value();
  herwig->Hwpram() -> cldir[1]=_cldir2_Parm.value();
  herwig->Hwpram() -> clsmr[0]=_clsmr1_Parm.value();
  herwig->Hwpram() -> clsmr[1]=_clsmr2_Parm.value();
  herwig->Hwpram() -> qdiqk=_qdiqk_Parm.value();
  herwig->Hwpram() -> pdiqk=_pdiqk_Parm.value();
  herwig->Hwuwts() -> decwt=_decwt_Parm.value();
  herwig->Hwuwts() -> pwt[0]=_pwt_Parm.value();

  // cjl note pmbn3 and pmbk2 negatives here
  herwig->Hwminb() -> pmbn1=_pmbn1_Parm.value();
  herwig->Hwminb() -> pmbn2=_pmbn2_Parm.value();
  herwig->Hwminb() -> pmbn3=-_pmbn3_Parm.value();
  herwig->Hwminb() -> pmbk1=_pmbk1_Parm.value();
  herwig->Hwminb() -> pmbk2=-_pmbk2_Parm.value();
  herwig->Hwminb() -> pmbm1=_pmbm1_Parm.value();
  herwig->Hwminb() -> pmbm2=_pmbm2_Parm.value();
  herwig->Hwminb() -> pmbp1=_pmbp1_Parm.value();
  herwig->Hwminb() -> pmbp2=_pmbp2_Parm.value();
  herwig->Hwminb() -> pmbp3=_pmbp3_Parm.value();
  herwig->Hwpram() -> ensof=_ensof_Parm.value();

  herwig->Hwupdt() -> rstab[21]=_pizstable_Parm.value();
  herwig->Hwdist() -> pltcut=_pltcut_Parm.value();

  herwig->Hwevnt() -> nowgt=_eventweight_Parm.value();
  herwig->Hwevnt() -> wgtmax=_maxweight_Parm.value();
  herwig->Hwpram() -> effmin=_effmin_Parm.value();
  herwig->Hwevnt() -> tlout=_tlout_Parm.value();
  herwig->Hwpram() -> ncolo=_colors_Parm.value();
  herwig->Hwpram() -> nflav=_flavors_Parm.value(); 
  herwig->Hwpram() -> swein=_weiangle_Parm.value();
  herwig->Hwpram() -> scabi=_cabangle_Parm.value();
  herwig->Hwpram() -> gamw=_wwidth_Parm.value();
  herwig->Hwpram() -> gamz=_zwidth_Parm.value();
  herwig->Hwpram() -> gamh=_hwidth_Parm.value();
  
  herwig->Hwprop() ->rmass[1]=_down_Parm.value();
  herwig->Hwprop() ->rmass[2]=_up_Parm.value();
  herwig->Hwprop() ->rmass[3]=_strange_Parm.value();
  herwig->Hwprop() ->rmass[4]=_charm_Parm.value();
  herwig->Hwprop() ->rmass[5]=_bottom_Parm.value(); 
  herwig->Hwprop() ->rmass[6]=_top_Parm.value();
  herwig->Hwprop() ->rmass[13]=_gluonmass_Parm.value();
  herwig->Hwprop() ->rmass[198]=_W_Parm.value();
  herwig->Hwprop() ->rmass[200]=_Z0_Parm.value();
  herwig->Hwprop() ->rmass[201]=_higgs_Parm.value();


  herwig->Hwpram() -> zprime = _zprime_Parm.value();
  herwig->Hwprop() -> rmass[202] = _ZP_Parm.value();
  herwig->Hwpram() -> gamzp = _zpwidth_Parm.value();

  herwig->Hwpram() -> iprint = _iprint_Parm.value();
  herwig->Hwevnt() -> maxpr = _maxpr_Parm.value();
  herwig->Hwpram() -> prvtx = _prvtx_Parm.value();
  herwig->Hwpram() -> prndef = _prndef_Parm.value();
  herwig->Hwpram() -> prntex = _prntex_Parm.value();
  herwig->Hwpram() -> prnweb = _prnweb_Parm.value();
  herwig->Hwpram() -> nprfmt = _nprfmt_Parm.value();

  herwig->Hwpram() -> lrsud = _read_Sudakov_Table.value();
  herwig->Hwpram() -> lwsud = _write_Sudakov_Table.value();
}

void  VecherModule::getParametersFromVecher(VECBOS_I_COMMON_BLOCKS *common) {

  // takes the AbsParms from the Vecher common blocks

  _lepton_type.set(common->Sumpro()->ILEPTON);

  // herwig part 
  Herwig* herwig = Herwig::Instance();

    // convert Herwig Common Blocks to the AbsParms of the Herwig Module

  string mystring;
  mystring.assign(herwig->Hwbmch()->part1,8);
  _Beamtype1_Parm.set(mystring);
  mystring.assign(herwig->Hwbmch()->part2,8);
  _Beamtype2_Parm.set(mystring);
  mystring.assign(herwig->Hwprch()->autpdf[0],20);
  _autpdf.set(mystring);
  _modpdf.set(herwig->Hwpram()->modpdf[0]);
  _Energy1_Parm.set(herwig->Hwproc() ->pbeam1);
  _Energy2_Parm.set(herwig->Hwproc() ->pbeam2);
  _process.set(herwig->Hwproc()->iproc);
  _maxer_Parm.set(herwig->Hwevnt() ->maxer);
  _randomseed1_Parm.set(herwig->Hwevnt() ->nrn[0]);
  _randomseed2_Parm.set(herwig->Hwevnt() ->nrn[1]);
  _lambdaQCD_Parm.set(herwig->Hwpram() -> qcdlam);
  _sudord_Parm.set(herwig->Hwusud() ->sudord);

//cjl  _rapiditiesm_Parm.set(herwig->Hwhard() ->yjmin);
  _rapiditiesM_Parm.set(herwig->Hwhard() ->yjmax);
  _ptM_Parm.set(herwig->Hwhard() ->ptmax);
  _ptm_Parm.set(herwig->Hwhard() ->ptmin);
  _invarmassM_Parm.set(herwig->Hwhard() ->emmax);
  _invarmassm_Parm.set(herwig->Hwhard() ->emmin);
  _thrust_Parm.set(herwig->Hwhard() ->thmax);
  _ptpow_Parm.set(herwig->Hwhard() ->ptpow);
  _empow_Parm.set(herwig->Hwhard() ->empow);
  _q2pow_Parm.set(herwig->Hwhard() ->q2pow);
  _qlim_Parm.set(herwig->Hwhard() ->qlim);
  _q2dilsM_Parm.set(herwig->Hwhard() ->q2max);
  _q2dilsm_Parm.set(herwig->Hwhard() ->q2min);
  _bgshat_Parm.set(herwig->Hwhard() ->bgshat);

  _azsoft_Parm.set(herwig->Hwpram()->azsoft);
  _azspin_Parm.set(herwig->Hwpram() -> azspin);
  _qspac_Parm.set(herwig->Hwpram() -> qspac);
  _ispac_Parm.set(herwig->Hwpram() -> ispac);
  _nospac_Parm.set(herwig->Hwpram() -> nospac);
  _vqcut_Parm.set(herwig->Hwpram() -> vqcut);
  _vgcut_Parm.set(herwig->Hwpram() -> vgcut);
  _vpcut_Parm.set(herwig->Hwpram() -> vpcut);
  _intrinsicpt_Parm.set(herwig->Hwpram() -> ptrms);

  _clmax_Parm.set(herwig->Hwpram() -> clmax); 
  _clpow_Parm.set(herwig->Hwpram() -> clpow);
  _psplt1_Parm.set(herwig->Hwpram() -> psplt[0]);
  _psplt2_Parm.set(herwig->Hwpram() -> psplt[1]);
  _cldir1_Parm.set(herwig->Hwpram() -> cldir[0]);
  _cldir2_Parm.set(herwig->Hwpram() -> cldir[1]);
  _clsmr1_Parm.set(herwig->Hwpram() -> clsmr[0]);
  _clsmr2_Parm.set(herwig->Hwpram() -> clsmr[1]);
  _qdiqk_Parm.set(herwig->Hwpram() -> qdiqk);
  _pdiqk_Parm.set(herwig->Hwpram() -> pdiqk);
  _decwt_Parm.set(herwig->Hwuwts() ->decwt);
  _pwt_Parm.set(herwig->Hwuwts() ->pwt[0]);

  //cjl need - sign on pmbn3 and pmbk2
  _pmbn1_Parm.set(herwig->Hwminb() -> pmbn1);
  _pmbn2_Parm.set(herwig->Hwminb() -> pmbn2);
  _pmbn3_Parm.set(-herwig->Hwminb() -> pmbn3);
  _pmbk1_Parm.set(herwig->Hwminb() -> pmbk1);
  _pmbk2_Parm.set(-herwig->Hwminb() -> pmbk2);
  _pmbm1_Parm.set(herwig->Hwminb() -> pmbm1);
  _pmbm2_Parm.set(herwig->Hwminb() -> pmbm2);
  _pmbp1_Parm.set(herwig->Hwminb() -> pmbp1);
  _pmbp2_Parm.set(herwig->Hwminb() -> pmbp2);
  _pmbp3_Parm.set(herwig->Hwminb() -> pmbp3);
  _ensof_Parm.set(herwig->Hwpram() -> ensof);

  _pizstable_Parm.set(herwig->Hwupdt() -> rstab[21]);
  _pltcut_Parm.set(herwig->Hwdist() -> pltcut);

  _eventweight_Parm.set(herwig->Hwevnt() ->nowgt);
  _maxweight_Parm.set(herwig->Hwevnt() ->wgtmax);
  _effmin_Parm.set(herwig->Hwpram() ->effmin);
  _tlout_Parm.set(herwig->Hwevnt() ->tlout);

  _colors_Parm.set(herwig->Hwpram() -> ncolo);
  _flavors_Parm.set(herwig->Hwpram() -> nflav); 
  _weiangle_Parm.set(herwig->Hwpram() -> swein);
  _cabangle_Parm.set(herwig->Hwpram() -> scabi);
  _wwidth_Parm.set(herwig->Hwpram() -> gamw);
  _zwidth_Parm.set(herwig->Hwpram() -> gamz);
  _hwidth_Parm.set(herwig->Hwpram() -> gamh);

  _down_Parm.set(herwig->Hwprop() ->rmass[1]);
  _up_Parm.set(herwig->Hwprop() ->rmass[2]);
  _strange_Parm.set(herwig->Hwprop() ->rmass[3]);
  _charm_Parm.set(herwig->Hwprop() ->rmass[4]);
  _bottom_Parm.set(herwig->Hwprop() ->rmass[5]); 
  _top_Parm.set(herwig->Hwprop() ->rmass[6]);
  _gluonmass_Parm.set(herwig->Hwprop() ->rmass[13]);
  _W_Parm.set(herwig->Hwprop() ->rmass[198]);
  _Z0_Parm.set(herwig->Hwprop() ->rmass[200]);
  _higgs_Parm.set(herwig->Hwprop() ->rmass[201]);
  
  _zprime_Parm.set(herwig->Hwpram() -> zprime);
  _ZP_Parm.set(herwig->Hwprop() -> rmass[202]);
  _zpwidth_Parm.set(herwig->Hwpram() -> gamzp);

  _iprint_Parm.set(herwig->Hwpram() -> iprint);
  _maxpr_Parm.set(herwig->Hwevnt() -> maxpr);
  _prvtx_Parm.set(herwig->Hwpram() -> prvtx);
  _prndef_Parm.set(herwig->Hwpram() -> prndef);
  _prntex_Parm.set(herwig->Hwpram() -> prntex);
  _prnweb_Parm.set(herwig->Hwpram() -> prnweb);
  _nprfmt_Parm.set(herwig->Hwpram() -> nprfmt);

  _read_Sudakov_Table.set(herwig->Hwpram() -> lrsud);
  _write_Sudakov_Table.set(herwig->Hwpram() -> lwsud);

  // Following two are used only internally to this routine:
  //_noshower_Parm.set(0);// correspondence in common block not finded
  //_nohadroniz_Parm.set(0);// correspondence in common block not finded
  
}








