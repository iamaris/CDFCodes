//--------------------------------------------------------------------
// HerwigMenu.cc
//
// created Nov 26 2001, Elena Gerchtein (CMU)
//
// Revision History:
//     Mar 2005  tsuno, add talk-to for SOFTME
//--------------------------------------------------------------------
#include "generatorMods/HerwigMenu.hh"
#include "herwig_i/Herwig.hh"

#include <iostream>
#include <iomanip>
#include <math.h>
using std::cout;
using std::endl;
using std::setw;
using std::string;

#include <string>
#include <list>

HerwigMenu::HerwigMenu( AppModule* target,
			APPMenu* topMenu,
			std::string menuName)
  :
  APPMenu(),
  _process("Process",target,1705), 

  _maxer_Parm("Maxer",target,100),
  _randomseed1_Parm("Randseed1",target,0),
  _randomseed2_Parm("Randseed2",target,0),
  _lambdaQCD_Parm( "Lambda_QCD", target, 0.18),
   
				// commands of Menu 01: The Beam menu
				// ----------------------------------
  _01Menu(),
  _Beamtype1_Parm("Beam1", target, "12345678"),
  _Beamtype2_Parm("Beam2", target , "12345678"),
  _Energy1_Parm("Pbeam1", target,980.),
  _Energy2_Parm("Pbeam2", target,980.),
  _pipsmr_Parm("Pipsmr",target,0),
  _vipwidx_Parm("Vipwidx",target,0.25),
  _vipwidy_Parm("Vipwidy",target,0.015),
  _vipwidz_Parm("Vipwidz",target,1.8),


				// commands of menu 02: The Sudakov menu
				// ----------------------------------
  _02Menu(),
  _sudord_Parm("Sudakov_order",target,2),

				// commands of menu 03: The Structure Function Menu
  _03Menu(),
  _autpdf("Autpdf",target,"MRS"),
  _modpdf("Modpdf",target,0),
  _nstru_Parm("Nstru",target,8),

				// commands of menu 04: The Hard Scattering Menu
				// ---------------------------------------------
//cjl  _rapiditiesm_Parm("Ymin",target,-8.0),    
  _04Menu(),
  _rapiditiesM_Parm("Ymax",target,8.0),    
  _ptm_Parm("Ptmin",target,0),
  _ptM_Parm("Ptmax",target,0),
  _invarmassm_Parm("MassMin",target,10.),
  _invarmassM_Parm("MassMax",target,1.E8),     
  _thrust_Parm("ThrustMax",target,0.9),
  _ptpow_Parm("Ptpow",target,4.0),
  _empow_Parm("Empow",target,4.00),
  _q2pow_Parm("Q2power",target,2.5),
  _qlim_Parm("Qlim",target,1.E8),
  _q2dilsm_Parm("Q2dilsmin",target,0),
  _q2dilsM_Parm("Q2dilsmax",target,0),
  _bgshat_Parm("Bgshat",target, false),
    
				// commands of Menu 05: Parton Shower Menu
				// ---------------------------------------
  _05Menu(),
  _azsoft_Parm("Azsoft",target,true),
  _azspin_Parm("Azspin",target,true),
  _qspac_Parm("Spacelike_evolution",target,2.5),
  _ispac_Parm("Ispac",target,0),
  _nospac_Parm("Nospac",target,0),
  _vqcut_Parm("Qcutoff",target,0.48),
  _vgcut_Parm("Gcutoff",target,0.10),
  _vpcut_Parm("Pcutoff",target,0.40),
  _softme_Parm("SoftME",target,true),
  _intrinsicpt_Parm("Pt_incoming_hadrons",target,0),
  _clreco_Parm("Clreco",target,false),
  _preco_Parm("Preco",target,0.111111),
  
				// commands of Menu 06: Hadronization Menu
				// ---------------------------------------
  _06Menu(),
  _clmax_Parm("Clmax", target,3.35), 
  _clpow_Parm("Clpow",target,2.0),  
  _psplt1_Parm("Psplt1",target,1.0),  
  _psplt2_Parm("Psplt2",target,1.0),  
  _cldir1_Parm("Cldir1",target,1),  
  _cldir2_Parm("Cldir2",target,1),  
  _clsmr1_Parm("Clsmr1",target,0.0),  
  _clsmr2_Parm("Clsmr2",target,0.0),  
  _qdiqk_Parm("Qdiqk",target,0.00), 
  _pdiqk_Parm("Pdiqk",target,5.00), 
  _sngwt_Parm("Sngwt",target,1.0),
  _decwt_Parm("Decwt",target,1.0),
  _pwt1_Parm("Pwt1",target,1.0),
  _pwt2_Parm("Pwt2",target,1.0),
  _pwt3_Parm("Pwt3",target,1.0),
  _pwt4_Parm("Pwt4",target,1.0),
  _pwt5_Parm("Pwt5",target,1.0),
  _pwt6_Parm("Pwt6",target,1.0),
  _pwt7_Parm("Pwt7",target,1.0),

				// commands of Menu 07: Underlying Event Menu
				// -------------------------------------------
  _07Menu(),
  _prsof_Parm("Prsof",target,1.00),
  _ensof_Parm("Ensof",target,1.00),
  _pmbn1_Parm("Pmbn1",target,9.11),
  _pmbn2_Parm("Pmbn2",target,0.115),
  _pmbn3_Parm("Pmbn3",target,9.50),
  _pmbk1_Parm("Pmbk1",target,0.029),
  _pmbk2_Parm("Pmbk2",target,0.104),
  _pmbm1_Parm("Pmbm1",target,0.4),
  _pmbm2_Parm("Pmbm2",target,2.0),
  _pmbp1_Parm("Pmbp1",target,5.2),
  _pmbp2_Parm("Pmbp2",target,3.0),
  _pmbp3_Parm("Pmbp3",target,5.2),

				// commands of Menu 08: Decays Menu
				// --------------------------------
  _08Menu(),
  _pizstable_Parm("Pizstable",target,0),
  _topltim_Parm("Topltim",target,5.534E-25),
  _pltcut_Parm("Pltcut",target,0.),
  _modbos1_Parm("Modbos1",target,0),
  _modbos2_Parm("Modbos2",target,0),

				// commands of Menu 09: Generation Menu
				// ------------------------------------
  _09Menu(),
  _eventweight_Parm("No_weights",target,1),
  _maxweight_Parm("Maxwt",target,0),
  _negwts_Parm("Negwts",target,0),
  _effmin_Parm("Effmin",target,0.001),
  _noshower_Parm("No_shower",target,0),
  _nohadroniz_Parm("No_hadronization",target,0),
  _tlout_Parm("Tlout",target,10.0),
  
				// commands of Menu 10: Parameters Menu
				// ------------------------------------
  _10Menu(),
  _colors_Parm("Colors",target,3),
  _flavors_Parm ("Flavors", target,6),
  _weiangle_Parm("Weinberg_angle", target,0.2319),  
  _cabangle_Parm("Cabbibo_angle",target,0.0488),
  _wwidth_Parm("W_width", target,2.12),
  _zwidth_Parm("Z_width", target,2.495),
  
				// commands of Menu 11: Masses Menu
				// --------------------------------
  _11Menu(),
  _down_Parm("down", target, 0.32),
  _up_Parm("up", target, 0.32),
  _strange_Parm("strange", target, 0.50),
  _charm_Parm("charm",target, 1.55),
  _bottom_Parm("bottom", target,4.95), 
  _top_Parm("top",target, 174.3),
  _gluonmass_Parm("gluon",target,0.75),  
  _W_Parm("W", target,80.42),
  _Z0_Parm("Z0",target,91.188),
  _higgs_Parm("Higgs",target,115.), 
  _gammax_Parm("Gammax",target,10.), 

				// commands of Menu 12: Readsusy Menu
				// -----------------------------------
  _12Menu(),
  _read_Filesusy("File_Readsusy",target,"$HERWIG_SUSY"),
  _rparty_Parm("Rparty",target,1),

				// commands of Menu 13: Zprime Menu
				// --------------------------------
  _13Menu(),
  _zprime_Parm("Zprime",target,0),
  _ZP_Parm("ZP",target,500.),
  _zpwidth_Parm("ZP_width",target,5.),

				// commands of Menu 14: Prints Menu 
				// --------------------------------
  _14Menu(),
  _iprint_Parm("iprint",target,1),
  _maxpr_Parm("maxpr",target,0),
  _prvtx_Parm("prvtx",target,1),
  _prndef_Parm("prndef",target,1),
  _prntex_Parm("prntex",target,0),
  _prnweb_Parm("prnweb",target,0),
  _nprfmt_Parm("nprfmt",target,1),

				// commands of Menu 15: Vers64 Menu 
				// --------------------------------
  _15Menu(),
  _optm_Parm("Optm",target,0),
  _iopstp_Parm("Iopstp",target,5),
  _iopsh_Parm("Iopsh",target,100000),
  _syspin_Parm("Syspin",target,1),
  _threeb_Parm("Threeb",target,1),
  _fourb_Parm("Fourb",target,0),
  _lrdec_Parm("Lrdec",target,0),
  _lwdec_Parm("Lwdec",target,88),

                                // commands of Menu 16: Vers65 Menu
                                // --------------------------------
  _16Menu(),
  _prespl_Parm("Prespl",target,1),
  _lhsoft_Parm("Lhsoft",target,1),
  _lhglsf_Parm("Lhglsf",target,0),
  _grvlam_Parm("Grvlam",target,10000.),
  _emgrv_Parm("Emgrv",target,1000.),
  _gamgrv_Parm("Gamgrv",target,0.),
  _itoprd_Parm("Itoprd",target,0),

                                // commands of Menu 17: Jimmy Menu
                                // --------------------------------
  _17Menu(),
  _ptjim_Parm("Ptjim",target,2.5),
  _msflag_Parm("Msflag",target,1),
  _jmrad73_Parm("Jmrad73",target,0.71),
  _jmrad91_Parm("Jmrad91",target,0.71)

