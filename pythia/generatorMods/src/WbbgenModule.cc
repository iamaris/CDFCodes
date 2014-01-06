//     generatorMods/src/WbbgenModule.cc
//***** Adapted from HerwigModule.cc   jlys  04-Jun-2001 ******
//     update wbbgen July-2001,   jlys
//     update to Herwig v6202 Aug 2001  jlys
//--------------------------------------------------------------------------
// File and Version Information:
//      WbbgenModule.cc
//
// Description (for HerwigModule, from which this is adapted):
//     This module is the interface between the Herwig Monte Carlo generator
//     and the CDF Run 2 software.   HerwigModule inherits from the base class
//     APPGenInputModule which defines the default behaviour for an AC++ 
//     generator input module.  A good portion of the code here moves
//     parameters between the AC++ abstract parameter class and the
//     Herwig common blocks that control the generator.
//     The Herwig output is transferred to the Hepevt common block using
//     the standard Fermilab library STDHEP.  Then, CDF code moves this
//     common block into the HEPG Trybos bank in the Abstract Event.
//
//
// Author List:
//     Marjorie Shapiro and Elena Bucciol               Original Author
// Revision History:
//     00001  Aug 1998  Original Version
//     1999, 2000, j lys and others
//     2000, May, j lys, for Herwig v 6.1
//     2001, June, j lys, for Wbbgen
//     Aug 29 2001 lena: clean up - remove gEvent, other
//     Aug 30 2001 lena: moving to AbsGenModule
//------------------------------------------------------------------------
//-----------------------
// This Class's Header --
//-----------------------
#include "generatorMods/WbbgenModule.hh"
#include "BankTools/ToFromYbos.hh"
//-------------
// C Headers --
//-------------
#include <time.h>
//---------------
// C++ Headers --
//--------------- 
#include <iostream>

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "herwig_i/Herwig.hh"
#include "wbbgen_i/wbbgen.hh"
#include "evt/Event.hh"
//#include "ParticleDB/hepevt.hh"
#include "stdhep_i/CdfHepevt.hh" 
#include "r_n/CdfRn.hh"
const long WbbgenModule::_defaultRandomSeed1 = 88351;
const long WbbgenModule::_defaultRandomSeed2 = 973546;

//void InitHerwigCommonBlocks();
void InitWbbgenCommonBlocks();

//---------------------------------------------
// Fortran Subroutines called by this Module --
//---------------------------------------------

extern "C" {
  //  void stdhepctb_();
				// In wbbgen_i area:
  void wherwiginterface_();
  void wbbusr_();
  void wbbusrend_();
  void stread_(double *pbeam1,int *ih1,
	       double *pbeam2,int *ih2,double *mq,int *njets);
  void loadev_(int *ist,double *avgwgt);
				//  In Herwig 
  void hwwarn_(char *,int *,int *);
  void hwuinc_();
  //  void hwusta(int *tstab);
  void hwabeg_();
  void hwaini_();
  void hwuine_();
  void hwbgen_();
  void hwcfor_();
  void hwcdec_();
  void hwdhad_();
  void hwdhvy_();
  void hwmevt_();
  void hwufne_(); 
  void whwanal_(int *ihwcod);
  void hwafin_(double *totwgt,int *nev);
  void hwaend_();
				// In Stdhep
  void hwghep_(int *mconv);
}

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------
static int init=0;
//cjl 15-jun-01
int nevgoodev = 0;
double totwgt = 0;

const char* WbbgenModule::genId =  "Wbbgen";


//______constructor_________________________________________________________

