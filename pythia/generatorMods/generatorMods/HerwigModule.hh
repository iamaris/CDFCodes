//--------------------------------------------------------------------------
// File and Version Information:
//      HerwigModule.hh
//
// Description:
//      This module is an generator Input module for use with Herwig 5.9 
//
// Environment:
//      Software developed for CDF
//
// Author List:
//      Marjorie Shapiro  and Elena Bucciol
//      Aug 1998
// Revisions
//      j lys, minor changes 1999
//             additions for Herwig 6.1 May-2000
//             Herwig v6202 Apr-2001
//      lena  removed USE_ROOT flag and include FrameMods/GeneratorModule.hh  
//            removed __CINT__   ( Aug 29 2001 ) 
//      Aug 30 2001 lena: moved ct and genId
//      May 2002, add talkto variables for vers 6_4  jlys
//      April 2003 add talkto variables for vers 6.500
//      Feb 2004  add talkto variable for vers 6.504  jlys
//      Feb 2005  add talkto variable for SOFTME  jdlee
//      Oct 2004  add talkto variable for vers 6.505 Jimmy  jlys
//------------------------------------------------------------------------

#ifndef HERWIGMODULE_HH 
#define HERWIGMODULE_HH 

#include <string>


class APPMenu;
class AbsEvent;
class AppResult;
class APPCommand;

#include "generatorMods/AbsGenModule.hh"

class HerwigModule : public AbsGenModule {

public:

  static const char* genId;
				// ****** constructors and destructor
  HerwigModule();

  virtual ~HerwigModule( );
				// ****** overloaded methods of GeneratorModule

  AppResult   genBeginJob();      
  AppResult   genEndJob  ();      
  AppResult   genBeginRun( AbsEvent* aJob );      
  AppResult   genEndRun  ( AbsEvent* aJob );      
  int         callGenerator(AbsEvent* event);
  void        fillHepevt();
				// ****** other methods
  void        Absparm2Her();
  void        Her2Absparm();


private:
  void InitializeTalkTo();

  APPMenu _01Menu;
  APPMenu _02Menu;
  APPMenu _03Menu;
  APPMenu _04Menu;
  APPMenu _05Menu;
  APPMenu _06Menu;
  APPMenu _07Menu; 
  APPMenu _08Menu;
  APPMenu _09Menu;
  APPMenu _10Menu;
  APPMenu _11Menu;
  APPMenu _12Menu;
  APPMenu _13Menu;
  APPMenu _14Menu;
  APPMenu _15Menu;
  APPMenu _16Menu;
  APPMenu _17Menu;
				// Here are the things our specialized commands 
				// will set...

				// the AbsParms below at begin job

  AbsParmGeneral<int> _process;              // QCD process generated
  AbsParmGeneral<int> _maxer_Parm;           //
  AbsParmGeneral<int> _randomseed1_Parm;      //
  AbsParmGeneral<int> _randomseed2_Parm;      //
  AbsParmGeneral<double> _lambdaQCD_Parm;     // to calculate alphas
  //  AbsParmGeneral<string> _decayTable_Parm;   // parm for the file name
				             // that has the decay table
  //  AbsParmGeneral<int>  _scaleChoiceParm;     // choice of qcd scale 
				             // 1=pt/2, 2=pt, 3=2pt

				// commands of Menu 01

  AbsParmGeneral<std::string> _Beamtype1_Parm;// initialize beam particle 1
  AbsParmGeneral<double> _Energy1_Parm;//initialize beam energy 1
  AbsParmGeneral<std::string> _Beamtype2_Parm;// initialize beam particle 2
  AbsParmGeneral<double> _Energy2_Parm;//initialize beam energy 2    
  AbsParmGeneral<int> _pipsmr_Parm;//smear primary vertex coors?
  AbsParmGeneral<double> _vipwidx_Parm;//primary vertex x width
  AbsParmGeneral<double> _vipwidy_Parm;//primary vertex y width
  AbsParmGeneral<double> _vipwidz_Parm;//primary vertex z width


     

				// commands of Menu 02 - v6.1 no read/write

  AbsParmGeneral<int> _sudord_Parm ; //Sudakov order    

				// commands of Menu 03

  AbsParmGeneral<std::string> _autpdf;
  AbsParmGeneral<int> _modpdf;
  AbsParmGeneral<int> _nstru_Parm;


