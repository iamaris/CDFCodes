//--------------------------------------------------------------------------
// Description:  VecbosModule
//      
// Wed Jul 11 16:24:20 CDT 2001, G.Velev  add vegas read/write flag
//
// rev. Aug 30 2001 lena: add genId and ct's; changed Generatormodule on 
//                        AbsGenModule


#ifndef VECBOSMODULE_HH
#define VECBOSMODULE_HH

#include "generatorMods/AbsGenModule.hh"
#include "BaBar/Cdf.hh"
#include "stdhep_i/CdfHepEvt.hh"
#include "Framework/AbsParmGeneral.hh"
typedef AbsParmGeneral<long> AbsParmGeneral_long;
#include <string>

class VecbosModule : public AbsGenModule {

public:
  
  static const char* genId;
					// Constructors
  VecbosModule();
    
                                        // Destructor
  virtual ~VecbosModule( );
					// ****** overloaded methods of 
					// GeneratorModule

  AppResult   genBeginRun     (AbsEvent* event);
  AppResult   genBeginJob     ();
  AppResult   genEndRun       (AbsEvent* event);
  AppResult   genEndJob       ();
  int         callGenerator   (AbsEvent* event);

  void fillHepevt();
    					// ****** access functions

private:

  // private event counter
  int _events;

  CdfHepEvt _hepevt;

  // Random number menu
  APPMenu _randomNumberMenu;

  // Initial random number seeds (2) (ignored if restoreState is requested)
  AbsParmGeneral_long _randomSeed1;
  static const   long _defaultRandomSeed1;
  AbsParmGeneral_long _randomSeed2;
  static const   long _defaultRandomSeed2;

  void _initializeTalkTo();
  void putParametersToVecbos(); 
  void getParametersFromVecbos();

  // Menus:
  APPMenu _structureFunctionMenu;
  APPMenu _importantMenu;
  APPMenu _isubMenu;
  APPMenu _processMenu;
  APPMenu _primaryCutsMenu;
  APPMenu _vegasParamMenu;

  // debug flag
  AbsParmBool _debug;    
  
  // Decay W or Z
  AbsParmBool _decay_boson;

  //Force the boson to decay to quarks
  AbsParmBool _make_jets ;
  
  // Beam type 
  AbsParmGeneral<int> _beam_type;
  
  //Which Boson W (1) or Z (0)
  AbsParmGeneral<int> _which_boson;

  //W charge +1 or -1, 0 means both
  AbsParmGeneral<int> _charge_of_W;

  //  _howto_decay_Z;
  AbsParmGeneral<int> _howto_decay_Z;

  // Number of jets, max 4
  AbsParmGeneral<int> _njets;
  
  // What kind of structure  funxtion to use 
  AbsParmGeneral<int> _structure_function;

  // If use external pdf function, which group
  AbsParmGeneral<int> _pdf_group;

  // If  use external pdf function, which set
  AbsParmGeneral<int> _pdf_set;

  // Init Run number
  AbsParmGeneral<int> _run_number;

  // QCD scale 
  AbsParmGeneral<int> _qcd_scale;

  // Perform MC over helicities
  AbsParmGeneral<int> _helicity;
  
  // Force bbbar in final state
  AbsParmGeneral<int> _force_bbbar;

  // Use importance sampling for jet pt
  AbsParmGeneral<int> _important;

  // Choose importance sampling mode
  AbsParmGeneral<int> _sampling_mode;

  // Lepton type, 1-electron, 2-muon, 3-taus (don't thisnk it will work)
  AbsParmGeneral<int> _lepton_type;

  // What kind of matrix element, exact, 1., 1./shat^(njets) 
  AbsParmGeneral<int> _matrix_ele;

  // 2,4 or 6 quarks subprocesses 
  AbsParmGeneral<int> _subprocesses;

  //Number of leptons required to pass the  cuts  
  AbsParmGeneral<int> _no_of_leptons;

  //Number of leptons required to pass the  cuts  
  AbsParmGeneral<int> _vegas_rw_flag;

  // Some internal parameters 
  AbsParmGeneral<int> _vegas_print;
  AbsParmGeneral<int> _vegas_n_inter;
  AbsParmGeneral<int> _vegas_n_calls1;
  AbsParmGeneral<int> _vegas_n_calls2;

  // Doubles
  // Set the CM energy, maybe 2TeV 
  AbsParmGeneral<double> _cm_energy;

  // Alpha for jet generation
  AbsParmGeneral<double> _alpha_jet_generation;
 
  // Min Jet Pt
  AbsParmGeneral<double> _jet_min_pt;

  // Sum Jet Pt
  AbsParmGeneral<double> _jet_sum_pt;

  // Max Jst abs(eta)
  AbsParmGeneral<double> _jet_eta_max;
  
  // Jet-jet eta separation
  AbsParmGeneral<double> _jet_eta_separation;

  // Minumum pt of the vector boson
  AbsParmGeneral<double> _boson_min_pt;

  // Max lepton abs(eta) 
  AbsParmGeneral<double> _lep_max_eta;

  // Min lepton Pt
  AbsParmGeneral<double> _lep_min_pt;
 
  // Min missing pt cut 
  AbsParmGeneral<double> _mis_pt_min;
  
  // Min weight to pass the event
  AbsParmGeneral<double> _min_weght;


  // Strings
  //Input vegas grid file 
  AbsParmGeneral<std::string> _vegas_inp_grid_file;
  
  //Output Vegas grid to file 
  AbsParmGeneral<std::string> _vegas_out_grid_file;

};

#endif

