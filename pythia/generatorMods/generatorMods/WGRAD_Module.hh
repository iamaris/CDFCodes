#ifndef _WGRAD_MODULE_HH
#define _WGRAD_MODULE_HH

// Declarations for Fortran routines usesed by this class
extern "C" {
  //  First the routines to set parameters from talk-to :  
  void wgrad_set_external_();
  void wgrad_use_pythia_(int wgrad_use_pythia[]);
  void wgrad_set_switches_(int ppswitches[]);
  void wgrad_set_soft_collinear_cuts_(double soft_coll_cuts[]);
  void wgrad_set_pdflib_(int pdflib_choice[]);
  void wgrad_set_properties_(double properties[]);
  void wgrad_set_kinematic_cuts_(double kinematic_cuts[]);
  void wgrad_set_grid_pars_(int grid_pars[]);
  void wgrad_set_rndm_seed_(int rndm_seed[]);
			  
  // Perform the grid search and precision calculation :
  void wgrad_begin_();

  // Generate a single unweighted event :
  void wgrad_evt_();

  // Print some information and finish :
  void wgrad_end_();

  // Pythia interface routines (one per Pythia common block) :
  // void wgrad_pythia_set_mstj_(int wgrad_pythia_mstj[]);
  // void wgrad_pythia_set_mrpy_(int wgrad_pythia_mrpy[]);
  // void wgrad_pythia_set_mstp_(int wgrad_pythia_mstp[]);
  // void wgrad_pythia_set_parp_(double wgrad_pythia_par[]);

  // Extract the event weight from WGRAD :
  void wgrad_get_event_weight_(double wgrad_event_weight[]);

  // Access the WGRAD common blocks in case Pythia is not run :
  void wgrad_get_particle_codes_(int wgrad_particle_codes[]);
  void wgrad_get_charged_lepton_(double wgrad_charged_lepton[]);
  void wgrad_get_neutrino_(double wgrad_neutrino[]);
  void wgrad_get_photon_(double wgrad_photon[]);
}

//----------------------
// Base Class Headers --
//----------------------
#include "Framework/APPModule.hh"
#include "Framework/AbsParmBool.hh"
#include "Framework/AbsParmFilename.hh"
#include "Framework/APPMenu.hh"
#include "generatorMods/PythiaSetCommands.hh"
#include "generatorMods/PythiaShowCommands.hh"
#include "generatorMods/AbsGenModule.hh"
#include "generatorMods/PythiaMenu.hh"
#include "Framework/AbsParmGeneral.hh"
typedef AbsParmGeneral<long> AbsParmGeneral_long;

#ifdef CDF
#include "BaBar/Cdf.hh"
#endif
//		---------------------
// 		-- Class Interface --
//		---------------------

class WGRAD_Module : public AbsGenModule {

//--------------------
// Instance Members --
//--------------------

public:

  static const char* genId;

  // Constructors
  WGRAD_Module();
  
 // Destructor
  virtual ~WGRAD_Module();
  
  // Operations
  AppResult  genBeginJob  ();
  AppResult  genBeginRun  (AbsEvent* run);
  AppResult  genEndJob    ();
  AppResult  genEndRun    (AbsEvent* run );
  int        callGenerator(AbsEvent* event);
  void fillHepevt();

private:

  // Random number menu
  APPMenu _randomNumberMenu;
  
  // Initial random number seeds (2) (ignored if restoreState is requested)
  AbsParmGeneral_long _randomSeed1;
  static const   long _defaultRandomSeed1;
  AbsParmGeneral_long _randomSeed2;
  static const   long _defaultRandomSeed2;
  

  // Debug flag :
  AbsParmBool _debug;

  // Use Pythia ?
  AbsParmBool _usePythia;

  // Use Les Houches Module ?
  AbsParmBool _useLesHouchesModule;

