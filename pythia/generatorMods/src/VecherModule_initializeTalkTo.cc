//------------------------------------------------------------------------
//	Software developed for the CDFII Detector
//
// Author List:
//	George Velev
// Modifications:
//
//  revision history:
//  -----------------
//  July 18, 2001, G.Velev,  created
//                           copy all the talk-to parameters from herwig to 
//                           support herprt talk
//                            add a control over sudakov read/write
//------------------------------------------------------------------------
#include "generatorMods/VecherModule.hh"

void VecherModule::_initializeTalkTo() {

  // Init the menus

 _lepton_type.addDescription("\t\t\tLeptonType\n\
     \t\t\t Specify the lepton type (1-electron,2-muon,3-tau) [1]   \n\
     \t\t\t Syntax: leptonType set <number>");

  commands( )->append(&_lepton_type);
  commands( )->append( &_summary );

  // herwig part 
    // Initialize all the menus
  // ------------------------
     _01Menu.initialize ("Beam",this);
     _01Menu.initTitle ("Beam Menu:  Beam types and momenta");

     _02Menu.initialize ("Sudakov",this);
     _02Menu.initTitle ("alpha_s order in Sudakov form factors"); 

     _03Menu.initialize ("Struc_Function",this);
     _03Menu.initTitle ("Select structure function set");

     _04Menu.initialize ("Hards",this);
     _04Menu.initTitle ("Hard scattering process params menu"); 

     _05Menu.initialize ("Shower",this);
     _05Menu.initTitle ("Evokes shower parameters menu"); 

     _06Menu.initialize ("Hadronization",this);
     _06Menu.initTitle ("Evokes hadronization parameters menu");  

     _07Menu.initialize ("Underl_event",this);
     _07Menu.initTitle ("Underlying soft event parameters menu"); 

     _08Menu.initialize ("Decays",this);
     _08Menu.initTitle ("Controls Particle Decays");

     _09Menu.initialize ("Generation",this);
     _09Menu.initTitle ("Evokes  generation parameter menu");

     _10Menu.initialize ("Sm_par",this);
     _10Menu.initTitle("Evokes standard model parameters menu");

     _11Menu.initialize ("Masses",this);
     _11Menu.initTitle ("Change particle masses"); 

     _12Menu.initialize ("Read_susy",this);
     _12Menu.initTitle ("Read susy file");
   
     _13Menu.initialize ("Zprime",this);
     _13Menu.initTitle ("Control Z-prime");
   
     _14Menu.initialize ("Prints",this);
     _14Menu.initTitle ("Control printing");

     _15Menu.initialize ("Read_write",this);
     _15Menu.initTitle ("Read or write Sudakov tables");

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
"\t\t\tAuthors of PDF function (MRS,CTEQ,etc) as string variable");

   _modpdf.addDescription(
"\t\t\tPDF number for PDFLIB as integer");

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

   _intrinsicpt_Parm.addDescription(
"\t\t\tSet intrinsic pt of incoming hadrons.\n\
 \t\t\tThe syntax is: Pt_incoming_hadrons  set <intrinsicpt>");

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

   _decwt_Parm.addDescription(
"\t\t\tSet weights for decuplet baryons for cluster decays.\n\
 \t\t\tThe syntax is: Decwt set <decwt>");

   _pwt_Parm.addDescription(
"\t\t\ta priori wt for d quark, default for all 6 q's is 1.\n\
 \t\t\tThe syntax is: Q_dq_weight set <ptw>");
 //"\t\t\t----> NEEDS WORK ");

   // Underlying Event Descriptions, Menu 07
   // --------------------------------------
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

   _ensof_Parm.addDescription(
"\t\t\tSet multiplicity enhancement for underlying soft event.\n\
  \t\t\tThe syntax is: Ensof set <ensof>");

   // Decays Descriptions, Menu 08
   // ----------------------------
   _pizstable_Parm.addDescription(
"\t\t\tSet whether pizeros are to be stable.\n\
  \t\t\tThe syntax is: Pizstable set <pizstable>\n\
 \t\t\twhere <pizstable> is 1(true) or 0(false) " );
   _pltcut_Parm.addDescription(
"\t\t\tSet lifetime cut in secs above which particles are stable.\n\
  \t\t\tThe syntax is: Pltcut set <pltcut> " );

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
 \t\t\tThe syntax is: tlout   set <tlout>\n\
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

   _hwidth_Parm.addDescription(
"\t\t\tSet Width of Higgs boson.\n\
 \t\t\tThe syntax is: Higgs_width set <hwidth>\n\
 \t\t\twhere <hwidth> is in GEV" );

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
 \t\t\tThe syntax is: higgs   set <higgs>\n\
 \t\t\twhere <higgs> is in GEV" );

   // Read_susy Descriptions, Menu 12
   // --------------------------------------

   _read_Filesusy.addDescription(
"\t\t\tSpecify the name of the file used to read the\n\
 \t\t\t\tSUSY particle properties.\n\
 \t\t\tThe format of the command is: File_Readsusy set <filename>" );

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

   // Prints Description, Menu 15
   // ---------------------------

   _read_Sudakov_Table.addDescription(
"\t\t\tFlag to switch on/off  reading of the Sudakov table.\n\
 \t\t\tThe name is defined with the environment variable HERWIG_SUD.\n\
  \t\t\tThe syntax is: readTable  set <flag>\n\
\t\t\twhere <flag> is 0 (off) or 1 (on)");

   _write_Sudakov_Table.addDescription(
"\t\t\tFlag to switch on/off writing of the Sudakov table.\n\
 \t\t\tThe name is defined with the environment variable HERWIG_SUD_OUT.\n\
  \t\t\tThe syntax is: writeTable  set <flag>\n\
\t\t\twhere <flag> is 0 (off) or 1 (on)");

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

   //   commands( )->append( &_decayTable_Parm);
   //   commands( )->append( &_scaleChoiceParm);

   _01Menu.commands( )->append( &_Beamtype1_Parm);
   _01Menu.commands( )->append( &_Energy1_Parm);
   _01Menu.commands( )->append( &_Beamtype2_Parm);
   _01Menu.commands( )->append( &_Energy2_Parm);  

   _02Menu.commands( )->append( &_sudord_Parm);

   _03Menu.commands( )->append( &_autpdf);
   _03Menu.commands( )->append( &_modpdf);

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
   _05Menu.commands( )->append ( &_intrinsicpt_Parm);

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
   _06Menu.commands( )->append (& _decwt_Parm);
   _06Menu.commands( )->append (& _pwt_Parm);

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
   _07Menu.commands( )->append (& _ensof_Parm);

   _08Menu.commands( )->append (& _pizstable_Parm);
   _08Menu.commands( )->append (& _pltcut_Parm);

   _09Menu.commands( )->append (& _eventweight_Parm);
   _09Menu.commands( )->append (& _maxweight_Parm);
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
   _10Menu.commands( )->append (& _hwidth_Parm);

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

   _12Menu.commands( )->append( &_read_Filesusy);

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

   _15Menu.commands()->append(&  _read_Sudakov_Table);
   _15Menu.commands()->append(&  _write_Sudakov_Table);
 }