{
  // this menu
  initialize("HerwigMenu",target,topMenu);
  initTitle("Herwig menu");
  InitHerwigMenu();

  Absparm2Her();
  Her2Absparm();
}

void HerwigMenu::InitHerwigMenu() {

  // Initialize all the menus
  // ------------------------
  _01Menu.initialize ("Beam",target(),this);
  _01Menu.initTitle ("Beam Menu:  Beam types and momenta");
  
  _02Menu.initialize ("Sudakov",target(),this);
  _02Menu.initTitle ("alpha_s order in Sudakov form factors"); 
  
  _03Menu.initialize ("Struc_Function",target(),this);
  _03Menu.initTitle ("Select structure function set");
  
  _04Menu.initialize ("Hards",target(),this);
  _04Menu.initTitle ("Hard scattering process params menu"); 
  
  _05Menu.initialize ("Shower",target(),this);
  _05Menu.initTitle ("Evokes shower parameters menu"); 
  
  _06Menu.initialize ("Hadronization",target(),this);
  _06Menu.initTitle ("Evokes hadronization parameters menu");  
  
  _07Menu.initialize ("Underl_event",target(),this);
  _07Menu.initTitle ("Underlying soft event parameters menu"); 
  
  _08Menu.initialize ("Decays",target(),this);
  _08Menu.initTitle ("Controls Particle Decays");
  
  _09Menu.initialize ("Generation",target(),this);
  _09Menu.initTitle ("Evokes  generation parameter menu");
  
  _10Menu.initialize ("Sm_par",target(),this);
  _10Menu.initTitle("Evokes standard model parameters menu");
  
  _11Menu.initialize ("Masses",target(),this);
  _11Menu.initTitle ("Change particle masses"); 
  
  _12Menu.initialize ("Read_susy",target(),this);
  _12Menu.initTitle ("Read susy file");
  
  _13Menu.initialize ("Zprime",target(),this);
  _13Menu.initTitle ("Control Z-prime");
  
  _14Menu.initialize ("Prints",target(),this);
  _14Menu.initTitle ("Control printing");
  
  _15Menu.initialize ("Vers64",target(),this);
  _15Menu.initTitle ("Version 6_400 adds");
  
  _16Menu.initialize ("Vers65",target(),this);
  _16Menu.initTitle ("Version 6_500 adds");

  _17Menu.initialize ("Jimmy",target(),this);
  _17Menu.initTitle ("Version 6_505 Jimmy adds");
  

  // Add descriptions for AbsParms
  // -------------------------------
  _process.addDescription(
 "\t\t\tSpecify integer code for process\n\
  \t\t\t see Herwig Documentation for codes\n\
  \t\t\t The syntax is: Process set <code>");
  _maxer_Parm.addDescription(
			     "\t\t\tNumber of errors allowed before run stops\n\
  \t\t\t The syntax is: Maxer set <maxer> ");
  _randomseed1_Parm.addDescription(
				   "\t\t\tHerwig random number seed 1\n\
  \t\t\t The syntax is: Randseed1 set <seed> ");
  _randomseed2_Parm.addDescription(
				   "\t\t\tHerwig random number seed 2\n\
  \t\t\t The syntax is: Randseed2 set <seed> ");
  
  _lambdaQCD_Parm.addDescription( 
				 "\t\t\tSpecify the value of lamba QCD.\n\
  \t\t\tThe syntax is: Lambda_QCD set <lambda>\n\
  \t\t\twhere <lambda> is specified in GEV" );
  
  //   _decayTable_Parm.addDescription(
  //"         Specify the name of the file used to read the decay table.\n\
    //         The format of the command is: decay_table set <filename>" );
    
    //   _scaleChoiceParm.addDescription(
    //"          Select the scale choice to be used.\n\
    //          The syntax of the command is: scale_choice set <choice>\n\
    //          where <choice> is a integer between 1 and 3:\n\
    //          1 is mu = PT/2 \n\
    //          2 is mu = PT \n\
    //          3 is mu = 2PT" );
    //
    // Descriptions of Beam Parameters, Menu 01
    // ----------------------------------------
    _Beamtype1_Parm.addDescription(
				   "\t\t\tSpecify the beam particle type \n\
  \t\t\tThe syntax is: Beam1 set <Beam1> \n\
  \t\t\twhere <Beam1> is a string variable (eg P or PBAR)\n\
  \t\t\tand where the particle type must be in capitals");
    
    _Beamtype2_Parm.addDescription(
				   "\t\t\tSpecify the beam particle type \n\
  \t\t\tThe syntax is: Beam2 set <Beam2> \n\
  \t\t\twhere <Beam2> is a string variable (eg P or PBAR)\n\
  \t\t\tand where the particle type must be in capitals");
    
    _Energy1_Parm.addDescription(
				 "\t\t\tSet Beam1 momentum.\n\
  \t\t\tThe syntax is: Pbeam1 set <mom1>\n\
  \t\t\twhere <mom1> is specified in GEV " );
    
    _Energy2_Parm.addDescription(
				 "\t\t\tSet Beam2 momentum.\n\
  \t\t\tThe syntax is: Pbeam2 set <mom2>\n\
  \t\t\twhere <mom2> is specified in GEV " );

   _pipsmr_Parm.addDescription(
"\t\t\tWhether to smear primary vertex.\n\
 \t\t\tThe syntax is: Pipsmr set <pipsmr>\n\
 \t\t\twhere <pipsmr> is 1 (yes) or 0 (no)" );

   _vipwidx_Parm.addDescription(
"\t\t\tSet x width for smeared primary vertex.\n\
 \t\t\tThe syntax is: Vipwidx set <vipwidx>\n\
 \t\t\twhere <vipwidx> is specified in mm " );

   _vipwidy_Parm.addDescription(
"\t\t\tSet y width for smeared primary vertex.\n\
 \t\t\tThe syntax is: Vipwidy set <vipwidy>\n\
 \t\t\twhere <vipwidy> is specified in mm " );

   _vipwidz_Parm.addDescription(
"\t\t\tSet z width for smeared primary vertex.\n\
 \t\t\tThe syntax is: Vipwidz set <vipwidz>\n\
 \t\t\twhere <vipwidz> is specified in mm " );

    //
    // Descriptions of Sudakov Parameters, Menu 02
    // -------------------------------------------
    
    _sudord_Parm.addDescription(
				"\t\t\tSpecify the order of Sudakov calculation.\n\
  \t\t\tThe syntax is: Sudakov_order set <sudakov_order>\n\
  \t\t\twhere <sudakov_order> is an integer, 1 or 2");
    
    //
    // Descriptions of Structure Function Parameters, Menu 03
    // ------------------------------------------------------
    _autpdf.addDescription(
			   "\t\t\tAuthors of PDF function (MRS,CTEQ,etc), string variable");
    
    _modpdf.addDescription(
			   "\t\t\tPDF number for PDFLIB as integer");

    _nstru_Parm.addDescription(
  "\t\t\tHerwig PDF number, used if modpdf.lt.0,\n\
   \t\t\t 1-5 are old, 6-8 are MRST1998, as integer");
    
    //
    // Descriptions of Hard Scattering Parameters, Menu 04
    // ---------------------------------------------------
    //cjl
    //  _rapiditiesm_Parm.addDescription(
    //"\t\t\tSet minimum rapidity.\n\
      // \t\t\tThe command is effective for:\n\
      // \t\t\t\t IPROC=1500 QCD 2->2 \n\
      // \t\t\t\t IPROC=17** Heavy flavor production \n\
      // \t\t\t\t IPROC=1800 Direct photon production\n\
      // \t\t\tThe syntax is: Ymin set <minrap>");
      //cjl_e
      _rapiditiesM_Parm.addDescription(
				       "\t\t\tSet maximum rapidity.\n\
  \t\t\tThe command is effective for:\n\
  \t\t\t\t IPROC=1500 QCD 2->2 \n\
  \t\t\t\t IPROC=17** Heavy flavor production \n\
  \t\t\t\t IPROC=1800 Direct photon production\n\
  \t\t\tThe syntax is: Ymax set <maxrap>");
      
      _ptm_Parm.addDescription(
			       "\t\t\tSet minimum transverse momentum.\n\
  \t\t\tThe syntax is: Ptmin set <ptm>\n\
  \t\t\twhere <ptm> is specified in GEV" );
      
      _ptM_Parm.addDescription(
			       "\t\t\tSet maximum transverse momentum.\n\
  \t\t\tThe syntax is: Ptmax set <ptM>\n\
  \t\t\twhere <ptM> is specified in GEV" );
      
      _invarmassm_Parm.addDescription(
				      "\t\t\tSet min invariant mass limits for Drell-Yan process.\n\
  \t\t\tThe syntax is: MassMin  set <invarmass>\n\
  \t\t\twhere <invarmassm> is specified in GEV " );
      
      _invarmassM_Parm.addDescription(
				      "\t\t\tSet max invariant mass limits for Drell-Yan process.\n\
  \t\t\tThe syntax is: MassMax  set <invarmassM>\n\
  \t\t\twhere <invarmassM> is specified in GEV " );
      
      _thrust_Parm.addDescription(
				  "\t\t\tMax thrust for 2->3 process.\n\
  \t\t\tThe syntax is: ThrustMax set <thrust>");
      
      _ptpow_Parm.addDescription(
				 "\t\t\tPower of importance sampling in QCD 2->2 process.\n\
  \t\t\td(sigma)/dEt fall like ET**(-Ptpow)\n\
  \t\t\tThe syntax is: Ptpow  set <ptpow>" );
      
      _empow_Parm.addDescription(
				 "\t\t\tPower of importance sampling in DRELL-YAN process.\n\
  \t\t\td(sigma)/dM falls like M**(-Empow)\n\
  \t\t\tThe syntax is: Empow set <empw> " );
      
      _q2pow_Parm.addDescription(
				 "\t\t\tQ**2 power for importance sampling in DILS.\n\
  \t\t\tThe syntax is: Q2power  set <q2pow>\n\
  \t\t\twhere <q2pow> is a positive real " );
      
      _qlim_Parm.addDescription(
				"\t\t\tUpper limit on hard process scale.\n\
  \t\t\tThe syntax is: Qlim set <qlim>");
      
      _q2dilsm_Parm.addDescription(
				   "\t\t\tQ**2 min limit for deep inelastic lepton scattering.\n\
  \t\t\tThe syntax is: Q2dilsmin  set <q2dilsm>");
      
      _q2dilsM_Parm.addDescription(
				   "\t\t\tQ**2 max limit for deep inelastic lepton scattering.\n\
  \t\t\tThe syntax is: Q2dilsmax set <q2dilsM>");
      
      _bgshat_Parm.addDescription(
				  "\t\t\tScale switch for boson-gluon fusion.\n\
  \t\t\tThe syntax is: Bgshat  set <bgshat>\n\
  \t\t\twhere <bgshat> is 1 (true) or 0 (false) " );
      
      //
      // Parton Shower Descriptions, Menu 05
      // -----------------------------------
      _azsoft_Parm.addDescription(
				  "\t\t\tSwitch on azimuthal correlation for soft gluons.\n\
  \t\t\tThe syntax is: Azsoft set <azsoft>\n\
  \t\t\twhere <azsoft> is 1(true) or 0(false) " );
      
      _azspin_Parm.addDescription(
				  "\t\t\tSwitch on azimuthal correlation including nearest\n\
  \t\t\t\tneighbor spin correlations.\n\
  \t\t\tThe syntax is: Azspin set <azspin>\n\
  \t\t\twhere <azspin> is  1(true) or 0(false) " );
      
      _qspac_Parm.addDescription(
				 "\t\t\tSet lower limit for spacelike evolution.\n\
  \t\t\tThe syntax is: Spacelike_evolution set <qspac>");
      
      _ispac_Parm.addDescription(
				 "\t\t\tSet ispac controlling qspac precise meaning.\n\
  \t\t\tThe syntax is: Ispac set <ispac>\n\
  \t\t\twhere <ispac> is 0 or 1 or 2 " );
      
      _nospac_Parm.addDescription(
				  "\t\t\tWhether to turn off ISR.\n\
  \t\t\tThe syntax is: Nospac set <nospac>\n\
  \t\t\twhere <nospac> is 1(true) or 0(false) " );
      
      _vqcut_Parm.addDescription(
				 "\t\t\tSet quark virtuality cutoff.\n\
  \t\t\tThe syntax is: Qcutoff set <vqcut>");
      
      _vgcut_Parm.addDescription(
				 "\t\t\tSet gluon virtuality cutoff.\n\
  \t\t\tThe syntax is: Gcutoff  set <vgcut>");
      
      _vpcut_Parm.addDescription(
				 "\t\t\tSet photon virtuality cutoff.\n\
  \t\t\tThe syntax is: Pcutoff  set <vpcut>");
      
      _softme_Parm.addDescription(
				 "\t\t\tSwitch on soft matrix-element correction.\n\
  \t\t\tThe syntax is: SoftME  set 1(true, default) or 0(false)");
      
      _intrinsicpt_Parm.addDescription(
				       "\t\t\tSet intrinsic pt of incoming hadrons.\n\
  \t\t\tThe syntax is: Pt_incoming_hadrons  set <intrinsicpt>");
      
      _clreco_Parm.addDescription(
				  "\t\t\tSwitch on colour rearrangement.\n\
  \t\t\tThe syntax is: Clreco set <clreco>\n\
  \t\t\twhere <clreco> is 1(true) or 0(false) " );
      
      _preco_Parm.addDescription(
				  "\t\t\tProbability for colour rearrangement.\n\
  \t\t\tThe syntax is: Preco set <preco> " );
      
      // Hadronization Descriptions, Menu 06
      // -----------------------------------
      _clmax_Parm.addDescription(
 "\t\t\tSet mass cluster mass parameter.\n\
  \t\t\tThe syntax is: Clmax set <clmax>\n\
  \t\t\twhere <clmax> is GEV" );
 
    _clpow_Parm.addDescription(
 "\t\t\tSet mass dependence of cluster hadronization.\n\
  \t\t\tThe syntax is: Clpow  set <clpow>");
 
    _psplt1_Parm.addDescription(
"\t\t\tSet mass distribution in cluster splitting, non-b.\n\
  \t\t\tThe syntax is: Psplt1  set <psplt1>");
 
    _psplt2_Parm.addDescription(
 "\t\t\tSet mass distribution in cluster splitting, b.\n\
  \t\t\tThe syntax is: Psplt2  set <psplt2>");
 
    _cldir1_Parm.addDescription(
 "\t\t\tSet remember direction in cluster decay to hads, non-b.\n\
  \t\t\tThe syntax is: Cldir1  set <cldir1>)\n\
  \t\t\twhere <cldir1> is 1(true) or 0(false) " );
 
    _cldir2_Parm.addDescription(
 "\t\t\tSet remember direction in cluster decay to hads, b.\n\
  \t\t\tThe syntax is: Cldir2  set <cldir2>)\n\
  \t\t\twhere <cldir2> is 1(true) or 0(false) " );
 
    _clsmr1_Parm.addDescription(
 "\t\t\tSet alignment correl. in cluster decay to hads, non-b.\n\
  \t\t\tThe syntax is: Clsmr1  set <clsmr1>");
 
    _clsmr2_Parm.addDescription(
 "\t\t\tSet alignment correl. in cluster decay to hads, b.\n\
  \t\t\tThe syntax is: Clsmr2  set <clsmr2>");
 
    _qdiqk_Parm.addDescription(
 "\t\t\tSet scale at which gluon can be split into diquarks.\n\
  \t\t\tThe syntax is: Qdiqk set <qdiqk>\n\
  \t\t\twhere <qdiqk> is in GEV " );
 
    _pdiqk_Parm.addDescription(
 "\t\t\tSet probability of diquark splitting per unit log scale.\n\
  \t\t\tThe syntax is: Pdiqk set <pdiqk>");
 
   _sngwt_Parm.addDescription(
"\t\t\tSet weights for singlet baryons.\n\
 \t\t\tThe syntax is: Sngwt set <sngwt>");

   _decwt_Parm.addDescription(
 "\t\t\tSet weights for decuplet baryons.\n\
  \t\t\tThe syntax is: Decwt set <decwt>");
 
   _pwt1_Parm.addDescription(
"\t\t\ta priori wt for d.dbar pairs in cluster decays.\n\
 \t\t\tThe syntax is: Pwt1 set <ptw1>");

   _pwt2_Parm.addDescription(
"\t\t\ta priori wt for u.ubar pairs in cluster decays.\n\
 \t\t\tThe syntax is: Pwt2 set <ptw2>");

   _pwt3_Parm.addDescription(
"\t\t\ta priori wt for s.sbar pairs in cluster decays.\n\
 \t\t\tThe syntax is: Pwt3 set <ptw3>");

   _pwt4_Parm.addDescription(
"\t\t\ta priori wt for c.cbar pairs in cluster decays.\n\
 \t\t\tThe syntax is: Pwt4 set <ptw4>");

   _pwt5_Parm.addDescription(
"\t\t\ta priori wt for b.bbar pairs in cluster decays.\n\
 \t\t\tThe syntax is: Pwt5 set <ptw5>");

   _pwt6_Parm.addDescription(
"\t\t\ta priori wt for t.tbar pairs in cluster decays.\n\
 \t\t\tThe syntax is: Pwt6 set <ptw6>");

   _pwt7_Parm.addDescription(
"\t\t\ta priori wt for qq.qqbar pairs in cluster decays.\n\
 \t\t\tThe syntax is: Pwt7 set <ptw7>");
 
    // Underlying Event Descriptions, Menu 07
    // --------------------------------------
   _prsof_Parm.addDescription(
"\t\t\tSet probability of underlying soft event.\n\
  \t\t\tThe syntax is: Prsof set <prsof>");

    _ensof_Parm.addDescription(
 "\t\t\tSet multiplicity enhancement for underlying soft event.\n\
   \t\t\tThe syntax is: Ensof set <ensof>");
 
    _pmbn1_Parm.addDescription(
 "\t\t\tSet soft interaction model parameter.\n\
   \t\t\tThe syntax is: Pmbn1 set <pmbn1>");
 
    _pmbn2_Parm.addDescription(
 "\t\t\tSet soft interaction model parameter.\n\
   \t\t\tThe syntax is: Pmbn2 set <pmbn2>");
 
    _pmbn3_Parm.addDescription(
 "\t\t\tSet (negative of) soft interaction model parameter.\n\
   \t\t\tThe syntax is: Pmbn3 set <pmbn3>");
 
    _pmbk1_Parm.addDescription(
 "\t\t\tSet soft interaction model parameter.\n\
   \t\t\tThe syntax is: Pmbk1 set <pmbk1>");
 
    _pmbk2_Parm.addDescription(
 "\t\t\tSet (negative of) soft interaction model parameter.\n\
   \t\t\tThe syntax is: Pmbk2 set <pmbk2>");
 
    _pmbm1_Parm.addDescription(
 "\t\t\tSet soft interaction model parameter.\n\
   \t\t\tThe syntax is: Pmbm1 set <pmbm1>");
 
    _pmbm2_Parm.addDescription(
 "\t\t\tSet soft interaction model parameter.\n\
   \t\t\tThe syntax is: Pmbm2 set <pmbm2>");
 
    _pmbp1_Parm.addDescription(
 "\t\t\tSet soft interaction model parameter.\n\
   \t\t\tThe syntax is: Pmbp1 set <pmbp1>");
 
    _pmbp2_Parm.addDescription(
 "\t\t\tSet soft interaction model parameter.\n\
   \t\t\tThe syntax is: Pmbp2 set <pmbp2>");
 
    _pmbp3_Parm.addDescription(
 "\t\t\tSet soft interaction model parameter.\n\
   \t\t\tThe syntax is: Pmbp3 set <pmbp3>");
 
    // Decays Descriptions, Menu 08
    // ----------------------------
    _pizstable_Parm.addDescription(
 "\t\t\tSet whether pizeros are to be stable.\n\
   \t\t\tThe syntax is: Pizstable set <pizstable>\n\
  \t\t\twhere <pizstable> is 1(true) or 0(false) " );

    _topltim_Parm.addDescription(
 "\t\t\tSet top quark lifetime, in seconds.\n\
   \t\t\tThe syntax is: Topltim set <topltim> " );

    _pltcut_Parm.addDescription(
 "\t\t\tLifetime cut in secs above which particles are stable.\n\
   \t\t\tThe syntax is: Pltcut set <pltcut> " );

   _modbos1_Parm.addDescription(
"\t\t\tSet force weak boson decay modes (see Table in Herwig6 doc).\n\
  \t\t\tThe syntax is: Modbos1 set <modbos(1)> " );

   _modbos2_Parm.addDescription(
"\t\t\tSet force weak boson decay modes (see Table in Herwig6 doc).\n\
  \t\t\tThe syntax is: Modbos2 set <modbos(2)> " );

    // Generation Parameter Descriptions, Menu 09
    // ------------------------------------------
    _eventweight_Parm.addDescription(
 "\t\t\tSpecify whether event are unweighted.\n\
  \t\t\tThe syntax is: No_weights set <nowt>\n\
  \t\t\tWhere <nowt> is 1 or 0.");
 
    //   _avwgt_Parm.addDescription(
    //"\t\t\tSet default mean event weight.\n\
    // \t\t\tThis parameter is only used if Use_weight=false\n\
   //  \t\t\tThe syntax is: Avwgt set <wt>");
 
    _maxweight_Parm.addDescription(
 "\t\t\tSets max weight.\n\
 \t\t\tThe syntax is: Maxwt set <maxweight>");
 
    _negwts_Parm.addDescription(
 "\t\t\tAllow negative weights, 1=yes 0=no\n\
 \t\t\tThe syntax is: Negwts set <negwts>");
 
    _effmin_Parm.addDescription(
 "\t\t\tSets min efficiency for event generation.\n\
 \t\t\tThe syntax is: Effmin set <effmin>");
 
    _noshower_Parm.addDescription(
 "\t\t\tTurn off parton shower.\n\
  \t\t\tThe syntax is: No_shower set <noshower>\n\
  \t\t\twhere <noshower> is true or false" );
 
    _nohadroniz_Parm.addDescription(
 "\t\t\tTurn off hadronization.\n\
  \t\t\tThe syntax is: No_hadronization set <nohadroniz>\n\
  \t\t\twhere <nohadroniz> is true or false" );
 
   _tlout_Parm.addDescription(
 "\t\t\ttime (sec) needed to terminate gracefully.\n\
  \t\t\tThe syntax is: Tlout   set <tlout>\n\
  \t\t\twhere <tlout> is in sec" );
 
    // SM-Parameter Menu item Descriptions, Menu 10
    // -------------------------------------------
 
    _colors_Parm.addDescription(
 "\t\t\tSet number of colors.\n\
  \t\t\tThe syntax is: Colors set <ncolors>");
 
    _flavors_Parm.addDescription(
 "\t\t\tSet number of flavors.\n\
  \t\t\tThe syntax is: Flavors set <flavors>");
 
    _weiangle_Parm.addDescription(
 "\t\t\tSet sin**2 of Weinberg angle.\n\
  \t\t\tThe syntax is: Weinberge_angle set <weiangle>\n\
  \t\t\twhere <weiangle> is real (0,1)" );
 
    _cabangle_Parm.addDescription(
 "\t\t\tSet sin**2 of Cabibbo angle.\n\
  \t\t\tThe syntax is: Cabibbo_angle set <cabangle>\n\
  \t\t\twhere <cabangle> is real (0,1)" );
    _wwidth_Parm.addDescription(
 "\t\t\tSet width of W boson.\n\
  \t\t\tThe syntax is: W_width set <wwidth>\n\
  \t\t\twhere <wwidth> is in GEV" );
 
    _zwidth_Parm.addDescription(
 "\t\t\tSet width of Z boson.\n\
  \t\t\tThe syntax is: Z_width set <zwidth>\n\
  \t\t\twhere <zwidth> is in GEV" );
 
    // Descriptions of Quark Mass Parameters, Menu 11
    // ----------------------------------------------
    _down_Parm.addDescription(
 "\t\t\tSet down quark mass.\n\
  \t\t\tThe syntax is: down   set <down>\n\
  \t\t\twhere <down> is in GEV" );
    _up_Parm.addDescription(
 "\t\t\tSet up quark mass.\n\
  \t\t\tThe syntax is: up   set <up>\n\
  \t\t\twhere <up> is in GEV" );
    _strange_Parm.addDescription(
 "\t\t\tSet strange quark mass.\n\
  \t\t\tThe syntax is: strange   set <strange>\n\
  \t\t\twhere <strange> is in GEV" );
    _charm_Parm.addDescription(
 "\t\t\tSet charm quark mass.\n\
  \t\t\tThe syntax is: charm   set <charm>\n\
  \t\t\twhere <charm> is in GEV" );
    _bottom_Parm.addDescription(
 "\t\t\tSet bottom quark mass.\n\
  \t\t\tThe syntax is: bottom   set <bottom>\n\
  \t\t\twhere <bottom> is in GEV" );
    _top_Parm.addDescription(
 "\t\t\tSet top quark mass.\n\
  \t\t\tThe syntax is: top   set <top>\n\
  \t\t\twhere <top> is in GEV" );
    _gluonmass_Parm.addDescription(
 "\t\t\tSet  gluon effective mass.\n\
  \t\t\tThe syntax is: gluon   set <gluonmass>\n\
  \t\t\twhere <gluonmass> is in GEV" );
    _W_Parm.addDescription(
 "\t\t\tSet W+/- mass.\n\
  \t\t\tThe syntax is: W   set <W>\n\
  \t\t\twhere <W> is in GEV" );
    _Z0_Parm.addDescription(
 "\t\t\tSet Z0 mass.\n\
  \t\t\tThe syntax is: Z0  set <Z0>\n\
  \t\t\twhere <Z0> is in GEV" );
    _higgs_Parm.addDescription(
 "\t\t\tSet  Higgs mass.\n\
  \t\t\tThe syntax is: Higgs   set <higgs>\n\
  \t\t\twhere <higgs> is in GEV" );
   _gammax_Parm.addDescription(
"\t\t\tSet limit on range of Higgs mass distribution.\n\
 \t\t\tThe syntax is: Gammax   set <gammax>\n\
 \t\t\twhere <gammax> multiplies the Higgs width" );

    // Read_susy Descriptions, Menu 12
    // --------------------------------------
 
    _read_Filesusy.addDescription(
 "\t\t\tSpecify the name of the file used to read the\n\
  \t\t\t\tSUSY particle properties.\n\
  \t\t\tThe format of the command is: File_Readsusy set <filename>" );
 
   _rparty_Parm.addDescription(
"\t\t\tSet whether R-parity is conserved in SUSY.\n\
 \t\t\tThe syntax is: Rparty   set <rparty>\n\
 \t\t\twhere <rparty> is 1 (yes) or 0 (no)" );

    // Z-prime Description, Menu 13
    // ----------------------------
    _zprime_Parm.addDescription(
 "\t\t\tWhether to use a Z-prime particle.\n\
  \t\t\tThe syntax is: Zprime  set <zprime>\n\
  \t\t\twhere <zprime> is 0 or 1");
 
    _ZP_Parm.addDescription(
 "\t\t\tSet mass of the Z-prime particle.\n\
  \t\t\tThe syntax is: ZP  set <ZP>\n\
  \t\t\twhere <ZP> is mass in GeV");
 
    _zpwidth_Parm.addDescription(
 "\t\t\tSet width of the Z-prime particle.\n\
  \t\t\tThe syntax is: ZP_width  set <zpwidth>\n\
  \t\t\twhere <zpwidth> is width in GeV");
 
    // Prints Description, Menu 14
    // ---------------------------
 
    _iprint_Parm.addDescription(
 "\t\t\tHerwig print level control.\n\
  \t\t\tThe syntax is: iprint   set <print>\n\
  \t\t\twhere :\n\
  \t\t\t     0 No printout;\n\
  \t\t\t     1 Print selected input parameters;\n\
  \t\t\t     2 1 + table of particle codes and properties;\n\
  \t\t\t     3 2 + tables of Sudakov form factors." );
 
    _maxpr_Parm.addDescription(
 "\t\t\tDump of Herwig event for maxpr events.\n\
  \t\t\tThe syntax is: maxpr   set <maxpr>\n\
  \t\t\twhere <maxpr> is no. of events to dump");
 
    _prvtx_Parm.addDescription(
 "\t\t\tWhether to include vtx info in dump of Herwig events.\n\
  \t\t\tThe syntax is: prvtx   set <prvtx>\n\
  \t\t\twhere <prvtx> is 1 (yes) or 0 (no)");
 
    _prndef_Parm.addDescription(
 "\t\t\tPrint to screen or log file in ASCII.\n\
  \t\t\tThe syntax is: prndef   set <prndef>\n\
  \t\t\twhere <prndef> is 1 (yes) or 0 (no)");
 
    _prntex_Parm.addDescription(
 "\t\t\tPrint to file in LATEX format.\n\
  \t\t\tThe syntax is: prntex   set <prntex>\n\
  \t\t\twhere <prntex> is 1 (yes) or 0 (no)");
 
    _prnweb_Parm.addDescription(
 "\t\t\tPrint to file in html format.\n\
  \t\t\tThe syntax is: prnweb   set <prnweb>\n\
  \t\t\twhere <prnweb> is 1 (yes) or 0 (no)");
 
    _nprfmt_Parm.addDescription(
 "\t\t\tHow may sig. figures in printing.\n\
  \t\t\tThe syntax is: nprfmt   set <nprfmt>\n\
  \t\t\twhere <nprfmt> is 1 (short) or 2 (long)");
 
    // Vers 6_400 Adds Description, Menu 15
    // ------------------------------------
    _optm_Parm.addDescription(
 "\t\t\tOptimise efficiency, VV or ZQQ.\n\
  \t\t\tThe syntax is: Optm   set <optm>\n\
  \t\t\twhere <optm> is 1 (yes) or 0 (no)");
 
    _iopstp_Parm.addDescription(
 "\t\t\tNo. of iterations to optimise effic, VV or ZQQ.\n\
  \t\t\tThe syntax is: Iopstp   set <iopstp>");
 
    _iopsh_Parm.addDescription(
 "\t\t\tEvents per iteration to optimise effic, VV or ZQQ.\n\
  \t\t\tThe syntax is: Iopsh   set <iopsh>");
 
    _syspin_Parm.addDescription(
 "\t\t\tTurn on spin correlations.\n\
  \t\t\tThe syntax is: Syspin   set <syspin>\n\
  \t\t\twhere <syspin> is 1 (yes) or 0 (no)");
 
    _threeb_Parm.addDescription(
 "\t\t\tTurn on 3-body decay matrix els.\n\
  \t\t\tThe syntax is: Threeb   set <threeb>\n\
  \t\t\twhere <threeb> is 1 (yes) or 0 (no)");
 
    _fourb_Parm.addDescription(
 "\t\t\tTurn on 4-body decay matrix els.\n\
  \t\t\tThe syntax is: Fourb   set <fourb>\n\
  \t\t\twhere <fourb> is 1 (yes) or 0 (no)");
 
    _lrdec_Parm.addDescription(
 "\t\t\tUnit to read spin correl or decay mat el inits.\n\
  \t\t\tThe syntax is: Lrdec   set <lrdec>");
 
    _lwdec_Parm.addDescription(
 "\t\t\tUnit to write spin correl or decay mat el inits.\n\
  \t\t\tThe syntax is: Lwdec   set <lwdec>");
 
    _prespl_Parm.addDescription(
 "\t\t\tLong. mom. or rapidity choice, see H 6_5 release note\n\
  \t\t\tThe syntax is: Prespl   set <prespl>");
 
    _lhsoft_Parm.addDescription(
 "\t\t\tLes Houches soft underlying event, 1 = yes\n\
  \t\t\tThe syntax is: Lhsoft   set <lhsoft>");
 
    _lhglsf_Parm.addDescription(
 "\t\t\tLes Houches, soft underlying event, 1 = yes\n\
  \t\t\tThe syntax is: Lhglsf   set <lhglsf>");
 
    _grvlam_Parm.addDescription(
 "\t\t\tGraviton resonance coupling, in GeV; proc 42..\n\
  \t\t\tThe syntax is: Grvlam   set <grvlam>");
 
    _emgrv_Parm.addDescription(
 "\t\t\tGraviton resonance mass, in GeV; proc 42..\n\
  \t\t\tThe syntax is: Emgrv   set <emgrv>");
 
    _gamgrv_Parm.addDescription(
 "\t\t\tGraviton resonance width, in GeV; proc 42..\n\
  \t\t\tThe syntax is: Gamgrv   set <gamgrv>");
 
    _itoprd_Parm.addDescription(
 "\t\t\tNot use PHOTOS (0 - default), or use PHOTOS (1)\n\
  \t\t\tThe syntax is: Itoprd   set <itoprd>");


     // Vers 6_505 Jimmy Adds Description, Menu 17
     // ------------------------------------------
     _ptjim_Parm.addDescription(
  "\t\t\tJimmy param   min PT of secondary scatters\n\
   \t\t\tThe syntax is: Ptjim   set <ptjim>");

     _msflag_Parm.addDescription(
  "\t\t\tJimmy on or off\n\
   \t\t\tThe syntax is: Msflag   set <msflag>");

     _jmrad73_Parm.addDescription(
  "\t\t\tJimmy param   Inverse proton radius squared\n\
   \t\t\tThe syntax is: Jmrad73   set <jmrad73>");

     _jmrad91_Parm.addDescription(
  "\t\t\tJimmy param   Inverse antiproton radius squared\n\
   \t\t\tThe syntax is: Jmrad91   set <jmrad91>");

 
    // Attach all menus and parameters to the appropriate pages
    // ---------------------------------------------------------
    commands( )->append( &_process);
    commands( )->append( &_maxer_Parm);
    commands( )->append( &_randomseed1_Parm);
    commands( )->append( &_randomseed2_Parm);
    commands( )->append( &_lambdaQCD_Parm);
 
    commands( )->append( &_01Menu);
    commands( )->append( &_02Menu);
    commands( )->append( &_03Menu);
    commands( )->append( &_04Menu);  
    commands( )->append( &_05Menu);
    commands( )->append( &_06Menu);
    commands( )->append( &_07Menu);  
    commands( )->append( &_08Menu);
    commands( )->append( &_09Menu);
    commands( )->append( &_10Menu);
    commands( )->append( &_11Menu);
    commands( )->append( &_12Menu);
    commands( )->append( &_13Menu);
    commands( )->append( &_14Menu);                 
    commands( )->append( &_15Menu);                 
    commands( )->append( &_16Menu);                 
    commands( )->append( &_17Menu);                 
 
    //   commands( )->append( &_decayTable_Parm);
    //   commands( )->append( &_scaleChoiceParm);
 
    _01Menu.commands( )->append( &_Beamtype1_Parm);
    _01Menu.commands( )->append( &_Energy1_Parm);
    _01Menu.commands( )->append( &_Beamtype2_Parm);
    _01Menu.commands( )->append( &_Energy2_Parm);  
    _01Menu.commands( )->append( &_pipsmr_Parm);
    _01Menu.commands( )->append( &_vipwidx_Parm);
    _01Menu.commands( )->append( &_vipwidy_Parm);
    _01Menu.commands( )->append( &_vipwidz_Parm);
 
    _02Menu.commands( )->append( &_sudord_Parm);
 
    _03Menu.commands( )->append( &_autpdf);
    _03Menu.commands( )->append( &_modpdf);
    _03Menu.commands( )->append( &_nstru_Parm);
 
 //cjl   _04Menu.commands( )->append ( &_rapiditiesm_Parm);
    _04Menu.commands( )->append ( &_rapiditiesM_Parm);
    _04Menu.commands( )->append ( &_ptm_Parm);
    _04Menu.commands( )->append ( &_ptM_Parm);
    _04Menu.commands( )->append ( &_invarmassm_Parm);
    _04Menu.commands( )->append ( &_invarmassM_Parm);
    _04Menu.commands( )->append ( &_thrust_Parm);
    _04Menu.commands( )->append ( &_ptpow_Parm);
    _04Menu.commands( )->append ( &_empow_Parm);
    _04Menu.commands( )->append ( &_q2pow_Parm);
    _04Menu.commands( )->append ( &_qlim_Parm); 
    _04Menu.commands( )->append ( &_q2dilsm_Parm); 
    _04Menu.commands( )->append ( &_q2dilsM_Parm);
    _04Menu.commands( )->append ( &_bgshat_Parm);
    
    _05Menu.commands( )->append ( &_azsoft_Parm);
    _05Menu.commands( )->append ( &_azspin_Parm);
    _05Menu.commands( )->append ( &_qspac_Parm);
    _05Menu.commands( )->append ( &_ispac_Parm);
    _05Menu.commands( )->append ( &_nospac_Parm);
    _05Menu.commands( )->append ( &_vqcut_Parm);
    _05Menu.commands( )->append ( &_vgcut_Parm);
    _05Menu.commands( )->append ( &_vpcut_Parm);
    _05Menu.commands( )->append ( &_softme_Parm);
    _05Menu.commands( )->append ( &_intrinsicpt_Parm);
    _05Menu.commands( )->append ( &_clreco_Parm);
    _05Menu.commands( )->append ( &_preco_Parm);
 
    _06Menu.commands( )->append (& _clmax_Parm);
    _06Menu.commands( )->append (& _clpow_Parm);
    _06Menu.commands( )->append (& _psplt1_Parm);
    _06Menu.commands( )->append (& _psplt2_Parm);
    _06Menu.commands( )->append (& _cldir1_Parm);
    _06Menu.commands( )->append (& _cldir2_Parm);
    _06Menu.commands( )->append (& _clsmr1_Parm);
    _06Menu.commands( )->append (& _clsmr2_Parm);
    _06Menu.commands( )->append (& _qdiqk_Parm);
    _06Menu.commands( )->append (& _pdiqk_Parm);
    _06Menu.commands( )->append (& _sngwt_Parm);
    _06Menu.commands( )->append (& _decwt_Parm);
    _06Menu.commands( )->append (& _pwt1_Parm);
    _06Menu.commands( )->append (& _pwt2_Parm);
    _06Menu.commands( )->append (& _pwt3_Parm);
    _06Menu.commands( )->append (& _pwt4_Parm);
    _06Menu.commands( )->append (& _pwt5_Parm);
    _06Menu.commands( )->append (& _pwt6_Parm);
    _06Menu.commands( )->append (& _pwt7_Parm);
 
    _07Menu.commands( )->append (& _prsof_Parm);
    _07Menu.commands( )->append (& _ensof_Parm);
    _07Menu.commands( )->append (& _pmbn1_Parm);
    _07Menu.commands( )->append (& _pmbn2_Parm);
    _07Menu.commands( )->append (& _pmbn3_Parm);
    _07Menu.commands( )->append (& _pmbk1_Parm);
    _07Menu.commands( )->append (& _pmbk2_Parm);
    _07Menu.commands( )->append (& _pmbm1_Parm);
    _07Menu.commands( )->append (& _pmbm2_Parm);
    _07Menu.commands( )->append (& _pmbp1_Parm);
    _07Menu.commands( )->append (& _pmbp2_Parm);
    _07Menu.commands( )->append (& _pmbp3_Parm);
 
    _08Menu.commands( )->append (& _pizstable_Parm);
    _08Menu.commands( )->append (& _topltim_Parm);
    _08Menu.commands( )->append (& _pltcut_Parm);
    _08Menu.commands( )->append (& _modbos1_Parm);
    _08Menu.commands( )->append (& _modbos2_Parm);
 
    _09Menu.commands( )->append (& _eventweight_Parm);
    _09Menu.commands( )->append (& _maxweight_Parm);
    _09Menu.commands( )->append (& _negwts_Parm);
    _09Menu.commands( )->append (& _effmin_Parm);
    _09Menu.commands( )->append (& _noshower_Parm);
    _09Menu.commands( )->append (& _nohadroniz_Parm);
    _09Menu.commands( )->append (& _tlout_Parm);
  
    _10Menu.commands( )->append (& _colors_Parm);
    _10Menu.commands( )->append (& _flavors_Parm);
    _10Menu.commands( )->append (& _weiangle_Parm);
    _10Menu.commands( )->append (& _cabangle_Parm);
    _10Menu.commands( )->append (& _wwidth_Parm);
    _10Menu.commands( )->append (& _zwidth_Parm);
 
    _11Menu.commands( )->append (& _down_Parm);
    _11Menu.commands( )->append (& _up_Parm);
    _11Menu.commands( )->append (& _strange_Parm);
    _11Menu.commands( )->append (& _charm_Parm);
    _11Menu.commands( )->append (& _bottom_Parm);
    _11Menu.commands( )->append (& _top_Parm);
    _11Menu.commands( )->append (& _gluonmass_Parm);
    _11Menu.commands( )->append (& _W_Parm);
    _11Menu.commands( )->append (& _Z0_Parm); 
    _11Menu.commands( )->append (& _higgs_Parm);
    _11Menu.commands( )->append (& _gammax_Parm);
 
    _12Menu.commands( )->append( &_read_Filesusy);
    _12Menu.commands( )->append (& _rparty_Parm);

    _13Menu.commands()->append(& _zprime_Parm);
    _13Menu.commands()->append(& _ZP_Parm);
    _13Menu.commands()->append(& _zpwidth_Parm);
 
    _14Menu.commands()->append(& _iprint_Parm);
    _14Menu.commands()->append(& _maxpr_Parm);
    _14Menu.commands()->append(& _prvtx_Parm);
    _14Menu.commands()->append(& _prndef_Parm);
    _14Menu.commands()->append(& _prntex_Parm);
    _14Menu.commands()->append(& _prnweb_Parm);
    _14Menu.commands()->append(& _nprfmt_Parm);

    _15Menu.commands()->append(& _optm_Parm);
    _15Menu.commands()->append(& _iopstp_Parm);
    _15Menu.commands()->append(& _iopsh_Parm);
    _15Menu.commands()->append(& _syspin_Parm);
    _15Menu.commands()->append(& _threeb_Parm);
    _15Menu.commands()->append(& _fourb_Parm);
    _15Menu.commands()->append(& _lrdec_Parm);
    _15Menu.commands()->append(& _lwdec_Parm);

    _16Menu.commands()->append(& _prespl_Parm);
    _16Menu.commands()->append(& _lhsoft_Parm);
    _16Menu.commands()->append(& _lhglsf_Parm);
    _16Menu.commands()->append(& _grvlam_Parm);
    _16Menu.commands()->append(& _emgrv_Parm);
    _16Menu.commands()->append(& _gamgrv_Parm);
    _16Menu.commands()->append(& _itoprd_Parm);

    _17Menu.commands()->append(& _ptjim_Parm);
    _17Menu.commands()->append(& _msflag_Parm);
    _17Menu.commands()->append(& _jmrad73_Parm);
    _17Menu.commands()->append(& _jmrad91_Parm);
}