  // -----------------------------------------------------
  // DSW 14.8.99
  // Talk-to parameters to replace input cards file :
  // ================================================
  // 1: p pbar collider 2: pp collider
  AbsParmGeneral<int> _beamType;
  // 1: W+ production, 2: W- production
  AbsParmGeneral<int> _W_charge;
  // 1: constant W width, 2: s-dependent W width
  AbsParmGeneral<int> _widthTreatment;
  // 1: electron, 2: muon
  AbsParmGeneral<int> _decayLepton;
  // 1: initial, 2: final, 4: all
  AbsParmGeneral<int> _QED_radType;
  // final state: 1: full, 2: a la Berends et al.
  AbsParmGeneral<int> _fsrType;
  // 0: no s.,no r., 1: with s,no r., 2: with s.,with r.
  AbsParmGeneral<int> _smearingRecombination;
  // 0:LO only; 1:NLO-LO ; 2:NLO
  AbsParmGeneral<int> _order;
  // 0:mu=MW, 1: mu=.5*MW, 2: mu=2*MW.
  AbsParmGeneral<int> _scale;
  // final state: without coll. cut (0), with (1)
  AbsParmGeneral<int> _fsrCollinearCut;
  // delta_s (photon energy cut)
  AbsParmGeneral<double> _photonEnergyCut;
  // delta_c (collinear cut)
  AbsParmGeneral<double> _photonCollinearCut;
  // pdflib group
  AbsParmGeneral<int> _pdflibGroup;
  // pdflib choice
  AbsParmGeneral<int> _pdflibSet;
  // W mass :
  AbsParmGeneral<double> _wMass;
  // W width :
  AbsParmGeneral<double> _wWidth;
  // Center of mass energy :
  AbsParmGeneral<double> _CME;
  // ptmin for lepton 
  AbsParmGeneral<double> _lepton_ptmin;
  // ymax for lepton 
  AbsParmGeneral<double> _lepton_ymax;
  // ptmin for neutrino
  AbsParmGeneral<double> _neutrino_ptmin;
  // ymax for neutrino
  AbsParmGeneral<double> _neutrino_ymax;
  // ptmin for photon
  AbsParmGeneral<double> _photon_ptmin;
  // ymax for photon
  AbsParmGeneral<double> _photon_ymax;
  // Transverse mass lower limit :
  AbsParmGeneral<double> _tmass_low;
  // Transverse mass upper limit :
  AbsParmGeneral<double> _tmass_high;
  // itmax for grid calculation.
  AbsParmGeneral<int> _itmaxGrid;
  // ncall for grid calculation.
  AbsParmGeneral<int> _ncallGrid;
  // itmax for precision calculation.
  AbsParmGeneral<int> _itmaxPrecision;
  // ncall for precision calculation.
  AbsParmGeneral<int> _ncallPrecision;
  // Random number seed :
  AbsParmGeneral<int> _rndmSeed;

  APPMenu _cardsMenu;

  // For WGRAD-Pythia interface :
  // AbsParmGeneral<int> _mstj_41;
  // AbsParmGeneral<int> _mrpy_1;
  // AbsParmGeneral<int> _mstp_81;
  // AbsParmGeneral<int> _mstp_91;
  // AbsParmGeneral<int> _mstp_111;
  // AbsParmGeneral<double> _parp_91;
  // AbsParmGeneral<double> _parp_92;
  // AbsParmGeneral<double> _parp_93;

  APPMenu _pythiaInterfaceMenu;
  // -----------------------------------------------------

  PythiaMenu* _pythiaMenu;


private:

  WGRAD_Module(const char* name, 
	       const char* description) :
    AbsGenModule( WGRAD_Module::genId, 
		  "W production including O(alpha) EW radiative corrections"),
    _debug("debug",this,false),
    _usePythia("usePythia",this,true),
    _useLesHouchesModule("useLesHouchesModule",this,false),
    _beamType("BeamType",this,1),
    _W_charge("W_Charge",this,1),
    _widthTreatment("widthTreatment",this,2),
    _decayLepton("DecayLepton",this,2),
    _QED_radType("QED_RadType",this,2),
    _fsrType("fsrType",this,1),
    _smearingRecombination("SmearingRecombination",this,0),
    _order("Order",this,2),
    _scale("Scale",this,0),
    _fsrCollinearCut("fsrCollinearCut",this,0),
    _photonEnergyCut("PhotonEnergyCut",this,0.0001),
    _photonCollinearCut("PhotonCollinearCut",this,0.0001),
    _pdflibGroup("pdflibGroup",this,3),
    _pdflibSet("pdflibSet",this,54),
    _wMass("W_Mass",this,80.3),
    _wWidth("W_Width",this,2.1),
    _CME("CME",this,2000.0),
    _lepton_ptmin("Lepton_ptmin",this,0.),
    _lepton_ymax("Lepton_ymax",this,100.),
    _neutrino_ptmin("Neutrino_ptmin",this,0.),
    _neutrino_ymax("Neutrino_ymax",this,100.),
    _photon_ptmin("Photon_ptmin",this,0.),
    _photon_ymax("Photon_ymax",this,100.),
    _tmass_low("mT_low",this,0.),
    _tmass_high("mT_high",this,2000.),
    _itmaxGrid("itmaxGrid",this,1),
    _ncallGrid("ncallGrid",this,2000),
    _itmaxPrecision("itmaxPrecision",this,2),
    _ncallPrecision("ncallPrecision",this,5000),
    _rndmSeed("randomSeed",this,4567),
    // _mstj_41("mstj(41)",this,-1),
    // _mrpy_1("mrpy(1)",this,-1),
    // _mstp_81("mstp(81)",this,-1),
    // _mstp_91("mstp(91)",this,-1),
    // _mstp_111("mstp(111)",this,-1),
    // _parp_91("parp(91)",this,-1.0),
    // _parp_92("parp(92)",this,-1.0),
    // _parp_93("parp(93)",this,-1.0)  ,
    _randomSeed1("RandomSeed",this,0),
    _randomSeed2("RandomSeed",this,0) ,
    _pythiaMenu()
  {}

  WGRAD_Module(const WGRAD_Module& m)
    :
    AbsGenModule( "x", "x" ),
    _debug("debug",this,false),
    _usePythia("usePythia",this,true),
    _useLesHouchesModule("useLesHouchesModule",this,false),
    _beamType("BeamType",this,1),
    _W_charge("W_Charge",this,1),
    _widthTreatment("widthTreatment",this,2),
    _decayLepton("DecayLepton",this,2),
    _QED_radType("QED_RadType",this,2),
    _fsrType("fsrType",this,1),
    _smearingRecombination("SmearingRecombination",this,0),
    _order("Order",this,2),
    _scale("Scale",this,0),
    _fsrCollinearCut("fsrCollinearCut",this,0),
    _photonEnergyCut("PhotonEnergyCut",this,0.0001),
    _photonCollinearCut("PhotonCollinearCut",this,0.0001),
    _pdflibGroup("pdflibGroup",this,3),
    _pdflibSet("pdflibSet",this,54),
    _wMass("W_Mass",this,80.3),
    _wWidth("W_Width",this,2.1),
    _CME("CME",this,2000.0),
    _lepton_ptmin("Lepton_ptmin",this,0.),
    _lepton_ymax("Lepton_ymax",this,100.),
    _neutrino_ptmin("Neutrino_ptmin",this,0.),
    _neutrino_ymax("Neutrino_ymax",this,100.),
    _photon_ptmin("Photon_ptmin",this,0.),
    _photon_ymax("Photon_ymax",this,100.),
    _tmass_low("mT_low",this,0.),
    _tmass_high("mT_high",this,2000.),
    _itmaxGrid("itmaxGrid",this,1),
    _ncallGrid("ncallGrid",this,2000),
    _itmaxPrecision("itmaxPrecision",this,2),
    _ncallPrecision("ncallPrecision",this,5000),
    _rndmSeed("randomSeed",this,4567),
    //_mstj_41("mstj(41)",this,-1),
    //_mrpy_1("mrpy(1)",this,-1),
    //_mstp_81("mstp(81)",this,-1),
    //_mstp_91("mstp(91)",this,-1),
    //_mstp_111("mstp(111)",this,-1),
    ////_parp_91("parp(91)",this,-1.0),
    //_parp_92("parp(92)",this,-1.0),
    //_parp_93("parp(93)",this,-1.0)  ,
    _randomSeed1("RandomSeed",this,0),
    _randomSeed2("RandomSeed",this,0),
    _pythiaMenu()
  {}

};

#endif