				// commands of Menu 04

  AbsParmGeneral<double> _ptm_Parm;// select transverse momentum limits min
  AbsParmGeneral<double> _invarmassm_Parm;// select invariant mass limits min 
  AbsParmGeneral<double> _rapiditiesM_Parm;// select rapidity limits max      
  AbsParmGeneral<double> _ptM_Parm;// select transverse momentum limits max
  AbsParmGeneral<double> _invarmassM_Parm;// select invariant mass limits max        
  AbsParmGeneral<double> _thrust_Parm;// select thrust 
  AbsParmGeneral<double> _ptpow_Parm;// select ET power 
  AbsParmGeneral<double> _empow_Parm;// select M power 
  AbsParmGeneral<double> _q2pow_Parm;// select Q**2 power
  AbsParmGeneral<double> _qlim_Parm;// select upper limits in scale     
  AbsParmGeneral<double> _q2dilsm_Parm;// set Q**2 limits min
  AbsParmGeneral<double> _q2dilsM_Parm;// set Q**2 limits max
  AbsParmGeneral<int> _bgshat_Parm;// scaling switch
  
				// commands of Menu 05

  AbsParmGeneral<int> _azsoft_Parm;//azimuthal correlation soft gluon
  AbsParmGeneral<int> _azspin_Parm;//azimuthal correlation spin
  AbsParmGeneral<double> _qspac_Parm;//lower limit for spacelike evolution 
  AbsParmGeneral<int> _ispac_Parm;//controls exact meaning of qspac
  AbsParmGeneral<int> _nospac_Parm;//can turn off ISR 
  AbsParmGeneral<double> _vqcut_Parm;//quark virtuality cutoff
  AbsParmGeneral<double> _vgcut_Parm;//gluon virtuality cutoff
  AbsParmGeneral<double> _vpcut_Parm;//photon virtuality cutoff
  AbsParmGeneral<int> _softme_Parm;//soft matrix-element correction
  //  AbsParmGeneral<double> _gluonmass_Parm;//gluon virtual mass cutoff
  AbsParmGeneral<double> _intrinsicpt_Parm;// intrinsic pt incoming hadrons  
  AbsParmGeneral<int> _clreco_Parm;// include colour rearrangementi
  AbsParmGeneral<double> _preco_Parm;// Prob. for colour rearrangement 

				// commands of Menu 06  

  AbsParmGeneral<double> _clmax_Parm;//mass cluster mass parameter 
  AbsParmGeneral<double> _clpow_Parm;// mass dependence of cluster hadronization 
  AbsParmGeneral<double> _psplt1_Parm;// mass dist in cluster splitting, non-b 
  AbsParmGeneral<double> _psplt2_Parm;// mass dist in cluster splitting, b 
  AbsParmGeneral<int> _cldir1_Parm;// direction memory, cluster splitting, non-b 
  AbsParmGeneral<int> _cldir2_Parm;// direction memory, cluster splitting, b 
  AbsParmGeneral<double> _clsmr1_Parm;// alignment, cluster splitting, non-b 
  AbsParmGeneral<double> _clsmr2_Parm;// alignment, cluster splitting, b 
  AbsParmGeneral<double> _qdiqk_Parm; //scale at which gluon can be split into diquarks
  AbsParmGeneral<double> _pdiqk_Parm; //probability of diquark splitting per unit log scale
  AbsParmGeneral<double> _sngwt_Parm; // a priori weights, singlet baryons for cluster decays  
  AbsParmGeneral<double> _decwt_Parm; // a priori weights, decuplet baryons for cluster decays  
  AbsParmGeneral<double> _pwt1_Parm; // a priori weights, d.dbar pairs
  AbsParmGeneral<double> _pwt2_Parm; // a priori weights, u.ubar pairs
  AbsParmGeneral<double> _pwt3_Parm; // a priori weights, s.sbar pairs
  AbsParmGeneral<double> _pwt4_Parm; // a priori weights, c.cbar pairs
  AbsParmGeneral<double> _pwt5_Parm; // a priori weights, b.bbar pairs
  AbsParmGeneral<double> _pwt6_Parm; // a priori weights, t.tbar pairs
  AbsParmGeneral<double> _pwt7_Parm; // a priori weights, qq.qqbar pairs
  
				// commands of Menu 07 