void HerwigMenu::Absparm2Her() {

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
herwig->Hwpram() ->nstru=_nstru_Parm.value();
herwig->Hwproc() ->pbeam1=_Energy1_Parm.value();
herwig->Hwproc() ->pbeam2=_Energy2_Parm.value();
herwig->Hwproc() ->iproc=_process.value();
herwig->Hwevnt() ->maxer=_maxer_Parm.value();
herwig->Hwevnt() ->nrn[0]=_randomseed1_Parm.value();
herwig->Hwevnt() ->nrn[1]=_randomseed2_Parm.value();
herwig->Hwpram() -> qcdlam=_lambdaQCD_Parm.value();
herwig->Hwusud() ->sudord=_sudord_Parm.value();
herwig->Hwdist() ->pipsmr=_pipsmr_Parm.value();
herwig->Hw6202() ->vipwid[0]=_vipwidx_Parm.value();
herwig->Hw6202() ->vipwid[1]=_vipwidy_Parm.value();
herwig->Hw6202() ->vipwid[2]=_vipwidz_Parm.value();

//cjl re ymin
//herwig->Hwhard() ->yjmin=_rapiditiesm_Parm.value();
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

herwig->Hwpram() -> azsoft=_azsoft_Parm.value();
herwig->Hwpram() -> azspin=_azspin_Parm.value();
herwig->Hwpram() -> qspac=_qspac_Parm.value();
herwig->Hwpram() -> ispac=_ispac_Parm.value();
herwig->Hwpram() -> nospac=_nospac_Parm.value();
herwig->Hwpram() -> vqcut=_vqcut_Parm.value();
herwig->Hwpram() -> vgcut=_vgcut_Parm.value();
herwig->Hwpram() -> vpcut=_vpcut_Parm.value();
herwig->Hwpram() -> softme=_softme_Parm.value();
herwig->Hwpram() -> ptrms=_intrinsicpt_Parm.value();
herwig->Hwuclu() -> clreco=_clreco_Parm.value();
herwig->Hwuclu() -> preco=_preco_Parm.value();

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
herwig->Hwuwts() -> sngwt=_sngwt_Parm.value();
herwig->Hwuwts() -> decwt=_decwt_Parm.value();
herwig->Hwuwts() -> pwt[0]=_pwt1_Parm.value();
herwig->Hwuwts() -> pwt[1]=_pwt2_Parm.value();
herwig->Hwuwts() -> pwt[2]=_pwt3_Parm.value();
herwig->Hwuwts() -> pwt[3]=_pwt4_Parm.value();
herwig->Hwuwts() -> pwt[4]=_pwt5_Parm.value();
herwig->Hwuwts() -> pwt[5]=_pwt6_Parm.value();
herwig->Hwuwts() -> pwt[6]=_pwt7_Parm.value();

herwig->Hwpram() -> prsof=_prsof_Parm.value();
herwig->Hwpram() -> ensof=_ensof_Parm.value();
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

herwig->Hwupdt() -> rstab[21]=_pizstable_Parm.value();
herwig->Hwprop() -> rltim[6]=_topltim_Parm.value();
herwig->Hwprop() -> rltim[12]=_topltim_Parm.value();
herwig->Hwdist() -> pltcut=_pltcut_Parm.value();
herwig->Hwbosc() -> modbos[0]=_modbos1_Parm.value();
herwig->Hwbosc() -> modbos[1]=_modbos2_Parm.value();

herwig->Hwevnt() -> nowgt=_eventweight_Parm.value();
herwig->Hwevnt() -> wgtmax=_maxweight_Parm.value();
herwig->Hw6203() -> negwts=_negwts_Parm.value();
herwig->Hwpram() -> effmin=_effmin_Parm.value();
herwig->Hwevnt() -> tlout=_tlout_Parm.value();
herwig->Hwpram() -> ncolo=_colors_Parm.value();
herwig->Hwpram() -> nflav=_flavors_Parm.value(); 
herwig->Hwpram() -> swein=_weiangle_Parm.value();
herwig->Hwpram() -> scabi=_cabangle_Parm.value();
herwig->Hwpram() -> gamw=_wwidth_Parm.value();
herwig->Hwpram() -> gamz=_zwidth_Parm.value();

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
herwig->Hwbosc() ->gammax=_gammax_Parm.value();

herwig->Hwpram() -> zprime = _zprime_Parm.value();
herwig->Hwprop() -> rmass[202] = _ZP_Parm.value();
herwig->Hwpram() -> gamzp = _zpwidth_Parm.value();

herwig->Hwrpar() -> rparty = _rparty_Parm.value();

herwig->Hwpram() -> iprint = _iprint_Parm.value();
herwig->Hwevnt() -> maxpr = _maxpr_Parm.value();
herwig->Hwpram() -> prvtx = _prvtx_Parm.value();
herwig->Hwpram() -> prndef = _prndef_Parm.value();
herwig->Hwpram() -> prntex = _prntex_Parm.value();
herwig->Hwpram() -> prnweb = _prnweb_Parm.value();
herwig->Hwpram() -> nprfmt = _nprfmt_Parm.value();

herwig->Hw6300() -> optm = _optm_Parm.value();
herwig->Hw6300() -> iopstp = _iopstp_Parm.value();
herwig->Hw6300() -> iopsh = _iopsh_Parm.value();
herwig->Hwdspn() -> syspin = _syspin_Parm.value();
herwig->Hwdspn() -> threeb = _threeb_Parm.value();
herwig->Hwdspn() -> fourb = _fourb_Parm.value();
herwig->Hwdspn() -> lrdec = _lrdec_Parm.value();
herwig->Hwdspn() -> lwdec = _lwdec_Parm.value();

herwig->Hw6500() -> prespl = _prespl_Parm.value();
herwig->Hwgupr() -> lhsoft = _lhsoft_Parm.value();
herwig->Hwgupr() -> lhglsf = _lhglsf_Parm.value();
herwig->Hwgrav() -> grvlam = _grvlam_Parm.value();
herwig->Hwgrav() -> emgrv = _emgrv_Parm.value();
herwig->Hwgrav() -> gamgrv = _gamgrv_Parm.value();
herwig->Hw6504() -> itoprd = _itoprd_Parm.value();

herwig->Jmparm() -> ptjim = _ptjim_Parm.value();
herwig->Jmparm() -> msflag = _msflag_Parm.value();
herwig->Jmparm() -> jmrad[72] = _jmrad73_Parm.value();
herwig->Jmparm() -> jmrad[90] = _jmrad91_Parm.value();
}

