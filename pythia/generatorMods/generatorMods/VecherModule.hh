//--------------------------------------------------------------------------
// Description:
//      C++ wrapper to put VEGAS/PAPAGENO unweighting into AC++
// Environment:
//	Software developed for the CDF Detector.
//
// Author List:
//      G.Velev                
//
// revision history:
// -----------------
//
//  July 11, 2001, G.Velev, copy all the talk-to parameters from herwig to  
//                 support herprt talk 
//  July 18, 2001, G.Velev, add initialize talk-to 
//  Aug 29, 2001   lena: remove USE_ROOT_
//------------------------------------------------------------------------

#ifndef __VECHERMODULE_HH__
#define __VECHERMODULE_HH__

//----------------------
// Base Class Headers --
//----------------------

class AbsEvent;

#include "Edm/StorableObject.hh"
#include "Framework/APPModule.hh"
#ifdef CDF
#include "BaBar/Cdf.hh" 
#endif
#include "Framework/AbsParmBool.hh"
#include "vecbos_i/vecbos_i_common_blocks.hh"
#include <string>
 
class VecherModule : public  AppModule {
//--------------------
// Instance Members --
//--------------------

public:

  static const char* genId;
					// Constructors
  VecherModule( const char* const theName = "Vecher", 
		const char* const theDescription = "Interface VECBOS -> HERWIG" );
  // Destructor
  virtual ~VecherModule( );
  // Operations
  virtual AppResult   beginRun( AbsEvent* event);
  virtual AppResult   beginJob( AbsEvent* event);
  virtual AppResult   endRun  ( AbsEvent* event);
      virtual AppResult   endJob  ( AbsEvent* event);
  virtual AppResult   event   ( AbsEvent* event);
  
  void fillHepevt( AbsEvent* event);
  
  //      bool        passed       () const {return _passed;}
  void _initializeTalkTo(); 
  void        putParametersToVecher(VECBOS_I_COMMON_BLOCKS *);
  void        getParametersFromVecher(VECBOS_I_COMMON_BLOCKS *);
  
private:

  // private counter
  int _events;

  // pointer to the common blocks
  VECBOS_I_COMMON_BLOCKS * common;

  // Parameters that control module operation
  // These can be set via the talk to
 
  AbsParmBool _summary;
  AbsParmGeneral<int> _lepton_type;

  // Herwig part 

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
			// Here are the things our specialized commands 
				// will set...

				// the AbsParms below at begin job
#ifndef __CINT__

  AbsParmGeneral<int> _process;              // QCD process generated
  AbsParmGeneral<int> _maxer_Parm;           //
  AbsParmGeneral<int> _randomseed1_Parm;      //
  AbsParmGeneral<int> _randomseed2_Parm;      //
//cjl  AbsParmGeneral<float> _lambdaQCD_Parm;     // to calculate alphas
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
     

				// commands of Menu 02 - v6.1 no read/write

  AbsParmGeneral<int> _sudord_Parm ; //Sudakov order    

				// commands of Menu 03

  AbsParmGeneral<std::string> _autpdf;
  AbsParmGeneral<int> _modpdf;


				// commands of Menu 04

//cjl  AbsParmGeneral<double> _rapiditiesm_Parm;// select rapidity limits min
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
  //  AbsParmGeneral<double> _gluonmass_Parm;//gluon virtual mass cutoff
  AbsParmGeneral<double> _intrinsicpt_Parm;// intrinsic pt incoming hadrons  

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
  AbsParmGeneral<double> _decwt_Parm; // a priori weights for decuplet baryons for cluster decays  
  AbsParmGeneral<double> _pwt_Parm; // a priori weights for quarks and diquarks
  
				// commands of Menu 07 

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
  AbsParmGeneral<double> _ensof_Parm;//multy. enhancement for underlyng soft event

				// commands of Menu 08

  AbsParmGeneral<int> _pizstable_Parm; //whether pizero is stable (non-decaying)
  AbsParmGeneral<double> _pltcut_Parm; //lifetime cut for stable particle
  
				// commands of Menu 09

  AbsParmGeneral<int> _eventweight_Parm; //control event weights
  AbsParmGeneral<double> _maxweight_Parm; //set max weight
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
  AbsParmGeneral<double> _hwidth_Parm;//Width of Higgs boson  

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
  
				// commands of Menu 12

  AbsParmGeneral<std::string> _read_Filesusy; //parm for the susy file name 

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
  AbsParmGeneral<int>  _read_Sudakov_Table; // flag, reads Sudakov table from file
  AbsParmGeneral<int>  _write_Sudakov_Table; // flag, write Sudakov table to file 
#endif

};

#endif