  AbsParmGeneral<double> _prsof_Parm;//prob. of underlying soft event
  AbsParmGeneral<double> _ensof_Parm;//multy. enhancement, underlyng soft event
  AbsParmGeneral<double> _pmbn1_Parm;// soft int. param for multiplicity
  AbsParmGeneral<double> _pmbn2_Parm;// soft int. param for multiplicity
  AbsParmGeneral<double> _pmbn3_Parm;// soft int. param for multiplicity
  AbsParmGeneral<double> _pmbk1_Parm;// soft int. param for negative binomial
  AbsParmGeneral<double> _pmbk2_Parm;// soft int. param for negative binomial
  AbsParmGeneral<double> _pmbm1_Parm;// soft int. param for cluster mass
  AbsParmGeneral<double> _pmbm2_Parm;// soft int. param for cluster mass
  AbsParmGeneral<double> _pmbp1_Parm;// soft int. param for cluster pt
  AbsParmGeneral<double> _pmbp2_Parm;// soft int. param for cluster pt
  AbsParmGeneral<double> _pmbp3_Parm;// soft int. param for cluster pt

				// commands of Menu 08

  AbsParmGeneral<int> _pizstable_Parm; //if pizero is stable (non-decaying)
  AbsParmGeneral<double> _topltim_Parm; //top quark lifetime
  AbsParmGeneral<double> _pltcut_Parm; //lifetime cut for stable particle
  AbsParmGeneral<int> _modbos1_Parm; //force decay modes, 1st weak boson
  AbsParmGeneral<int> _modbos2_Parm; //force decay modes, 2nd weak boson
  
				// commands of Menu 09

  AbsParmGeneral<int> _eventweight_Parm; //control event weights
  AbsParmGeneral<double> _maxweight_Parm; //set max weight
  AbsParmGeneral<int> _negwts_Parm; //allow negative weights
  AbsParmGeneral<double> _effmin_Parm; //min. acceptable eff to generate evts
  AbsParmGeneral<int> _noshower_Parm; //turn off parton shower
  AbsParmGeneral<int> _nohadroniz_Parm; //turn off hadronization
  AbsParmGeneral<double> _tlout_Parm; //time (sec) needed to terminate gracefully

				// commands of Menu 10

  AbsParmGeneral<int> _colors_Parm;// number of colors
  AbsParmGeneral<int> _flavors_Parm;// number of flavors
  AbsParmGeneral<double> _weiangle_Parm;// sin**2 of Weimberg angle 
  AbsParmGeneral<double> _cabangle_Parm;// sin**2 of Cabibbo angle
  AbsParmGeneral<double> _wwidth_Parm;// Width of W boson
  AbsParmGeneral<double> _zwidth_Parm;//Width of Z boson

				// commands of Menu 11

  AbsParmGeneral<double> _down_Parm;// Set down quark mass
  AbsParmGeneral<double> _up_Parm;// Set up quark mass
  AbsParmGeneral<double> _strange_Parm;// Set strange quark mass
  AbsParmGeneral<double> _charm_Parm;// Set charm quark mass
  AbsParmGeneral<double> _bottom_Parm;// Set bottom quark mass
  AbsParmGeneral<double> _top_Parm;// Set top quark mass
  AbsParmGeneral<double> _gluonmass_Parm;// Set gluon mass
  AbsParmGeneral<double> _W_Parm;// Set W+/- mass
  AbsParmGeneral<double> _Z0_Parm;// Set Z0  mass
  AbsParmGeneral<double> _higgs_Parm;// Set Higgs  mass
  AbsParmGeneral<double> _gammax_Parm;// Set Higgs  mass
  
				// commands of Menu 12

  AbsParmGeneral<std::string> _read_Filesusy; //parm for the susy file name 
  AbsParmGeneral<int> _rparty_Parm;// Whether R-parity is conserved in SUSY

				// commands of Menu 13

  AbsParmGeneral<int> _zprime_Parm;// Whether to use Z-prime particle
  AbsParmGeneral<double> _ZP_Parm;// Mass of Z-prime particle
  AbsParmGeneral<double> _zpwidth_Parm;// Width of Z-prime particle

				// commands of Menu 14