//____________________________________________________________________________
void HerwigMenu::Her2Absparm() {
  // convert Herwig Common Blocks to the AbsParms of the Herwig Module

  Herwig* herwig = Herwig::Instance();

  string mystring;
  mystring.assign(herwig->Hwbmch()->part1,8);
  _Beamtype1_Parm.set(mystring);
  mystring.assign(herwig->Hwbmch()->part2,8);
  _Beamtype2_Parm.set(mystring);
  mystring.assign(herwig->Hwprch()->autpdf[0],20);
  _autpdf.set(mystring);
  _modpdf.set(herwig->Hwpram()->modpdf[0]);
  _nstru_Parm.set(herwig->Hwpram()->nstru);
  _Energy1_Parm.set(herwig->Hwproc() ->pbeam1);
  _Energy2_Parm.set(herwig->Hwproc() ->pbeam2);
  _process.set(herwig->Hwproc()->iproc);
  _maxer_Parm.set(herwig->Hwevnt() ->maxer);
  _randomseed1_Parm.set(herwig->Hwevnt() ->nrn[0]);
  _randomseed2_Parm.set(herwig->Hwevnt() ->nrn[1]);
  _lambdaQCD_Parm.set(herwig->Hwpram() -> qcdlam);
  _sudord_Parm.set(herwig->Hwusud() ->sudord);
  _pipsmr_Parm.set(herwig->Hwdist()->pipsmr);
  _vipwidx_Parm.set(herwig->Hw6202()->vipwid[0]);
  _vipwidy_Parm.set(herwig->Hw6202()->vipwid[1]);
  _vipwidz_Parm.set(herwig->Hw6202()->vipwid[2]);

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
  _softme_Parm.set(herwig->Hwpram() -> softme);
  _intrinsicpt_Parm.set(herwig->Hwpram() -> ptrms);
  _clreco_Parm.set(herwig->Hwuclu() -> clreco);
  _preco_Parm.set(herwig->Hwuclu() -> preco);

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
  _sngwt_Parm.set(herwig->Hwuwts() ->sngwt);
  _decwt_Parm.set(herwig->Hwuwts() ->decwt);
  _pwt1_Parm.set(herwig->Hwuwts() ->pwt[0]);
  _pwt2_Parm.set(herwig->Hwuwts() ->pwt[1]);
  _pwt3_Parm.set(herwig->Hwuwts() ->pwt[2]);
  _pwt4_Parm.set(herwig->Hwuwts() ->pwt[3]);
  _pwt5_Parm.set(herwig->Hwuwts() ->pwt[4]);
  _pwt6_Parm.set(herwig->Hwuwts() ->pwt[5]);
  _pwt7_Parm.set(herwig->Hwuwts() ->pwt[6]);

  _prsof_Parm.set(herwig->Hwpram() -> prsof);
  _ensof_Parm.set(herwig->Hwpram() -> ensof);
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

  _pizstable_Parm.set(herwig->Hwupdt() -> rstab[21]);
  _topltim_Parm.set(herwig->Hwprop() -> rltim[6]);
  _pltcut_Parm.set(herwig->Hwdist() -> pltcut);
  _modbos1_Parm.set(herwig->Hwbosc()->modbos[0]);
  _modbos2_Parm.set(herwig->Hwbosc()->modbos[1]);

  _eventweight_Parm.set(herwig->Hwevnt() ->nowgt);
  _maxweight_Parm.set(herwig->Hwevnt() ->wgtmax);
  _negwts_Parm.set(herwig->Hw6203() ->negwts);
  _effmin_Parm.set(herwig->Hwpram() ->effmin);
  _tlout_Parm.set(herwig->Hwevnt() ->tlout);

  _colors_Parm.set(herwig->Hwpram() -> ncolo);
  _flavors_Parm.set(herwig->Hwpram() -> nflav); 
  _weiangle_Parm.set(herwig->Hwpram() -> swein);
  _cabangle_Parm.set(herwig->Hwpram() -> scabi);
  _wwidth_Parm.set(herwig->Hwpram() -> gamw);
  _zwidth_Parm.set(herwig->Hwpram() -> gamz);

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
  _gammax_Parm.set(herwig->Hwbosc()->gammax);
  
  _zprime_Parm.set(herwig->Hwpram() -> zprime);
  _ZP_Parm.set(herwig->Hwprop() -> rmass[202]);
  _zpwidth_Parm.set(herwig->Hwpram() -> gamzp);

  _rparty_Parm.set(herwig->Hwrpar()->rparty);

  _iprint_Parm.set(herwig->Hwpram() -> iprint);
  _maxpr_Parm.set(herwig->Hwevnt() -> maxpr);
  _prvtx_Parm.set(herwig->Hwpram() -> prvtx);
  _prndef_Parm.set(herwig->Hwpram() -> prndef);
  _prntex_Parm.set(herwig->Hwpram() -> prntex);
  _prnweb_Parm.set(herwig->Hwpram() -> prnweb);
  _nprfmt_Parm.set(herwig->Hwpram() -> nprfmt);

  _optm_Parm.set(herwig->Hw6300() -> optm);
  _iopstp_Parm.set(herwig->Hw6300() -> iopstp);
  _iopsh_Parm.set(herwig->Hw6300() -> iopsh);
  _syspin_Parm.set(herwig->Hwdspn() -> syspin);
  _threeb_Parm.set(herwig->Hwdspn() -> threeb);
  _fourb_Parm.set(herwig->Hwdspn() -> fourb);
  _lrdec_Parm.set(herwig->Hwdspn() -> lrdec);
  _lwdec_Parm.set(herwig->Hwdspn() -> lwdec);

  _prespl_Parm.set(herwig->Hw6500() -> prespl);
  _lhsoft_Parm.set(herwig->Hwgupr() -> lhsoft);
  _lhglsf_Parm.set(herwig->Hwgupr() -> lhglsf);
  _grvlam_Parm.set(herwig->Hwgrav() -> grvlam);
  _emgrv_Parm.set(herwig->Hwgrav() -> emgrv);
  _gamgrv_Parm.set(herwig->Hwgrav() -> gamgrv);
  _itoprd_Parm.set(herwig->Hw6504() -> itoprd);

  _ptjim_Parm.set(herwig->Jmparm() -> ptjim);
  _msflag_Parm.set(herwig->Jmparm() -> msflag);
  _jmrad73_Parm.set(herwig->Jmparm() -> jmrad[72]);
  _jmrad91_Parm.set(herwig->Jmparm() -> jmrad[90]);

  // Following two are used only internally to this routine:
  //_noshower_Parm.set(0);// correspondence in common block not finded
  //_nohadroniz_Parm.set(0);// correspondence in common block not finded


}