WbbgenModule::WbbgenModule() : 
  AbsGenModule( WbbgenModule::genId,"AC++ module for WBBGEN" ),
  //  _process("Process",this,1705), 

  _maxer_Parm("Maxer",this,100),
  //  _randomseed1_Parm("Randseed1",this,0),
  //  _randomseed2_Parm("Randseed2",this,0),
  _lambdaQCD_Parm( "Lambda_QCD", this, 0.18),
   
				// commands of Menu 01: The Beam menu
				// ----------------------------------
  //  _Beamtype1_Parm("Beam1", this, "12345678"),
  //  _Beamtype2_Parm("Beam2", this , "12345678"),
  //  _Energy1_Parm("Pbeam1", this,980.),
  //  _Energy2_Parm("Pbeam2", this,980.),
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
  _qlim_Parm("Qlim",this,0),
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
  _intrinsicpt_Parm("Pt_incoming_hadrons",this,0),
  
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
  _pltcut_Parm("Pltcut",this,0.),
  _modbos1_Parm("Modbos1",this,0),
  _modbos2_Parm("Modbos2",this,0),

				// commands of Menu 09: Generation Menu
				// ------------------------------------
  _eventweight_Parm("No_weights",this,1),
  _maxweight_Parm("Maxwt",this,0),
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

				// commands of Menu 15: Wbbuser Menu 
				// --------------------------------
  _iih1_Parm("ibeam1",this,3),
  _iih2_Parm("ibeam2",this,1),
  _ebeam_Parm("ebeam",this,980.),
  _mq_Parm("bmass",this,4.75),
  _njets_Parm("njets",this,3),
  _ndns_Parm("ndns",this,61),
  _ptjmin_Parm("ptjmin",this,20.),
  _etajmax_Parm("etajmax",this,2.5),
  _drjmin_Parm("drjmin",this,0.4),
  _ptbmin_Parm("ptbmin",this,20.),
  _etabmax_Parm("etabmax",this,2.5),
  _drbmin_Parm("drbmin",this,0.4),
  _ptlmin_Parm("ptlmin",this,20.),
  _etalmax_Parm("etalmax",this,2.5),
  _drlmin_Parm("drlmin",this,0.0),
  _ptnmin_Parm("ptnmin",this,0.0),
  _iqopt_Parm("iqopt",this,1),
  _qfac_Parm("qfac",this,1.0),
  _nopt_Parm("nopt",this,50000),
  _niter_Parm("niter",this,1),
  _maxev_Parm("maxev",this,10000),
  _seed1_Parm("seed1",this,12345),
  _seed2_Parm("seed2",this,67890),
  _jgridr_Parm("gridr",this,0),
  _jgridw_Parm("gridw",this,22),
  _iilepton_Parm("iilepton",this,11),
  _wbbfilen("wbbfile",this,"wbbfile"),
  _randomSeed1("RandomSeed1",this,WbbgenModule::_defaultRandomSeed1),
  _randomSeed2("RandomSeed2",this,WbbgenModule::_defaultRandomSeed2)
{
  // wbbgen menu's and commands
  InitializeTalkTo();
  // Initialize the relevant submenu
  _randomNumberMenu.initialize("RandomNumberMenu",this);
  _randomNumberMenu.initTitle("Fake event random number menu");
  commands()->append(&_randomNumberMenu);

  // Add the commands to the menu
  _randomNumberMenu.commands()->append(&_randomSeed1);
  _randomNumberMenu.commands()->append(&_randomSeed2);

  ostringstream tmpSstream1;
  ostringstream tmpSstream2;

  // Describe them
  tmpSstream1 << "      \t\t\tSeed #1 for the random number generator"
              << "\n\t\t\t(default " << _randomSeed1.value() << ").";
  _randomSeed1.addDescription(tmpSstream1.str());
  tmpSstream2 << "      \t\t\tSeed #2 for the random number generator"
              << "\n\t\t\t(default " << _randomSeed2.value() << ").";  
  _randomSeed2.addDescription(tmpSstream2.str());


  // Herwig Common blocks are initialized in HerwigModule
  // Call trivial Fortran routines
  wherwiginterface_();
  // Transfer numbers from Herwig Common Blocks to Absparms
  Her2Absparm();
  // Map Wbbgen Common blocks to global C structures
  InitWbbgenCommonBlocks();
  // Put contructor initialized  values of Absparms into Wbbgen Common
  Absparm2Wbb();
}