  AbsParmGeneral<int> _iprint_Parm;// Herwig print level control
  AbsParmGeneral<int> _maxpr_Parm;// Dump of Herwig events
  AbsParmGeneral<int> _prvtx_Parm;// Whether to have vtx print in dump
  AbsParmGeneral<int> _prndef_Parm;// print ASCII to screen/log file
  AbsParmGeneral<int> _prntex_Parm;// print LATEX to *.tex files 
  AbsParmGeneral<int> _prnweb_Parm;// print html to *.html
  AbsParmGeneral<int> _nprfmt_Parm;// controls print sig. figs - short or long

				// commands of Menu 15

  AbsParmGeneral<int> _optm_Parm;// optimise efficiency, VV or ZQQ
  AbsParmGeneral<int> _iopstp_Parm;// no. of iters, opt effic, VV or ZQQ
  AbsParmGeneral<int> _iopsh_Parm;// evts per iter, opt effic, VV or ZQQ
  AbsParmGeneral<int> _syspin_Parm;// turn on spin correlations
  AbsParmGeneral<int> _threeb_Parm;// three-body matrix elements
  AbsParmGeneral<int> _fourb_Parm;// four-body matrix elements
  AbsParmGeneral<int> _lrdec_Parm;// unit to read spin cor, mat el inits.
  AbsParmGeneral<int> _lwdec_Parm;// unit to write spin cor, mat el inits.

				// commands of Menu 16

  AbsParmGeneral<int> _prespl_Parm;// long. mom or rapidity (H 6_5)
  AbsParmGeneral<int> _lhsoft_Parm;// Les H, underlying event
  AbsParmGeneral<int> _lhglsf_Parm;// Les H, colour self-connected gluons
  AbsParmGeneral<double> _grvlam_Parm;// graviton coupling
  AbsParmGeneral<double> _emgrv_Parm;// graviton mass
  AbsParmGeneral<double> _gamgrv_Parm;// graviton width
  AbsParmGeneral<int> _itoprd_Parm;// Not use or use PHOTOS (H 6_504)

                               // commands of Menu 17

  AbsParmGeneral<double> _ptjim_Parm;// Jimmy min pt sec scat (H 6_505)
  AbsParmGeneral<int> _msflag_Parm;// Jimmy on or off (H 6_505)
  AbsParmGeneral<double> _jmrad73_Parm;// Jimmy prot radius inv-sq (H 6_505)
  AbsParmGeneral<double> _jmrad91_Parm;// Jimmy pbar radius inv-sq (H 6_505)

  // event counter
  int _events;

private: // 2 private ct's
  HerwigModule(const char* const theName, 
	       const char* const theDescription): 
    AbsGenModule( theName, theDescription ),
    _process("Process",this,1705), 
    
    _maxer_Parm("Maxer",this,100),
    _randomseed1_Parm("Randseed1",this,0),
    _randomseed2_Parm("Randseed2",this,0),
    _lambdaQCD_Parm( "Lambda_QCD", this, 0.18),
    
				// commands of Menu 01: The Beam menu
				// ----------------------------------
    _Beamtype1_Parm("Beam1", this, "12345678"),
    _Beamtype2_Parm("Beam2", this , "12345678"),
    _Energy1_Parm("Pbeam1", this,980.),
    _Energy2_Parm("Pbeam2", this,980.),
    _pipsmr_Parm("Pipsmr",this,0),
    _vipwidx_Parm("Vipwidx",this,0.25),
    _vipwidy_Parm("Vipwidy",this,0.015),
    _vipwidz_Parm("Vipwidz",this,1.8),


				// commands of menu 02: The Sudakov menu
				// ----------------------------------
    _sudord_Parm("Sudakov_order",this,2),

				// commands of menu 03: The Structure Function Menu
    _autpdf("Autpdf",this,"MRS"),
    _modpdf("Modpdf",this,28),
    _nstru_Parm("Nstru",this,8),

