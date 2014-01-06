//----------------------------------------------------------------------
//  MinBias code
//----------------------------------------------------------------------
// Rev. Aug 29 lena: clean up - removed USE_ROOT_ and __CINT__
//      aug 30 2001 lena : added ct, genId
//      Nov 05 2002 Anwar : Move add_particle_HepEvt from MinBias MinBiasModule.cc

#ifndef __MINBIAS_HH__
#define __MINBIAS_HH__

#include <iostream>
using std::ostream;

//----------------------
// Base Class Headers --
//----------------------
#include <math.h>

#include "Framework/AbsParmGeneral.hh"
typedef AbsParmGeneral<long> AbsParmGeneral_long;

class AbsEvent;
class AppResult;
class HepRandomEngine;

#include "generatorMods/AbsGenModule.hh"

class MinBiasModule : public AbsGenModule {

protected:

  int init;
//  int nt;
  int mssflg,mulflg,iknflg,iermsg,iermlg,ierkng;
  int nevmsg,nevmlg,nevkng,never; 
  int nmbr,nevhc,nevddf,nevsdf,nevela;
  int stable;
				// code of the process to be generated
  int fProcess;
     
  int NKGPAK,MAXGEN;

  double  sgrat[5], sgdrat[5],pcmlab[4],pbeam,tecm,s,bElastic,b0,b0inv,pnrat;
  double  ammin, ammax, ammin2,ammax2,cmlim, rclim2;
  double  sig0,sigel,sigsd,sigdd,sigres,sigtot;
  double  rate[5],drate[5];
  double  ddpmax;

  double  multiplicityScaleFactor;
  double  epsilon;
  double  flatRapidityRegion;
  int icont,isdp,isdpb;

  int inelasticEvent,singleDiffractiveEvent,doubleDiffractiveEvent,
      elasticEvent;
          
public:

  static const char* genId;
  static HepRandomEngine* minBiasEngine;
                                        // Constructors
  MinBiasModule();

                                        // Destructor
  virtual ~MinBiasModule( );
                                        // ****** overloaded methods of 
					// GeneratorModule

  int         callGenerator(AbsEvent* event);
  AppResult   genBeginRun  (AbsEvent* event);
  AppResult   genBeginJob  ();
  AppResult   genEndRun    (AbsEvent* anEvent);
  AppResult   genEndJob    ();
  void        fillHepevt   ();
  // temporary solution - this is called in fillHepevt
  void        writeHEPGbank(); 
  void    add_particle_HepEvt(int type,int firstMother,int lastMother,
                                     int status,double p[4],double mass);


                                        // ****** access functions

  int     Process() { return fProcess; }
  double  MultiplicityScaleFactor() {return multiplicityScaleFactor;} 


					// ****** setters

  void    SetProcess(int p) { fProcess = p; }
  void    SetMultiplicityScaleFactor(double x){multiplicityScaleFactor=x;}

//--------------------
// Instance Members --
//--------------------

  void  initEvent(void);
  
  void  mbr_generate_event(void);

  int   hard_core_event(void);
  int   double_diffractive_event(void);
  int   single_diffractive_event(void);
  int   elastic_event(void);
  int   fragment_cluster(int ngenmx,double pxcm[4],int icharg);
  
  void  generate_lorentz_boost(double pcmlab[4]);
  void  add_lorentz_boost(double beta[4],double v1[4],double v2[4]);
  void  generate_one_particle_from_fire_ball(int ngenpa, int itype,int nt);
  void  rotate_three_vector(double phi,double thet,double psi,
			    double &x,double &y,double &z);

  double  generate_single_diffractive_t(double amx);
  double  generate_double_diffractive_t(void);


  int   average_multiplicity(double amx);
  int   total_multiplicity_by_dino(double wmu,double avnstar);
  int   two_body_decay(int ngenmx,int ip1,int ip2);
  int   three_body_decay(int ngenmx,int ip1,int ip2, int ip3);
  int   balance_energy_momentum(double amx,int ngen0);

  double  generate_diffractive_mass(void);
  double  generate_random_o2(void);
  double  generate_pt(double amx,int nt);
  double  generate_rapidiy(double amx);
  double  mbr_gamma(double);

  void    print(void);
  //void    writeMbrEvt2HepgBank(AbsEvent*);

  inline double  pfun(double y) {
    return 1.0+cos(y)*cos(y);
  }