//--------------
// Destructor --
//--------------

WbbgenModule::~WbbgenModule(){ }

AppResult WbbgenModule::genBeginJob( ) {

  CdfRn* rn = CdfRn::Instance();
  if ( !rn->isReadingFromFile() ) {
    rn->SetEngineSeeds(_randomSeed1.value(), _randomSeed2.value(),"WBBGEN");
  }
			 // Since we may have changed parameters via Talk-to,
			 // we need to refill Herwig Common blocks from the 
			 // AbsParms
  Absparm2Wbb();

  Absparm2Her();

  wbbusr_();
  std::cout << " Finished first wbbusr call " << std::endl;
  wbbusrend_();
  wbbusr_();
  std::cout << " Finished second wbbusr call " << std::endl;
  wbbusrend_();
  double pbeam1;
  int ih1;
  double pbeam2;
  int ih2;
  double mq;
  int njets;
  stread_(&pbeam1,&ih1,&pbeam2,&ih2,&mq,&njets);
  // Set b mass for herwig here, also the incoming momenta, and proc=0

  
  Herwig* herwig = Herwig::Instance(); 
  herwig->Hwprop() ->rmass[5]= mq; 
  herwig->Hwproc()->pbeam1 = pbeam1;
  herwig->Hwproc()->pbeam2 = pbeam2;
  herwig->Hwproc()->iproc = 0;

  // Must get IPART1 and IPART2 (char*8) of herwig.inc here
  char chprot[8] = "P      ";
  char chpbar[8] = "PBAR   ";
  if(ih1==1){
    for(int i=0; i<7; i++){
      herwig->Hwbmch()->part1[i] = chprot[i];
    }
  }
  else{
    for(int i=0; i<7; i++){
      herwig->Hwbmch()->part1[i] = chpbar[i];
    }
  }
  if(ih2==1){
    for(int i=0; i<7; i++){
      herwig->Hwbmch()->part2[i] = chprot[i];
    }
  }
  else{
    for(int i=0; i<7; i++){
      herwig->Hwbmch()->part2[i] = chpbar[i];
    }
  }


				// Now Compute Parameter Dependent Constants 
  hwuinc_();
				// Here can add calls to make particles stable 
  // hwusta_(tstab);
                                // User initialisation if wanted
  hwabeg_();
                                // Initialise elementary process
  hwaini_();

  //preset event counter
  _events = 0;

  init = 1;
  return AppResult::OK;
}

//_____________________________________________________________________________
AppResult WbbgenModule::genEndJob() {
                               // termination (prints wgts if used)
                               // User terminal calculations if wanted
  CdfHepevt* hepevt = CdfHepevt::Instance();
  hepevt->HepevtPtr()->NEVHEP = _events;

  //cjl next line, testing
  hwafin_(&totwgt,&nevgoodev);
  hwaend_();
  return AppResult::OK;
}


//_____________________________________________________________________________
AppResult WbbgenModule::genBeginRun( AbsEvent* run ) {
  return AppResult::OK;
}

//_____________________________________________________________________________
AppResult WbbgenModule::genEndRun( AbsEvent* run ) {
  return AppResult::OK;
}