				// commands of menu 04: The Hard Scattering Menu
				// ---------------------------------------------
//cjl  _rapiditiesm_Parm("Ymin",this,-8.0),    
    _rapiditiesM_Parm("Ymax",this,8.0),    
    _ptm_Parm("Ptmin",this,0),
    _ptM_Parm("Ptmax",this,0),
    _invarmassm_Parm("MassMin",this,10.),
    _invarmassM_Parm("MassMax",this,1.E8),     
    _thrust_Parm("ThrustMax",this,0.9),
    _ptpow_Parm("Ptpow",this,4.0),
    _empow_Parm("Empow",this,4.00),
    _q2pow_Parm("Q2power",this,2.5),
    _qlim_Parm("Qlim",this,1.E8),
    _q2dilsm_Parm("Q2dilsmin",this,0),
    _q2dilsM_Parm("Q2dilsmax",this,0),
    _bgshat_Parm("Bgshat",this, false),
    
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
    _softme_Parm("SoftME",this,true),
    _intrinsicpt_Parm("Pt_incoming_hadrons",this,0),
    _clreco_Parm("Clreco",this,false),
    _preco_Parm("Preco",this,0.111111),
    
				// commands of Menu 06: Hadronization Menu
				// ---------------------------------------
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
    _sngwt_Parm("Sngwt",this,1.0),
    _decwt_Parm("Decwt",this,1.0),
    _pwt1_Parm("Pwt1",this,1.0),
    _pwt2_Parm("Pwt2",this,1.0),
    _pwt3_Parm("Pwt3",this,1.0),
    _pwt4_Parm("Pwt4",this,1.0),
    _pwt5_Parm("Pwt5",this,1.0),
    _pwt6_Parm("Pwt6",this,1.0),
    _pwt7_Parm("Pwt7",this,1.0),
    
				// commands of Menu 07: Underlying Event Menu
				// -------------------------------------------
    _prsof_Parm("Prsof",this,1.00),
    _ensof_Parm("Ensof",this,1.00),
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

				// commands of Menu 08: Decays Menu
				// --------------------------------
    _pizstable_Parm("Pizstable",this,0),
    _topltim_Parm("Topltim",this,5.534E-25),
    _pltcut_Parm("Pltcut",this,0.),
    _modbos1_Parm("Modbos1",this,0),
    _modbos2_Parm("Modbos2",this,0),
    
				// commands of Menu 09: Generation Menu
				// ------------------------------------
    _eventweight_Parm("No_weights",this,1),
    _maxweight_Parm("Maxwt",this,0),
    _negwts_Parm("Negwts",this,0),
    _effmin_Parm("Effmin",this,0.001),
    _noshower_Parm("No_shower",this,0),
    _nohadroniz_Parm("No_hadronization",this,0),
    _tlout_Parm("Tlout",this,10.0),
  
				// commands of Menu 10: Parameters Menu
				// ------------------------------------
    _colors_Parm("Colors",this,3),
    _flavors_Parm ("Flavors", this,6),
    _weiangle_Parm("Weinberg_angle", this,0.2319),  
    _cabangle_Parm("Cabbibo_angle",this,0.0488),
    _wwidth_Parm("W_width", this,2.12),
    _zwidth_Parm("Z_width", this,2.495),
    
				// commands of Menu 11: Masses Menu
				// --------------------------------
    _down_Parm("down", this, 0.32),
    _up_Parm("up", this, 0.32),
    _strange_Parm("strange", this, 0.50),
    _charm_Parm("charm",this, 1.55),
    _bottom_Parm("bottom", this,4.95), 
    _top_Parm("top",this, 174.3),
    _gluonmass_Parm("gluon",this,0.75),  
    _W_Parm("W", this,80.42),
    _Z0_Parm("Z0",this,91.188),
    _higgs_Parm("Higgs",this,115.), 
    _gammax_Parm("Gammax",this,10.), 
    
				// commands of Menu 12: Readsusy Menu
				// -----------------------------------
    _read_Filesusy("File_Readsusy",this,"$HERWIG_SUSY"),
    _rparty_Parm("Rparty",this,1),

				// commands of Menu 13: Zprime Menu
				// --------------------------------
    _zprime_Parm("Zprime",this,0),
    _ZP_Parm("ZP",this,500.),
    _zpwidth_Parm("ZP_width",this,5.),

				// commands of Menu 14: Prints Menu 
				// --------------------------------
    _iprint_Parm("iprint",this,1),
    _maxpr_Parm("maxpr",this,0),
    _prvtx_Parm("prvtx",this,1),
    _prndef_Parm("prndef",this,1),
    _prntex_Parm("prntex",this,0),
    _prnweb_Parm("prnweb",this,0),
    _nprfmt_Parm("nprfmt",this,1),