  inline double  f3(double x,double ptsoft, double power) {
    return x/pow((1.+x/ptsoft),power);
  }

  inline double  pamass(int i) {
    if      (i==2212 || i==-2212) return 0.938;
    else if (i==2112 || i==-2112) return 0.940;
    else if (i==111)              return 0.135;
    else                          return 0.139;
  }

private:

   void initrn(void);

   AbsParmGeneral<int> _process;
   AbsParmGeneral<float> _pbeam;
   AbsParmGeneral<float> _scale;
   AbsParmGeneral<float> _rprot;
   AbsParmGeneral<float> _rneutr;
   AbsParmGeneral<int> _isdp;
   AbsParmGeneral<int> _isdpb;
   AbsParmGeneral<int> _sdvers;
   AbsParmGeneral<int> _icont;
   AbsParmGeneral<int> _ddvers;
   AbsParmGeneral<float> _ammin;
   AbsParmGeneral<float> _ammax;
   AbsParmGeneral<float> _ximax;
   AbsParmGeneral<float> _ximin;
   AbsParmGeneral<float> _dymin;
   AbsParmGeneral<float> _epsilon;
   AbsParmGeneral<float> _sigpomp;
   AbsParmGeneral<float> _alpha;

  // Random number menu
  APPMenu _randomNumberMenu;
  
  // Initial random number seeds (2) (ignored if restoreState is requested)
   AbsParmGeneral_long _randomSeed1;
  static const   long _defaultRandomSeed1;
  AbsParmGeneral_long _randomSeed2;
  static const   long _defaultRandomSeed2;

private:
  MinBiasModule(const char* theName,
		const char* theDescription): 
    AbsGenModule( theName, theDescription),
    _process( "process", this, 1 ),
    _pbeam( "pbeam", this, 900.0 ),
    _scale( "mult_scale", this, 1.15 ),
    _rprot( "rel_prot", this, 1.0 ),
    _rneutr( "rel_neutr", this, 1.0 ),
    _isdp( "sd_p_only", this, 0 ),
    _isdpb( "sd_pbar_only", this, 0 ),
    _sdvers( "sd_vers", this, 1 ),
    _icont( "no_resonance", this, 0 ),
    _ddvers( "dd_vers", this, 1 ),
    _ammin( "mmin", this, sqrt(1.5) ),
    _ammax( "mmax", this, -1. ),
    _ximax( "ximax", this, 0.15 ),
    _ximin( "ximin", this, -1. ),
    _dymin( "dymin", this, 2.3 ),
    _epsilon( "epsilon", this, 0.104 ),
    _sigpomp( "sigpomp", this, 2.82 ),
    _alpha( "alpha", this, 0.25 ),
    _randomSeed1( "RandomSeed1", this, MinBiasModule::_defaultRandomSeed1 ),
    _randomSeed2( "RandomSeed2", this, MinBiasModule::_defaultRandomSeed2 )
  {}

  MinBiasModule(const MinBiasModule& m): 
    AbsGenModule( "x", "x"),
    _process( "process", this, 1 ),
    _pbeam( "pbeam", this, 900.0 ),
    _scale( "mult_scale", this, 1.15 ),
    _rprot( "rel_prot", this, 1.0 ),
    _rneutr( "rel_neutr", this, 1.0 ),
    _isdp( "sd_p_only", this, 0 ),
    _isdpb( "sd_pbar_only", this, 0 ),
    _sdvers( "sd_vers", this, 1 ),
    _icont( "no_resonance", this, 0 ),
    _ddvers( "dd_vers", this, 1 ),
    _ammin( "mmin", this, sqrt(1.5) ),
    _ammax( "mmax", this, -1. ),
    _ximax( "ximax", this, 0.15 ),
    _ximin( "ximin", this, -1. ),
    _dymin( "dymin", this, 2.3 ),
    _epsilon( "epsilon", this, 0.104 ),
    _sigpomp( "sigpomp", this, 2.82 ),
    _alpha( "alpha", this, 0.25 ),
    _randomSeed1( "RandomSeed1", this, MinBiasModule::_defaultRandomSeed1 ),
    _randomSeed2( "RandomSeed2", this, MinBiasModule::_defaultRandomSeed2 )
  {}


};

#endif  // _MINBIAS_HH_ end of MinBias.hh