//______________________________________________________________________________
int WbbgenModule::callGenerator(AbsEvent* event) {
  // generates single event with WBBGEN + HERWIG

  const int mxerr = 5000;
  int nterr = 0;
  bool goodev = false;

  if(init==0) {
    std::cout << " Herwig not properly initialized" << std::endl;
  }

  CdfHepevt* hepevt = CdfHepevt::Instance();
  while(!goodev){
    hepevt->HepevtPtr()->NEVHEP = _events;
    hwuine_();  // Initialize event
    int ist = 0;
    double avgwgt = 0;
    loadev_(&ist,&avgwgt);
    if(ist !=1) {
      if(! _noshower_Parm.value()) {
	hwbgen_();  // Generate parton cascade
	if(! _nohadroniz_Parm.value()) {
	  hwcfor_();  // Do cluster formation
	  hwcdec_();  // Do cluster decays
	  hwdhad_();  // Do unstable particle decays
	  hwdhvy_();  // Do heavy flavor decays
	  hwmevt_();  // Add soft underlying event
	}
      }
      hwufne_();  // Finish event
      // User event analysis if wanted
      int ihwcod = 1;
      whwanal_(&ihwcod);
      if(Herwig::Instance()->Hwevnt()->ierror > 99) {
	++nterr;
	if(nterr>mxerr) {
	  std::cerr << "Herwig:Too many errors on event generation" << std::endl;
	  goodev=true;
	}
      }
      else {
	if(ihwcod != 0) {
	  goodev=true;
	}
      }
    }
    else{
      //at end of event list that we are reading in, so must quit
      hwafin_(&totwgt,&nevgoodev);
      hwaend_();
      ERRLOG(ELfatal,"[WBBGEN_OUT_OF_EVENTS]")
	<< " We have come to the end of the unweighted events in wbbgen"
	<< endmsg;
      //next line never reached!
      goodev=true;
    }
    if(goodev){
      nevgoodev++;
      totwgt+=avgwgt;
    }
  }
  _events++;
  return 0;
}

//______________________________________________________________________________
void WbbgenModule::fillHepevt() {
  // Put output into STDHEP version of HEPEVT common

  int mconv = 1;
  hwghep_(&mconv);
}

//______________________________________________________________________________
void WbbgenModule::Her2Absparm() {
  // convert Herwig Common Blocks to the AbsParms of the Herwig Module

  Herwig* herwig = Herwig::Instance();

  string mystring;
  //  mystring.assign(gHwbmch->part1,8);
  //  _Beamtype1_Parm.set(mystring);
  //  mystring.assign(gHwbmch->part2,8);
  //  _Beamtype2_Parm.set(mystring);
  mystring.assign(herwig->Hwprch()->autpdf[0],20);
  _autpdf.set(mystring);
  _modpdf.set(herwig->Hwpram()->modpdf[0]);
  //  _Energy1_Parm.set(gHwproc ->pbeam1);
  //  _Energy2_Parm.set(gHwproc ->pbeam2);
  //  _process.set(gHwproc->iproc);
  _maxer_Parm.set(herwig->Hwevnt() ->maxer);
  //_randomseed1_Parm.set(herwig->Hwevnt() ->nrn[0]);
  //_randomseed2_Parm.set(herwig->Hwevnt() ->nrn[1]);
  _lambdaQCD_Parm.set(herwig->Hwpram() -> qcdlam);
  _sudord_Parm.set(herwig->Hwusud() ->sudord);
  _pipsmr_Parm.set(herwig->Hwdist()->pipsmr);
  _vipwidx_Parm.set(herwig->Hw6202()->vipwid[0]);
  _vipwidy_Parm.set(herwig->Hw6202()->vipwid[1]);
  _vipwidz_Parm.set(herwig->Hw6202()->vipwid[2]);

//cjl  _rapiditiesm_Parm.set(gHwhard ->yjmin);
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
  _sngwt_Parm.set(herwig->Hwuwts() ->sngwt);
  _decwt_Parm.set(herwig->Hwuwts() ->decwt);
  _pwt1_Parm.set(herwig->Hwuwts() ->pwt[0]);
  _pwt2_Parm.set(herwig->Hwuwts() ->pwt[1]);
  _pwt3_Parm.set(herwig->Hwuwts() ->pwt[2]);
  _pwt4_Parm.set(herwig->Hwuwts() ->pwt[3]);
  _pwt5_Parm.set(herwig->Hwuwts() ->pwt[4]);
  _pwt6_Parm.set(herwig->Hwuwts() ->pwt[5]);
  _pwt7_Parm.set(herwig->Hwuwts() ->pwt[6]);

  //cjl need - sign on pmbn3 and pmbk2
  _prsof_Parm.set(herwig->Hwpram() -> prsof);
  _ensof_Parm.set(herwig->Hwpram() -> ensof);
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
  _pltcut_Parm.set(herwig->Hwdist() -> pltcut);
  _modbos1_Parm.set(herwig->Hwbosc()->modbos[0]);
  _modbos2_Parm.set(herwig->Hwbosc()->modbos[1]);

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


  // Following two are used only internally to this routine:
  //_noshower_Parm.set(0);// correspondence in common block not finded
  //_nohadroniz_Parm.set(0);// correspondence in common block not finded


}