				// commands of Menu 15: Vers64 Menu 
				// --------------------------------
    _optm_Parm("Optm",this,0),
    _iopstp_Parm("Iopstp",this,5),
    _iopsh_Parm("Iopsh",this,100000),
    _syspin_Parm("Syspin",this,1),
    _threeb_Parm("Threeb",this,1),
    _fourb_Parm("Fourb",this,0),
    _lrdec_Parm("Lrdec",this,0),
    _lwdec_Parm("Lwdec",this,88),

				// commands of Menu 16: Vers65 Menu 
				// --------------------------------
    _prespl_Parm("Prespl",this,1),
    _lhsoft_Parm("Lhsoft",this,1),
    _lhglsf_Parm("Lhglsf",this,0),
    _grvlam_Parm("Grvlam",this,10000.),
    _emgrv_Parm("Emgrv",this,1000.),
    _gamgrv_Parm("Gamgrv",this,0.),
    _itoprd_Parm("Itoprd",this,0),

                               // commands of Menu 17: Jimmy Menu
                               // --------------------------------

    _ptjim_Parm("Ptjim",this,2.5),
    _msflag_Parm("Msflag",this,1),
    _jmrad73_Parm("Jmrad73",this,0.71),
    _jmrad91_Parm("Jmrad91",this,0.71)

  {}

  HerwigModule(const HerwigModule& m) :
    AbsGenModule( "x", "x" ),
    _process("Process",this,1705), 
    
    _maxer_Parm("Maxer",this,100),
    _randomseed1_Parm("Randseed1",this,0),
    _randomseed2_Parm("Randseed2",this,0),
    _lambdaQCD_Parm( "Lambda_QCD", this, 0.18),
    
				// commands of Menu 01: The Beam menu
				// ----------------------------------
    _Beamtype1_Parm("Beam1", this, "12345678"),
    _Beamtype2_Parm("Beam2", this , "12345678"),
    _Energy1_Parm("Pbeam1", this,980.),
    _Energy2_Parm("Pbeam2", this,980.),
    _pipsmr_Parm("Pipsmr",this,0),
    _vipwidx_Parm("Vipwidx",this,0.25),
    _vipwidy_Parm("Vipwidy",this,0.015),
    _vipwidz_Parm("Vipwidz",this,1.8),


				// commands of menu 02: The Sudakov menu
				// ----------------------------------
    _sudord_Parm("Sudakov_order",this,2),

				// commands of menu 03: The Structure Function Menu
    _autpdf("Autpdf",this,"MRS"),
    _modpdf("Modpdf",this,28),
    _nstru_Parm("Nstru",this,8),

				// commands of menu 04: The Hard Scattering Menu
				// ---------------------------------------------
//cjl  _rapiditiesm_Parm("Ymin",this,-8.0),    
    _rapiditiesM_Parm("Ymax",this,8.0),    
    _ptm_Parm("Ptmin",this,0),
    _ptM_Parm("Ptmax",this,0),
    _invarmassm_Parm("MassMin",this,10.),
    _invarmassM_Parm("MassMax",this,1.E8),     
    _thrust_Parm("ThrustMax",this,0.9),
    _ptpow_Parm("Ptpow",this,4.0),
    _empow_Parm("Empow",this,4.00),
    _q2pow_Parm("Q2power",this,2.5),
    _qlim_Parm("Qlim",this,1.E8),
    _q2dilsm_Parm("Q2dilsmin",this,0),
    _q2dilsM_Parm("Q2dilsmax",this,0),
    _bgshat_Parm("Bgshat",this, false),
    
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
    _softme_Parm("SoftME",this,true),
    _intrinsicpt_Parm("Pt_incoming_hadrons",this,0),
    _clreco_Parm("Clreco",this,false),
    _preco_Parm("Preco",this,0.111111),
    
				// commands of Menu 06: Hadronization Menu
				// ---------------------------------------
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
    _sngwt_Parm("Sngwt",this,1.0),
    _decwt_Parm("Decwt",this,1.0),
    _pwt1_Parm("Pwt1",this,1.0),
    _pwt2_Parm("Pwt2",this,1.0),
    _pwt3_Parm("Pwt3",this,1.0),
    _pwt4_Parm("Pwt4",this,1.0),
    _pwt5_Parm("Pwt5",this,1.0),
    _pwt6_Parm("Pwt6",this,1.0),
    _pwt7_Parm("Pwt7",this,1.0),
    