//
// This routine takes the AbsParms of the Herwig Module and transfers them
// to the Herwig Common Blocks

void WbbgenModule::Absparm2Her() {

  Herwig* herwig = Herwig::Instance();

  int nch;
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
  herwig->Hwpram()->modpdf[1]=_modpdf.value();
  herwig->Hwevnt() ->maxer=_maxer_Parm.value();
  //herwig->Hwevnt() ->nrn[0]=_randomseed1_Parm.value();
  //herwig->Hwevnt() ->nrn[1]=_randomseed2_Parm.value();
  herwig->Hwpram() -> qcdlam=_lambdaQCD_Parm.value();
  herwig->Hwusud() ->sudord=_sudord_Parm.value();
  herwig->Hwdist() ->pipsmr=_pipsmr_Parm.value();
  herwig->Hw6202() ->vipwid[0]=_vipwidx_Parm.value();
  herwig->Hw6202() ->vipwid[1]=_vipwidy_Parm.value();
  herwig->Hw6202() ->vipwid[2]=_vipwidz_Parm.value();

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
  herwig->Hwdist() -> pltcut=_pltcut_Parm.value();
  herwig->Hwbosc() -> modbos[0]=_modbos1_Parm.value();
  herwig->Hwbosc() -> modbos[1]=_modbos2_Parm.value();
  
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
  herwig->Hwpram()-> gamzp = _zpwidth_Parm.value();
  
  herwig->Hwrpar() -> rparty = _rparty_Parm.value();
  
  herwig->Hwpram() -> iprint = _iprint_Parm.value();
  herwig->Hwevnt() -> maxpr = _maxpr_Parm.value();
  herwig->Hwpram() -> prvtx = _prvtx_Parm.value();
  herwig->Hwpram() -> prndef = _prndef_Parm.value();
  herwig->Hwpram() -> prntex = _prntex_Parm.value();
  herwig->Hwpram() -> prnweb = _prnweb_Parm.value();
  herwig->Hwpram() -> nprfmt = _nprfmt_Parm.value();

}
//
// This routine takes the AbsParms of the Wbb part of the module and transfers
// them to the Herwig Common Blocks

void WbbgenModule::Absparm2Wbb() {

gWbbeams ->ih1=_iih1_Parm.value()-2;
gWbbeams ->ih2=_iih2_Parm.value()-2;
gWbbeams ->ebeam=_ebeam_Parm.value();
gWbpparam ->mq=_mq_Parm.value();
gWbevtpari ->njets=_njets_Parm.value();
gWbglocut ->ptjmin=_ptjmin_Parm.value();
gWbglocut ->etajmax=_etajmax_Parm.value();
gWbglocut ->drjmin=_drjmin_Parm.value();
gWbglocut ->ptbmin=_ptbmin_Parm.value();
gWbglocut ->etabmax=_etabmax_Parm.value();
gWbglocut ->drbmin=_drbmin_Parm.value();
gWbglocut ->ptlmin=_ptlmin_Parm.value();
gWbglocut ->etalmax=_etalmax_Parm.value();
gWbglocut ->drlmin=_drlmin_Parm.value();
gWbglocut ->ptnmin=_ptnmin_Parm.value();
gWbpdfvar ->iqopt=_iqopt_Parm.value();
gWbpdfvar ->qfac=_qfac_Parm.value();
gWbpdfvar ->ndns=_ndns_Parm.value();
gWboptiter ->nopt=_nopt_Parm.value();
gWboptiter ->niter=_niter_Parm.value();
gWbweights ->maxev=_maxev_Parm.value();
gWbkkseed ->kseed[0]=_seed1_Parm.value();
gWbkkseed ->kseed[1]=_seed2_Parm.value();
gWbgridunit ->jgridr=_jgridr_Parm.value();
gWbgridunit ->jgridw=_jgridw_Parm.value();
gWbleptontype ->iilepton=_iilepton_Parm.value();

int nch;
nch = (_wbbfilen.value().size()<20) ? _wbbfilen.value().size() : 19;
for(int i=0; i<19; i++) {
  if(i<nch) {
    gWbevtout->fname[i] = _wbbfilen.value().c_str()[i];
  }
  else {
    gWbevtout->fname[i] = ' ';
  }
}

}