				// commands of Menu 07: Underlying Event Menu
				// -------------------------------------------
    _prsof_Parm("Prsof",this,1.00),
    _ensof_Parm("Ensof",this,1.00),
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

				// commands of Menu 08: Decays Menu
				// --------------------------------
    _pizstable_Parm("Pizstable",this,0),
    _topltim_Parm("Topltim",this,5.534E-25),
    _pltcut_Parm("Pltcut",this,0.),
    _modbos1_Parm("Modbos1",this,0),
    _modbos2_Parm("Modbos2",this,0),
    
				// commands of Menu 09: Generation Menu
				// ------------------------------------
    _eventweight_Parm("No_weights",this,1),
    _maxweight_Parm("Maxwt",this,0),
    _negwts_Parm("Negwts",this,0),
    _effmin_Parm("Effmin",this,0.001),
    _noshower_Parm("No_shower",this,0),
    _nohadroniz_Parm("No_hadronization",this,0),
    _tlout_Parm("Tlout",this,10.0),
  
				// commands of Menu 10: Parameters Menu
				// ------------------------------------
    _colors_Parm("Colors",this,3),
    _flavors_Parm ("Flavors", this,6),
    _weiangle_Parm("Weinberg_angle", this,0.2319),  
    _cabangle_Parm("Cabbibo_angle",this,0.0488),
    _wwidth_Parm("W_width", this,2.12),
    _zwidth_Parm("Z_width", this,2.495),
    
				// commands of Menu 11: Masses Menu
				// --------------------------------
    _down_Parm("down", this, 0.32),
    _up_Parm("up", this, 0.32),
    _strange_Parm("strange", this, 0.50),
    _charm_Parm("charm",this, 1.55),
    _bottom_Parm("bottom", this,4.95), 
    _top_Parm("top",this, 174.3),
    _gluonmass_Parm("gluon",this,0.75),  
    _W_Parm("W", this,80.42),
    _Z0_Parm("Z0",this,91.188),
    _higgs_Parm("Higgs",this,115.), 
    _gammax_Parm("Gammax",this,10.), 
    
				// commands of Menu 12: Readsusy Menu
				// -----------------------------------
    _read_Filesusy("File_Readsusy",this,"$HERWIG_SUSY"),
    _rparty_Parm("Rparty",this,1),

				// commands of Menu 13: Zprime Menu
				// --------------------------------
    _zprime_Parm("Zprime",this,0),
    _ZP_Parm("ZP",this,500.),
    _zpwidth_Parm("ZP_width",this,5.),

				// commands of Menu 14: Prints Menu 
				// --------------------------------
    _iprint_Parm("iprint",this,1),
    _maxpr_Parm("maxpr",this,0),
    _prvtx_Parm("prvtx",this,1),
    _prndef_Parm("prndef",this,1),
    _prntex_Parm("prntex",this,0),
    _prnweb_Parm("prnweb",this,0),
    _nprfmt_Parm("nprfmt",this,1),

				// commands of Menu 15: Vers64 Menu 
				// --------------------------------
    _optm_Parm("Optm",this,0),
    _iopstp_Parm("Iopstp",this,5),
    _iopsh_Parm("Iopsh",this,100000),
    _syspin_Parm("Syspin",this,1),
    _threeb_Parm("Threeb",this,1),
    _fourb_Parm("Fourb",this,0),
    _lrdec_Parm("Lrdec",this,0),
    _lwdec_Parm("Lwdec",this,88),

				// commands of Menu 16: Vers65 Menu 
				// --------------------------------
    _prespl_Parm("Prespl",this,1),
    _lhsoft_Parm("Lhsoft",this,1),
    _lhglsf_Parm("Lhglsf",this,0),
    _grvlam_Parm("Grvlam",this,10000.),
    _emgrv_Parm("Emgrv",this,1000.),
    _gamgrv_Parm("Gamgrv",this,0.),
    _itoprd_Parm("Itoprd",this,0),

                                 // commands of Menu 17: Jimmy Menu
                                 // --------------------------------
     _ptjim_Parm("Ptjim",this,2.5),
     _msflag_Parm("Msflag",this,1),
     _jmrad73_Parm("Jmrad73",this,0.71),
     _jmrad91_Parm("Jmrad91",this,0.71)

{}


};

#endif