void WbbgenModule::InitializeTalkTo() {
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

  _15Menu.initialize ("Wbbuser",this);
  _15Menu.initTitle ("Control wbb process");

  // Add descriptions for AbsParms
  // -------------------------------
  _maxer_Parm.addDescription(
  "\t\t\tNumber of errors allowed before run stops\n\
  \t\t\t The syntax is: Maxer set <maxer> ");
  // _randomseed1_Parm.addDescription(
  //"\t\t\tHerwig random number seed 1\n\
    //\t\t\t The syntax is: Randseed1 set <seed> ");
    //_randomseed2_Parm.addDescription(
    //"\t\t\tHerwig random number seed 2\n\
    //\t\t\t The syntax is: Randseed2 set <seed> ");

  _lambdaQCD_Parm.addDescription( 
  "\t\t\tSpecify the value of lamba QCD.\n\
  \t\t\tThe syntax is: Lambda_QCD set <lambda>\n\
  \t\t\twhere <lambda> is specified in GEV" );

  //
  // Descriptions of Beam Parameters, Menu 01
  // ----------------------------------------

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
  _pltcut_Parm.addDescription(
  "\t\t\tSet lifetime cut in secs above which particles are stable.\n\
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
  \t\t\twhere <Higgs> is in GEV" );
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

  // commands of Menu 15: Wbbuser Menu 
  // --------------------------------
  _iih1_Parm.addDescription(
  "\t\t\t1st incident beam, prot=3 antip=1");
  _iih2_Parm.addDescription(
  "\t\t\t2nd incident beam, prot=3 antip=1");
  _ebeam_Parm.addDescription(
  "\t\t\t1st incident beam energy in GeV");
  _mq_Parm.addDescription(
  "\t\t\tmass of b quark for Wbb process");
  _njets_Parm.addDescription(
  "\t\t\tno. of jets in Wbb + njets, def. is 3");
  _ndns_Parm.addDescription(
  "\t\t\tpdf set to use, in pdfset code, def. is 61");
  _ptjmin_Parm.addDescription(
  "\t\t\tmin. pt of non-b jet parton (GeV), def. is 20");
  _etajmax_Parm.addDescription(
  "\t\t\tmax. eta of non-b jet parton");
  _drjmin_Parm.addDescription(
  "\t\t\tmin. dr between two non-b jet partons");
  _ptbmin_Parm.addDescription(
  "\t\t\tmin. pt of b parton (GeV), def. is 20");
  _etabmax_Parm.addDescription(
  "\t\t\tmax. eta of b parton");
  _drbmin_Parm.addDescription(
  "\t\t\tmin. dr between two b partons");
  _ptlmin_Parm.addDescription(
  "\t\t\tmin. pt of charged lepton (GeV), def. is 20");
  _etalmax_Parm.addDescription(
  "\t\t\tmax. eta of charged lepton");
  _drlmin_Parm.addDescription(
  "\t\t\tmin. dr between chg lepton and a parton b");
  _ptnmin_Parm.addDescription(
  "\t\t\tmin. pt of neutrino (GeV), def. is 0");
  _iqopt_Parm.addDescription(
  "\t\t\tqsq scale, 0 (MW**2), 1 (MW**2+PT**2)");
  _qfac_Parm.addDescription(
  "\t\t\tqfac for Wbb process");
  _nopt_Parm.addDescription(
  "\t\t\tno. of events per iteration");
  _niter_Parm.addDescription(
  "\t\t\tno. of iterations ");
  _maxev_Parm.addDescription(
  "\t\t\tmax. no. of events to generate");
  _seed1_Parm.addDescription(
  "\t\t\tfirst seed, integer");
  _seed2_Parm.addDescription(
  "\t\t\tsecond seed, integer");
  _jgridr_Parm.addDescription(
  "\t\t\tgrid read unit, def. is 0, cannot be 70-75");
  _jgridw_Parm.addDescription(
  "\t\t\tgrid write unit, def. is 22, cannot be 70-75");
  _iilepton_Parm.addDescription(
  "\t\t\tlepton from W decay, 11 for el (def), 13 mu, 15 tau");
  _wbbfilen.addDescription(
  "\t\t\tprimary name of intermediate files");

  // Attach all menus and parameters to the appropriate pages
  // ---------------------------------------------------------
  commands( )->append( &_maxer_Parm);
  //commands( )->append( &_randomseed1_Parm);
  //commands( )->append( &_randomseed2_Parm);
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


  //   _01Menu.commands( )->append( &_Beamtype1_Parm);
  //   _01Menu.commands( )->append( &_Energy1_Parm);
  //   _01Menu.commands( )->append( &_Beamtype2_Parm);
  //   _01Menu.commands( )->append( &_Energy2_Parm);  

  _01Menu.commands( )->append( &_pipsmr_Parm);
  _01Menu.commands( )->append( &_vipwidx_Parm);
  _01Menu.commands( )->append( &_vipwidy_Parm);
  _01Menu.commands( )->append( &_vipwidz_Parm);

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
  _08Menu.commands( )->append (& _pltcut_Parm);
  _08Menu.commands( )->append (& _modbos1_Parm);
  _08Menu.commands( )->append (& _modbos2_Parm);

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
   
  _15Menu.commands()->append(& _iih1_Parm);
  _15Menu.commands()->append(& _iih2_Parm);
  _15Menu.commands()->append(& _ebeam_Parm);
  _15Menu.commands()->append(& _mq_Parm);
  _15Menu.commands()->append(& _njets_Parm);
  _15Menu.commands()->append(& _ndns_Parm);
  _15Menu.commands()->append(& _ptjmin_Parm);
  _15Menu.commands()->append(& _etajmax_Parm);
  _15Menu.commands()->append(& _drjmin_Parm);
  _15Menu.commands()->append(& _ptbmin_Parm);
  _15Menu.commands()->append(& _etabmax_Parm);
  _15Menu.commands()->append(& _drbmin_Parm);
  _15Menu.commands()->append(& _ptlmin_Parm);
  _15Menu.commands()->append(& _etalmax_Parm);
  _15Menu.commands()->append(& _drlmin_Parm);
  _15Menu.commands()->append(& _ptnmin_Parm);
  _15Menu.commands()->append(& _iqopt_Parm);
  _15Menu.commands()->append(& _qfac_Parm);
  _15Menu.commands()->append(& _nopt_Parm);
  _15Menu.commands()->append(& _niter_Parm);
  _15Menu.commands()->append(& _maxev_Parm);
  _15Menu.commands()->append(& _seed1_Parm);
  _15Menu.commands()->append(& _seed2_Parm);
  _15Menu.commands()->append(& _jgridr_Parm);
  _15Menu.commands()->append(& _jgridw_Parm);
  _15Menu.commands()->append(& _iilepton_Parm);
  _15Menu.commands()->append(&  _wbbfilen);
}

//-----------------------------------------------------------------------------
// $Id:
// $Log:
//-----------------------------------------------------------------------------




