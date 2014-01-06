//***************************************************************************
//*  C++ version of  Minimum Bias event generator  adopated from mbr        *
//*  Anwar Ahmad Bhatti            December 24, 1998                        *
//*
//*
//* Anwar May 5,99 Update code to conform with fnalp/development version
//*                1)include user scale factor for multiplicity,minimum=2
//*                2)add "epsilon parameter' for mass generation
//*                3)change energy/momentum balancing routine **3==>alpha
//*                4)rapidity plateaue, 0.35*log(mass) ==>0.40*log(mass)
//*                5)additional user switches
//*                 
//* Mary Convery Oct 20, 1999
//*                1)bug fixes
//*                2)add talk-to parameters
//*                3)add new methods of Double and Single Diffractive 
//*                  event generation (cf CDF 4947) as defaults;
//*                  old methods can be switched on
//*                 
//***************************************************************************
// Rev:    aug 29 2001 lena:   clean up - removed USE_ROOT_ and gEvent
//         aug 30 2001 lena:   added ct and genId; change printout;
//         Apr 08 2002 anwar:  fix gcc problem
//         Nov 05 2002 anwar:  Move add_particle_HepEvt from MinBias MinBiasModule.cc
#include <string>
#include <math.h>

#include "Framework/APPFramework.hh"
//#include "evt/evt.h"
//#include "evt/Event.hh"
#include "inc/misc.hh"
#include "r_n/CdfRn.hh"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGaussT.h"
#include "CLHEP/Random/RandomEngine.h"
#include "generatorMods/MinBiasConstants.hh"
#include "generatorMods/MinBiasModule.hh"

#include "stdhep_i/CdfHepevt.hh"
#include "AbsEnv/AbsEnv.hh"
#include "SimulationObjects/HEPG_StorableBank.hh"
#include "Edm/Handle.hh"

#include <sstream>
using std::ostream;
using std::cout;
using std::endl;
using std::ostringstream ;

//void add_particle_HepEvt(int type,int firstMother,int lastMother,
//                                    int status,double p[4],double mass);

const char* MinBiasModule::genId = "mbr";
const long MinBiasModule::_defaultRandomSeed1 = 92253591;
const long MinBiasModule::_defaultRandomSeed2 = 35476;

HepRandomEngine* MinBiasModule::minBiasEngine = 0;

//----------------
// Constructors --
//----------------

MinBiasModule::MinBiasModule()
  : AbsGenModule( MinBiasModule::genId,"Minimum Bias AC++ module"),
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
    _randomSeed1("RandomSeed1",this,MinBiasModule::_defaultRandomSeed1),
    _randomSeed2("RandomSeed2",this,MinBiasModule::_defaultRandomSeed2)
{
  _process.addDescription(
"  Choose a combination of Hard Core, Double-, Single-Diffractive, Elastic:\n\
   Process type=1*HC+10*DD+100*SD+1000*EL, eg 101=HC+SD\n\
    Do not add leading Zeros\n\
    Default: generate hard core (inelastic, non-diffractive) events only (1)");
  _pbeam.addDescription(
"  Enter the beam energy in GeV (900)");
  _scale.addDescription(
"  Enter the multiplicity scale factor (1.15)");
  _rprot.addDescription(
"  Enter the relative probability for the nucleon in the decay of diffracted\n\
    masses to be a (anti)proton (1)");
  _rneutr.addDescription(
"  Enter the relative probability for the nucleon in the decay of diffracted\n\
    masses to be a (anti)neutron (1)");
  _isdp.addDescription(
"  Generate single diffraction of the proton only: 0=false 1=true (0)");
  _isdpb.addDescription(
"  Generate single diffraction of the antiproton only: 0=false 1=true (0)");
  _sdvers.addDescription(
"  New/old version of generating single diffractive events and cross section\n\
    New version (1999) uses renormalized gap probability, old version can\n\
    generate resonances at low energies: 0=old 1=new (1)");
  _icont.addDescription(
"  Do not include resonances in single diffraction: 0=resonances 1=no res (0)\n\
    Used only in old SD version");
  _ddvers.addDescription(
"  New/old version of generating double diffractive events and cross section\n\
    New version (1999) uses renormalized gap probability, old version events\n\
    always generated with t=1: 0=old 1=new (1)");
  _ammin.addDescription(
"  Enter the minimum diffractive mass in GeV (sqrt(1.5))\n\
    Used in old SD/DD version and new DD dy_max=log(s/mmin^4)");
  _ammax.addDescription(
"  Enter the maximum diffractive mass in GeV (else will set to sqrt(0.15s))\n\
    Used only in old SD version, set ximax for new");
  _ximax.addDescription(
"  Enter the maximum single diffractive pomeron momentum fraction\n\
    xi=1-x_F (0.15)\n\
    Used only in new SD version.");
  _ximin.addDescription(
"  Enter the minimum single diffractive pomeron momentum fraction\n\
    xi=1-x_F (0.)\n\
    Default will be set to 1.5/s.  Used only in new SD version.");
  _dymin.addDescription(
"  Enter the minimum double diffractive rapidity gap width (2.3)\n\
    Used only in new DD version.  Note dy_max=log(s/mmin^4)");
  _epsilon.addDescription(
"  Enter the value of epsilon to be used in the diffractive mass \n\
    distributions (0.104)");
  _sigpomp.addDescription(
"  Enter the value of the total pomeron-proton cross section in mb (2.82)");
  _alpha.addDescription(
"  Enter the value of alpha' to be used in the pomeron trajectory (0.25)");

  commands( )->append( &_process);
  commands( )->append( &_pbeam);
  commands( )->append( &_scale);
  commands( )->append( &_rprot);
  commands( )->append( &_rneutr);
  commands( )->append( &_isdp);
  commands( )->append( &_isdpb);
  commands( )->append( &_sdvers);
  commands( )->append( &_icont);
  commands( )->append( &_ddvers);
  commands( )->append( &_ammin);
  commands( )->append( &_ammax);
  commands( )->append( &_ximax);
  commands( )->append( &_ximin);
  commands( )->append( &_dymin);
  commands( )->append( &_epsilon);
  commands( )->append( &_sigpomp);
  commands( )->append( &_alpha);

// Initialize the relevant submenu
  _randomNumberMenu.initialize("RandomNumberMenu",this);
  _randomNumberMenu.initTitle("MinBiasModule random number menu");
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
}

//--------------
// Destructor --
//--------------

MinBiasModule::~MinBiasModule() { }

//_____________________________________________________________________________
int MinBiasModule::callGenerator(AbsEvent* event){
  CdfHepevt* hepevt = CdfHepevt::Instance();
  hepevt->clearCommon();
  
  mbr_generate_event();
  return 0;
}

//_____________________________________________________________________________
AppResult  MinBiasModule::genBeginJob(){

  CdfRn* rn = CdfRn::Instance();
  if ( !rn->isReadingFromFile() ) {
    rn->SetEngineSeeds(_randomSeed1.value(), 
                       _randomSeed2.value(),MinBiasModule::genId);
  }

  MinBiasModule::minBiasEngine = CdfRn::Instance()->GetEngine(MinBiasModule::genId);

  initEvent();

  return AppResult::OK;
}

//_____________________________________________________________________________
AppResult  MinBiasModule::genBeginRun(AbsEvent* anEvent){
  return AppResult::OK;
}

//_____________________________________________________________________________
AppResult  MinBiasModule::genEndRun(AbsEvent* anEvent){
  return AppResult::OK;
}

//_____________________________________________________________________________

AppResult  MinBiasModule::genEndJob(){
  return AppResult::OK;
}

void MinBiasModule::fillHepevt() { writeHEPGbank(); }
       
void MinBiasModule::writeHEPGbank() {

  // should work?
  AbsEvent* event = AbsEnv::instance()->theEvent();
  CdfHepEvt hepevt;
  HEPG_StorableBank* hepg = hepevt.makeHEPG_Bank(event);

  if (!hepg || hepg->is_invalid()) {
    ERRLOG(ELwarning,"MinBiasModule") << "Can't create a valid HEPG_StorableBank" << endmsg;
    return;
  }
  // Add the bank to the event
  // Get the handle
  Handle<HEPG_StorableBank> h(hepg);
  // Append the bank to the event
  if ((event->append(h)).is_null()) {
   ERRLOG(ELwarning,"MinBiasModule") << "Can't add HEPG_StorableBank to the event." << endmsg;
    return;
  }
  return;
  
}

//_____________________________________________________________________________
void  MinBiasModule::initEvent(void){

  std::cout <<  " MBR initialization routine " << std::endl;

  stable=1;

  double  rprot,rneutr,szero,fact,aa,bb,ratsum,alpha;
  double  sig;

  nmbr                    = 0;
  nevhc                   = 0;
  nevddf                  = 0;
  nevsdf                  = 0;
  nevela                  = 0;
  never                   = 0;
  nevmlg                  = 0;
  nevkng                  = 0;
  ierkng                  = 0;
  iermlg                  = 0;
  mssflg                  = 0;
  mulflg                  = 0;
  iknflg                  = 0;

  int  HardCore           = 0;
  int  SingleDiffractive  = 0;
  int  DoubleDiffractive  = 0;
  int  ElasticScattering  = 0;
				// 'Process type=1*HC+10*DD+100*SD+1000*EL',
				//  by default generate hard core events only
//fProcess=101;                 //  do not add leading Zeros 
  //fProcess=1111;
  fProcess=_process.value();
  
  if ((fProcess%10)==1)        HardCore=1;
  if (((fProcess/10)%10)==1)   DoubleDiffractive=1;
  if (((fProcess/100)%10)==1)  SingleDiffractive=1;
  if (((fProcess/1000)%10)==1) ElasticScattering=1;


  // Following parameters are taken from the MBRPAR file
  //
      
  MAXGEN = 5000;

  pbeam  = _pbeam.value();
  tecm   = 2.0*sqrt(pbeam*pbeam+AMP*AMP);
  s      = tecm*tecm;
  rprot  = _rprot.value();
  rneutr = _rneutr.value();

  //  Add additional parameters   from MBRPAR  anwar May 5,1999
  
  multiplicityScaleFactor=_scale.value();
  epsilon = _epsilon.value();
  icont   = _icont.value();
  isdp    = _isdp.value();
  isdpb   = _isdpb.value();

  flatRapidityRegion=0.4;
                           // taken from geneta. Flat  rapidity region is
                           // flatRapidityRegion*alog(mass)

  bElastic = 7.9+0.7*log( s/(2.*AMP*AMP));
  b0inv    = 1.0/bElastic;
  b0       = (2.0/3.0)*bElastic;

  ammin    = _ammin.value();
  if (_ammax.value()<0.) {
    ammax=sqrt(.15)*tecm;
    std::cout << "setting mmax=" << ammax << std::endl;
    }
  else {
    ammax=_ammax.value();
  }

  //  check if the parameters are consistent

  int ierc=0;

  if (pbeam<0.0) ierc=ierc+1;
  if(ammin<sqrt(1.5)|| ammin>tecm|| ammin>ammax) {
    ierc=ierc+1;
  }
  if (ammax<sqrt(1.5)||ammax>tecm) ierc=ierc+1;
  if (rprot < 0.0)      ierc=ierc+1;
  if (rneutr< 0.0)      ierc=ierc+1;
  if (rprot+rneutr==0.0)	ierc=ierc+1;
  if (isdp==1&&isdpb==1)	ierc=ierc+1;

  if (ierc!=0) {
    std::cout << " MBR init : error in input parameters " << std::endl;
    return;
  }

  pnrat=rprot/(rprot+rneutr);

  ammin2=ammin*ammin;
  ammax2=ammax*ammax;


//  compute the normalized cross sections for each kind of event.
//  first compute the cross sections for this combination of
//  input parameters and compute the not-normalized cross section
//  for diffractive mass generation SGDRAT
//

  for(int j=0;j<4;j++){ 
    sgrat[j]=0.;
  }

  for(int j=0;j<5;j++){
    sgdrat[j]=0.;
  }

  if(ammax-ammin<1.e-6) {
    sgdrat[0]=1.0;
    cmlim=ammin;
  }
  else {

    cmlim = (ammin2>1.8) ? sqrt(ammin2):sqrt(1.8);
    
    if(icont==1) {
      sgdrat[0]=1.0;
    }
    else {
      if(ammin2<=2.0&&ammax2<=2.5) {
        sgdrat[1]=.09;
      }
      if(ammin2<=2.6&&ammax2<=3.5) {
        sgdrat[2]=.156;
      }
      if(ammin2<=4.0&&ammax2<=5.0) {
        sgdrat[3]=.086;
      }

      if(ammin2<1.8) {
        rclim2 = (ammax2 < 1.8) ? ammax2 : 1.8;
        sgdrat[4]=.113*(rclim2-ammin2)/0.3;
        // The first resonance has to be extended to this region
        sgdrat[1]=sgdrat[1]+.0565;
      }
      if(ammax2>1.8) {
        sgdrat[0]=2.*0.68*(1.+36./s)*log(ammax/cmlim);
      }
    }
  }


  // The s dependence of the cross-sections was changed 
  //  to agree with a recent fit to the highest available data
  //  see CDF NOTE # 675    June 1988

  double a= 0.68;
  double b=36.00;

  // single diffractive cross-section

  sigsd=a*(1.+b/s)*log(0.6+0.1*s);      
  szero=pow(50.0,2);

  fact=pow(log(sqrt(s/szero)),2);

  alpha=0.175+0.015*fact/(1.+0.2*fact);
  double  k=1.3;

  if(sqrt(s) <= 275.0) {
    sig=26.3+2.*sigsd;
    aa=1./(2.*(1-alpha));
    bb=sig+sqrt(sig*sig+4.*k*((1.-alpha)/alpha)*sigsd*sigsd);
    sigtot=aa*bb;     // total cross-section 
    sig0=26.3;        // hard core cross-section  
  }
  else {
    sigtot=2.*sigsd*(1.+1./sqrt(2.*alpha))/(1.-2.*alpha);
    sig0=0.5*sigtot;
  }

  sigel=sigtot*alpha;           // Elastic cross-section
  sigdd=k*sigsd*sigsd/sigel;    // Double diffractive cross-section
    
  sigres=0.332;
  sigres=2.*sigres;
  sigsd =2.*sigsd;              // The Single cross-section for the collision
                                // is twice that of a single vertex


///////////////////////////////////////////////////////////////

  if (_ddvers.value()+_sdvers.value() > 0) {

  double sigma0=_sigpomp.value();
  double eps=_epsilon.value();
  double c0=sigma0*sigma0/(16.*3.14159*0.38938);
        //(sigma0(Pom-p)/4*sqrt(pi)*hbar*c)**2
  double alph=_alpha.value();
  double c1, step, fmin, fmax;

  if (_ddvers.value()==1) {

    double alph2=alph*alph;
    double lsm=log(s/pow(ammin,4));
    double dymax=lsm;

//// DD gap probability normalization
    double dymin=2.3;
    c1=c0/(2.*alph*sigma0);
    step=(dymax-dymin)/1000000.;
    fmax=0.;
    if (fabs(dymin)>0.01) {
      fmin=c1*exp(2.*eps*dymin)*(lsm-dymin)*
           (exp(-2.*alph*dymin*exp(-dymin))
           -exp(-2.*alph*dymin*exp(dymin)))/dymin;
    }
    else {
      fmin=c1*exp(2.*eps*dymin)*lsm*(dymin*(4.*alph)+
           dymin*dymin*(-8.*alph2)+pow(dymin,3)*(2.*alph/3.+8.*alph2*alph))
           -c1*exp(2.*eps*dymin)*(exp(-2.*alph*dymin*exp(-dymin))
              -exp(-2.*alph*dymin*exp(dymin)));
    }
    double fluxdd=step*(fmin+fmax)/2.;
    for(int i=0;i<999999;i++){
      double dy=dymin+(i+1)*step;
      double f;
      if (dy>0.01) {
        f=step*c1*exp(2.*eps*dy)*(lsm-dy)*
          (exp(-2.*alph*dy*exp(-dy))-exp(-2.*alph*dy*exp(dy)))/dy;
      }
      else {
        f=step*c1*exp(2.*eps*dy)*lsm*(dy*(4.*alph)+
           dy*dy*(-8.*alph2)+pow(dy,3)*(2.*alph/3.+8.*alph2*alph))
           -step*c1*exp(2.*eps*dy)*(exp(-2.*alph*dy*exp(-dy))
              -exp(-2.*alph*dy*exp(dy)));
      }
      fluxdd=fluxdd+f;
    }
    if (fluxdd<1.) {
      fluxdd=1.;
    }

    c1=c0*pow(s,eps)/(2.*alph);
    dymin=_dymin.value();
    if (fabs(dymin)>0.01) {
      fmin=c1*(lsm-dymin)*exp(eps*dymin)*
           (exp(-2.*alph*dymin*exp(-dymin))
           -exp(-2.*alph*dymin*exp(dymin)))/dymin;
    }
    else {
      fmin=c1*lsm*exp(eps*dymin)*(dymin*(4.*alph)+
           dymin*dymin*(-8.*alph2)+pow(dymin,3)*(2.*alph/3.+8.*alph2*alph))
           -c1*exp(eps*dymin)*(exp(-2.*alph*dymin*exp(-dymin))
              -exp(-2.*alph*dymin*exp(dymin)));
    }
    fmax=c1*(lsm-dymax)*exp(eps*dymax)*
         (exp(-2.*alph*dymax*exp(-dymax))
         -exp(-2.*alph*dymax*exp(dymax)))/dymax;
    step=(dymax-dymin)/1000000.;
    sigdd=step*(fmin+fmax)/2.;
    ddpmax=0;
    for(int i=0;i<999999;i++){
      double dy=dymin+(i+1)*step;
      double f;
      if (dy>0.01) {
        f=(lsm-dy)*exp(eps*dy)*(exp(-2.*alph*dy*exp(-dy))
          -exp(-2.*alph*dy*exp(dy)))/dy;
      }
      else {
        f=lsm*exp(eps*dy)*(dy*(4.*alph)+
           dy*dy*(-8.*alph2)+pow(dy,3)*(2.*alph/3.+8.*alph2*alph))
           -exp(eps*dy)*(exp(-2.*alph*dy*exp(-dy))
              -exp(-2.*alph*dy*exp(dy)));
      }
      sigdd=sigdd+step*c1*f;
      if (f>ddpmax) {ddpmax=f;}
    }
    sigdd=sigdd/fluxdd;
  }



  if (_sdvers.value()==1) {
    c0=pow(6.566,2)/(16.*3.141592654);
    double a1=0.9;
    double a2=0.1;
    double b1=4.6;
    double b2=0.6;
    double ximin=_ximin.value();
    if (ximin<=0.) {
      ximin=1.5/s;
      std::cout << "setting ximin=" << ximin << std::endl;
    }
    double ximax=_ximax.value();

// SD flux integral
    double ximaxflux=0.1;
    double xmin=eps*(-log(ximaxflux)+b1/(2.*alph));
    double xmax=eps*(-log(ximin)+b1/(2.*alph));
    c1=c0/(2.*alph)*exp(-eps*b1/alph);
    fmin=c1*exp(2.*xmin)*((a1/xmin)+a2/(xmin-eps*(b1-b2)/(2.*alph)));
    fmax=c1*exp(2.*xmax)*((a1/xmax)+a2/(xmax-eps*(b1-b2)/(2.*alph)));
    step=(xmax-xmin)/1000000.;
    double fluxsd=step*(fmin+fmax)/2.;
    for(int i=0;i<999999;i++){
      double x=xmin+(i+1)*step;
      fluxsd=fluxsd+step*c1*exp(2.*x)*((a1/x)+a2/(x-eps*(b1-b2)/(2.*alph)));
    }
    if (fluxsd<1.) {
      fluxsd=1.;
    }

    c1=c0*sigma0/(2.*alph)*exp(-eps*b1/(2.*alph))*(pow(s,eps)/fluxsd);
    xmin=eps*(-log(ximax)+b1/(2.*alph));
    fmin=c1*exp(xmin)*((a1/xmin)+a2/(xmin-eps*(b1-b2)/(2.*alph)));
    fmax=c1*exp(xmax)*((a1/xmax)+a2/(xmax-eps*(b1-b2)/(2.*alph)));
    step=(xmax-xmin)/1000000.;
    sigsd=step*(fmin+fmax)/2.;
    for(int i=0;i<999999;i++){
      double x=xmin+(i+1)*step;
      sigsd=sigsd+step*c1*exp(x)*((a1/x)+a2/(x-eps*(b1-b2)/(2.*alph)));
    }

    sigsd=2.*sigsd;
  }

    sig0=sigtot-(sigsd+sigdd+sigel);

  }

//////////////////////////////////////////////////////////////////////


  // now decide which of them must be taken into account
  
  for (int i=0;i<4;i++){
    sgrat[i]=0;
  }
   
  if (HardCore)            sgrat[0]=sig0;
  if (DoubleDiffractive)   sgrat[1]=sigdd;
  if (SingleDiffractive)   sgrat[2]=sigsd;
  if (ElasticScattering)   sgrat[3]=sigel;


  // Normalize cross section for different processes

  ratsum=0;
  for(int i=0;i<4;i++){
    ratsum+=sgrat[i];
  }
  if(ratsum==0) {
    std::cout << " MBR init: Nothing to generate " << std::endl;
    return;
  }
  else {
    for(int i=0;i<4;i++){
      sgrat[i]/=ratsum;
    }
    rate[0]=sgrat[0];
    rate[1]=rate[0]+sgrat[1];
    rate[2]=rate[1]+sgrat[2];
    rate[3]=rate[2]+sgrat[3];
  }


  // Normalize mass generation cross section

  ratsum=sgdrat[0]+sgdrat[1]+sgdrat[2]+sgdrat[3]+sgdrat[4];
  for(int i=0;i<5;i++){
    sgdrat[i]/=ratsum;
  }

  drate[0]=sgdrat[0];
  drate[1]=drate[0]+sgdrat[1];
  drate[2]=drate[1]+sgdrat[2];
  drate[3]=drate[2]+sgdrat[3];
  drate[4]=drate[3]+sgdrat[4];

  if(HardCore==1)          std::cout << " Inelastic scattering selected " << std::endl;
  if(SingleDiffractive==1) std::cout << " Single diffraction   selected " << std::endl;
  if(DoubleDiffractive==1) std::cout << " Double diffraction   selected " << std::endl;
  if(ElasticScattering==1) std::cout << " Inelastic scattering selected " << std::endl;

  print();
  std::cout << " MBR initialization complete " << std::endl;
  return;
}

//_____________________________________________________________________________
void  MinBiasModule::mbr_generate_event (void){

  //**********************************************************
  //
  // This is the main subroutine for event generation, which is
  // called by the standard CDF routine CDFGEN. It is obviously
  // a small modification of MB1 GENEV routine.
  //
  //==========================================================
  //
  //   Author:     S. Belforte
  //                                 June 1984
    
  mssflg=0;  mulflg=0; iknflg=0;


  // Clear current event type flags
  inelasticEvent=0;
  singleDiffractiveEvent=0;
  doubleDiffractiveEvent=0;
  elasticEvent=0;
  
  CdfHepevt* hepevt = CdfHepevt::Instance();
  HepRandomEngine* engine = MinBiasModule::minBiasEngine;

  int ierr=1;

  hepevt->HepevtPtr()->NHEP=0;
  hepevt->HepevtPtr()->NEVHEP=++nmbr;

  //std::cout << " MBR: Generate events   " << nmbr << std::endl;

  double rnd = RandFlat::shoot(engine);
  if(rnd<=rate[0]) {
    nevhc++;
    //std::cout << " MBR: Generate hard core event  " << nevhc << std::endl;
    inelasticEvent=1;
    ierr=hard_core_event();
  }    
  else if(rnd<=rate[1]) {
    nevddf++;
    doubleDiffractiveEvent=1;
    // std::cout << " MBR: Generate double diffractive  event " <<nevddf << std::endl;
    ierr=double_diffractive_event();
  }
  else if(rnd<=rate[2]) {
    nevsdf++;
    singleDiffractiveEvent=1;
    //std::cout << " MBR: Generate single diffractive  event " <<nevsdf << std::endl;
    ierr=single_diffractive_event();
  }
  else { 
    nevela++;
    elasticEvent=1;
    //std::cout << " MBR: Generate elastic scattering event  " <<nevela<< std::endl;
    ierr=elastic_event();
  }	

  if (ierr!=0) {
    std::cout << "MBR::generate_event: sever error  stop the program " << std::endl;
    return;
  }

  //  test error flags for this event
  if (mssflg==1||mulflg==1||iknflg==1) never++;
  if (mssflg==1) nevmsg++;
  if (mulflg==1) nevmlg++;
  if (iknflg==1) nevkng++;

  generate_lorentz_boost(pcmlab);

  double v1[4],v2[4];

  int numberOfParticles=hepevt->HepevtPtr()->NHEP;

  for (int i=0;i<numberOfParticles;i++){
					// kludge for EGCS - still need it
    double* hp = hepevt->HepevtPtr()->PHEP[i];     
    for (int j=0;j<4;j++) {
      v1[j]=hp[j];
    }
    add_lorentz_boost(pcmlab,v1,v2);

    for (int j=0;j<4;j++) {
      hp[j]=v2[j];
    }
  }


  if((nmbr%100)==0) {
    std::cout << std::setw(6) << " MBR: " <<
                 std::setw(6) << numberOfParticles <<
                 std::setw(35)<< " particle generated in event # "<< 
                 std::setw(6) << nmbr   <<
                 std::setw(6) << nevhc  <<
                 std::setw(6) << nevsdf <<
                 std::setw(6) << nevddf <<
                 std::setw(6) << nevela <<  std::endl;
  }
  return;
}


//_____________________________________________________________________________

int  MinBiasModule::hard_core_event(void){

  //**********************************************************
  //
  // This subroutine generates a hard core event, i.e.
  // it treat the whole system p+pbar as a unique
  // mass of sqrt(s)Gev at rest in the event center
  // of mass and invoke the routine FRAGMX to decay
  // it in exactly the same way used for diffractive
  // events.
  //
  //   Author:     S. Belforte  June 1984
  //----------------------------------------------------------
  //  OUTPUT:
  //         IERR :  I*4 is the error return code:
  //	      	      0 = successfull calling
  //		      1 = unable to decay the mass probable
  //			  error in input parameters
  //================================================================

  CdfHepevt* hepevt = CdfHepevt::Instance();

  double p[4]={0.0,0.0,0.0,tecm};
  add_particle_HepEvt(MBCLST,0,0,3,p,tecm);


  //  PXCM is the beta of the lorentz transformation from cm-x ref system 
  //  to event cm system

  double pxcm[4]={0.0,0.0,0.0,1.0};


  int icharg=0;
  int ngenmx=hepevt->HepevtPtr()->NHEP;

  //  std::cout << " call fragment cluster " << std::endl;
  int ierr=fragment_cluster(ngenmx,pxcm,icharg);

  //if (ierr!=0) {
    //std::cout << " MBR::HardCore: Fatal error, error code = 1 " << std::endl; 
  //}

  return ierr;
}
//_____________________________________________________________________________

int  MinBiasModule::double_diffractive_event(void){

  //**********************************************************
  //
  // This subroutine generates a double diffractive event, i.e.
  // it turns both the proton and the antiproton into diffractive
  // masses AM1 and AM2 generated indipendently by GENMAS and
  // give them  a transverse momentum by the routine GTDDIF
  // (Gen. T Double DIFfr.).
  // The azimuthal angle PHI for the scatter is generated at
  // random in [0,2pi]. Then the subroutine FRAGMX is invoched
  // to fragment the diffracted masses. If this routine results
  // unable to fragment Mx the program tries again with a
  // new value for AM1 AM2, till it exceeded the maximun number
  // of trials for each event.
  //
  //   Author:     S. Belforte  June 1984
  //----------------------------------------------------------
  //  OUTPUT:
  //
  //         IERR :  I*4 is the error return code:
  //	      	      0 = successfull calling
  //		      1 = unable to generate a double
  //			  diffractive event, probable
  //			  error in input parameters
  //   
  //================================================================

  double   e1,e2,p2,xf,pt,px,py,pz;
  double   phi;
  int maxtry=10;
  CdfHepevt* hepevt = CdfHepevt::Instance();
  HepRandomEngine* engine = MinBiasModule::minBiasEngine;

  // Initialization: reset error code and trial counter

  int irept=0;
  int ngen0=hepevt->HepevtPtr()->NHEP;

  //  start of program body

  double am1,am2,t,pt2;

  int ierr=1;
  while(ierr!=0) {

    if(irept++>maxtry) {
      return 1;
    }

////////////////////////////////////////////////////////
    if (_ddvers.value()==1) {
    pt2=-1.0;
    while(pt2<=0.0) {

      double eps=_epsilon.value();
      double alph=_alpha.value();
      double alph2=alph*alph;
      double m04=pow(ammin,4);
      double s=tecm*tecm;
      double dymax=log(s/(m04));
      double dymin=_dymin.value();

      double P,y,dy;

      do {
          dy=dymin+(dymax-dymin) * RandFlat::shoot(engine);
          if (dy>0.01)
            P=(log(s/m04)-dy)*exp(eps*dy)*(exp(-2.*alph*dy*exp(-dy))
              -exp(-2.*alph*dy*exp(dy)))/dy;
          else
            P=log(s/m04)*exp(eps*dy)*(dy*(4.*alph)+
               dy*dy*(-8.*alph2)+pow(dy,3)*(2.*alph/3.+8.*alph2*alph))
               -exp(eps*dy)*(exp(-2.*alph*dy*exp(-dy))
              -exp(-2.*alph*dy*exp(dy)));
          if (P>ddpmax) std::cout << "WARNING double_diffractive_event: ddpmax="
            << ddpmax << " " << P << " " << dy;
          y=ddpmax * RandFlat::shoot(engine);
         }
      while (y>P);

      double y0max=(log(s/m04)-dy)/2.;
      double y0min=-y0max;
      double y0=y0min+(y0max-y0min) * RandFlat::shoot(engine);

      am1=sqrt(tecm*exp(-y0-dy/2.));
      am2=sqrt(tecm*exp(y0-dy/2.));

      double b=2.*alph*dy;
      do {t=(1./b)*log( RandFlat::shoot(engine));}
      while (-t>exp(dy));

      e1=.5*(tecm+(am1*am1-am2*am2)/tecm);
      e2=tecm-e1;
      p2=e1*e1-am1*am1;
      xf=1.-(am1*am1-AMP*AMP)/s;

      pt2=xf*fabs(t)-pow((AMP*(1.-xf)),2);

    }
    }
    else {
      am1=generate_diffractive_mass();
      am2=generate_diffractive_mass();
      // std::cout << "Double diffraction " << am1 << "  " << am2 << std::endl;
      e1=.5*(tecm+(am1*am1-am2*am2)/tecm);
      e2=tecm-e1;
      p2=e1*e1-am1*am1;
      xf=1.-(am1*am1-AMP*AMP)/s;
      pt2=-1.0;
      while(pt2<=0.0) {
        t=generate_double_diffractive_t();    // always t=1.0; Loop for future upgrade anwar
        pt2=xf*fabs(t)-pow((AMP*(1.-xf)),2);  //
      }
      }

//////////////////////////////////////////////////////////////

    pt=sqrt(pt2);
    pz=sqrt(p2-pt2);

    phi=RandFlat::shoot(engine)*twopi;
    px=pt*cos(phi);
    py=pt*sin(phi);

    // proton diffraction (recoil #1) and antiproton diffraction (recoil #2)

    double p1[4]={ px, py, pz,sqrt(px*px+py*py+pz*pz+am1*am1)};
    double p2[4]={-px,-py,-pz,sqrt(px*px+py*py+pz*pz+am2*am2)}; //px->pz
  
    hepevt->HepevtPtr()->NHEP=ngen0;
    add_particle_HepEvt(MBCLST,0,0,3,p1,am1);
    int cluster1=hepevt->HepevtPtr()->NHEP;

    add_particle_HepEvt(MBCLST,0,0,3,p2,am2);
    int cluster2=hepevt->HepevtPtr()->NHEP;

    //  P1CM and P2CM are the betas of the lorentz transformations from
    //  cm-x ref. system to event cm system

    double p1cm[4]= {-px/e1,-py/e1,-pz/e1,e1/am1};
    double p2cm[4]= {+px/e2,+py/e2,+pz/e2,e2/am2};

    // std::cout <<  "diffracted systems decay " << t << " "<< pt2 << " " <<pz << std::endl;

    ierr=fragment_cluster(cluster1,p1cm,1);
    if(ierr==0) {
      ierr=fragment_cluster(cluster2,p2cm,-1);
    }
  }
  //  if(irept>1) {
  //    std::cout <<" MBR: generate_double_diffraction tries "<<irept<<" tries."<< std::endl;
  //  }
  return 0;
}

//_____________________________________________________________________________

int MinBiasModule::single_diffractive_event(void){

  //**********************************************************
  //
  // This subroutine generates a single diffractive event, i.e.
  // it decide at random (50% prob. each) to scatter quasi
  // elastically the proton or the antiproton, than turns the
  // other colliding particle into a diffracted mass MX (generated
  // by the subroutine GENMAS) and assign it a transverse
  // momentum by the routine GPTSDF (Gen. PT Single DiFfr.).
  // The azimuthal angle PHI for the scatter is generated at
  // random in [0,2pi]. Then the subroutine FRAGMX is invoched
  // to fragment the diffracted mass. If this routine results
  // unable to fragment Mx the program tries again with a
  // new value for Mx, till it exceeded the maximun number
  // of trials for each event.
  //==========================================================
  //
  //   Author:     S. Belforte
  //                                 June 1984
  //  OUTPUT:
  //
  //         IERR :  I*4 is the error return code:
  //	      	      0 = successfull calling
  //		      1 = unable to generate a single
  //			  diffractive event, probable
  //			  error in input parameters
  //   

  CdfHepevt* hepevt = CdfHepevt::Instance();
  HepRandomEngine* engine = MinBiasModule::minBiasEngine;

  int maxrpt=10;

  int ngen0=hepevt->HepevtPtr()->NHEP;

  int irept=0;
  int ierr=1;

  while(ierr!=0) {
    irept++;
    if(irept>=2) {
      mssflg=1;
      iermsg++;
    }

    if (irept>=maxrpt) {
      std::cout << " MBR SnglDiff " << irept << std::endl;
      return 1;       
    }

    //  choose proton or antiproton to scatter quasi-elastically
    //  ICHARG stores  the charge of Mx.


    int iquas,icharg,psrec;
    if(RandFlat::shoot(engine)>=0.5) {
       //  --- proton is scattered quasi-elastically
      iquas=1;
      icharg=-1;
      psrec=PSPRO;
    }
    else {
       //  --- anti-proton is scattered quasi-elastically
      iquas=2;
      icharg=1;
      psrec=PSPROB;
    }

    //
    // see if user overwrote this decision by forcing diffraction of the
    // proton (antiproton) only, and thus scatter for the antiproton (proton)
    //
    if(isdp==1) {
      iquas=2;
      icharg=1;
      psrec=PSPROB;
    }
    else {
      if(isdpb==1){
        iquas=1;
        icharg=-1;
        psrec=PSPRO;
      }
    }

    //   mass of diffracted system is generated according to the general
    //   distribution  1/M**2 in the interval 2.0 GeV - TECM and stored
    //   in EVSP common as information for the simulator

//////////////////////////////////////////////////

    double amx, t, xf, ptsqr;

    if (_sdvers.value()==1) {

      double eps, alph, ximin, ximax, s;

      eps=_epsilon.value();
      alph=_alpha.value();
      s=tecm*tecm;
      ximin=_ximin.value();
      if (ximin<=0.) {
        ximin=1.5/s;
      }
      ximax=_ximax.value();

      double xi,P,y;

      ptsqr=-1.0;
      do {
        do {
          xi=ximin*exp((log(ximax)-log(ximin))*RandFlat::shoot(engine));
          P=pow(xi,-1.-eps)*(0.9/(4.6+2.*alph*log(1./xi))+
                             0.1/(0.6+2.*alph*log(1./xi)));
          y=(1./xi)*RandFlat::shoot(engine);
        }
        while (y>P);

        amx=sqrt(xi*s);

        double tmin=-AMP*AMP*xi*xi/(1-xi);
        do {
          t=tmin+log(1.- RandFlat::shoot(engine));
          P=pow((4.*AMP*AMP-2.8*t)/((4.*AMP*AMP-t)*pow(1.-t/0.71,2)),2)
            *exp(-2.*alph*log(xi)*t);
          y=exp(t)*RandFlat::shoot(engine);
        }
        while (y>P);
        xf=1.-(amx*amx-AMP*AMP)/s;
        ptsqr=xf*fabs(t)-pow((AMP*(1.-xf)),2);
      } while (ptsqr<=0.0);
    }

    else {
      amx=generate_diffractive_mass();

      t=0.0;
      //  generate the transferred squared momentum t
      // for the diffracted mass
      ptsqr=-1.0;
      do {
        t=generate_single_diffractive_t(amx);
        xf=1.-(amx*amx-AMP*AMP)/s;
        ptsqr=xf*fabs(t)-pow((AMP*(1.-xf)),2);
      } while (ptsqr<=0.0);
    }

//////////////////////////////////////////////////////////////

    double erec=.5*(tecm-(amx*amx-AMP*AMP)/tecm);
    double psqr=erec*erec-AMP*AMP;
    double ex=tecm-erec;
    double pt=sqrt(ptsqr);
    double pz=sqrt(psqr-ptsqr);

    // PZ is positive in the direction on the proton beam (standard CDF reference system)
    if (iquas==2){
      pz=-pz;
    }

    double phi=RandFlat::shoot(engine)*twopi;
    double px=pt*cos(phi);
    double py=pt*sin(phi);


    //    std::cout << " MBR : SingleDiff : MX ="  << amx << " t=" << t << std::endl;
     
    //  quasi elastically scattered particle (recoil)

    double mass=pamass(psrec);
    double p1[4]={+px,+py,+pz,sqrt(px*px+py*py+pz*pz+mass*mass)};
    double p2[4]={-px,-py,-pz,sqrt(px*px+py*py+pz*pz+amx*amx)};

    hepevt->HepevtPtr()->NHEP=ngen0;
					// status code for the stable 
					// particle is 1

    //    add_particle_HepEvt(psrec,0,0,100+iquas,p1,pamass(psrec));

    add_particle_HepEvt(psrec,0,0,1,p1,pamass(psrec));
    add_particle_HepEvt(MBCLST,0,0,3,p2,amx);

    int ngenmx=hepevt->HepevtPtr()->NHEP;

    //PXCM is the beta of the lorentz transformation from cm-x ref.system to event cm system

    double pxcm[4]={px/ex,py/ex,pz/ex,ex/amx};
    ierr=fragment_cluster(ngenmx,pxcm,icharg);
  }
  return ierr;
}

//_____________________________________________________________________________

int  MinBiasModule::elastic_event(void){

  //**********************************************************
  //
  // This subroutine generates a p-pbar pair according
  // to elastic angular distribution: DS/DT=EXP(-BT),
  // from which the theta angle is derived. Phi is uniform
  // in 0-2pi. The value for B comes from a formula from
  // K.Goulianos: Belastic = 7.9 + 0.7 *  ln( S/(2*Mp**2) ),
  // S being (center of mass energy)**2 and Mp the proton mass.
  //
  //   Author:     S. Belforte
  //                                 November 15, 1985
  //  OUTPUT:
  //
  //         IERR :  I*4 is the error return code:
  //	      	      0 = successfull calling
  //  


  CdfHepevt* hepevt = CdfHepevt::Instance();
  HepRandomEngine* engine = MinBiasModule::minBiasEngine;

  // generate t according to the exponential law in the interval 0-infinity

   double arg=0.0;
   do {
     arg=1.0-RandFlat::shoot(engine);
   } while(arg==0.0);

   double t=fabs(b0inv*log(arg));

   double costh=1.0-t/(2*pbeam*pbeam);
   double sinth=sqrt(t/(pbeam*pbeam)-1.0/4.0*pow((t/(pbeam*pbeam)),2));
   double phi=RandFlat::shoot(engine)*TWOPI;

   double pz=pbeam*costh;
   double px=pbeam*sinth*cos(phi);
   double py=pbeam*sinth*sin(phi);

   double mass=pamass(PSPRO);
   double p1[4]={px,py,pz,sqrt(px*px+py*py+pz*pz+mass*mass)};

   mass=pamass(PSPROB);
   double p2[4]={-px,-py,-pz,sqrt(px*px+py*py+pz*pz+mass*mass)};

				// declare proton and antiproton as final
				// particles (status code = 1)

   add_particle_HepEvt(PSPRO ,0,0,1,p1,pamass(PSPRO));
   add_particle_HepEvt(PSPROB,0,0,1,p2,pamass(PSPROB));

   return 0;
}

//_____________________________________________________________________________

void  MinBiasModule::generate_lorentz_boost(double pcmlab[4]){
  //**********************************************************
  //
  //  PCMLAB[4] : R*4 is the (beta,gamma) vector for the boost

   pcmlab[0]=0.0;
   pcmlab[1]=0.0;
   pcmlab[2]=0.0;
   pcmlab[3]=1.0;

}

//_____________________________________________________________________________

double  MinBiasModule::generate_diffractive_mass(void){

  //**********************************************************
  //
  // This routine generates a diffracted mass AMX according
  // to a continuum distribution 1./m**2 in the input mass**2
  // interval plus the eventual resonance contribution. To avoid
  // unnecessary complications the mass distribution will not be
  // the naive expected one when the input mass interval
  // [ammin,ammax] is a narrow one in the resonance region,
  // anyway the mass will always fall inside that interval.
  //
  //==========================================================
  //
  //   Author:     S. Belforte
  //                                September 1984
  //
  //================================================================

 
  double amx=0.0; 
  double amx2=0.0;
  int MXREP=20;
  int irep=0;

  HepRandomEngine* engine = MinBiasModule::minBiasEngine;

  double rnd=RandFlat::shoot(engine);

  if(rnd<drate[0]) {
    //  continuum spectrum
    double rnd=RandFlat::shoot(engine);
    if(epsilon==0) {
      amx=cmlim*pow((ammax/cmlim),rnd);
    }
    else {
      amx=cmlim*ammax/pow(((1.-rnd)*pow(ammax,(2.*epsilon))+
          rnd*pow(cmlim,(2.*epsilon)) ),(1./(2.*epsilon)));

    }
  }
  else if(rnd < drate[1]) {
    while (amx2<1.5||amx2>2.5||amx2<ammin2||amx2>ammax2) {
      amx2=RandGaussT::shoot(engine,2.2,.3);
      if(irep++>MXREP) return 2.2;
    }
    amx=sqrt(amx2);
  }
  else if(rnd < drate[2]) {
    while (amx2<2.5||amx2>4||amx2<ammin2||amx2>ammax2) {
      amx2=RandGaussT::shoot(engine,2.8,.3);
      if(irep++>MXREP) return 2.8;
    }
    amx=sqrt(amx2);
  }
  else if(rnd < drate[3]) {
    while (amx2<3||amx2<ammin2||amx2>ammax2) {
      amx2=RandGaussT::shoot(engine,4.4,.8);
      if(irep++>MXREP) return 4.4;
    }
    amx=sqrt(amx2);
  }
  else if(rnd<drate[4]) {
    //  initial rectangle
    amx2=ammin2+(rclim2-ammin2)*RandFlat::shoot(engine);
    amx=sqrt(amx2);
  }
  return amx;
}

//_____________________________________________________________________________

void  MinBiasModule::add_lorentz_boost(double beta[4], double v1[4], double v2[4]){ 
  //**************************************************
  //*
  //**************************************************

  double bp=0;
  for(int i=0;i<3;i++) {
    bp=bp+beta[i]*v1[i];
  }

  double bpp=(bp*beta[3]/(beta[3]+1.)-v1[3])*beta[3];   

  v2[0]=v1[0]+beta[0]*bpp;   
  v2[1]=v1[1]+beta[1]*bpp;  
  v2[2]=v1[2]+beta[2]*bpp;   
  v2[3]=beta[3]*(v1[3]-bp);  

}

//_____________________________________________________________________________

double  MinBiasModule::generate_single_diffractive_t(double amx){

  //**********************************************************
  //
  // This subroutine generates the t for a single diffractive
  // event. The distribution used is a phenomelogical one
  // intended to reproduce experimental exixting data.
  //
  //   Author:     S. Belforte
  //===========================================================
  HepRandomEngine* engine = MinBiasModule::minBiasEngine;
  double amo=0.0;
  double tmax=1.0;
  
  double rad2=sqrt(2.0);
  
  double t=0.0;
  double b,binv,facnor;
  if(amx!=amo) {
    b=b0*(1.+.04/(pow((amx-rad2),2)+.02));
    facnor=1.0-exp(-b*tmax);
    binv=1./b;
    amo=amx;
  }
  double rn=RandFlat::shoot(engine);
  t=binv*log(1.0-rn*facnor);
  return t;
}

//_____________________________________________________________________________

double  MinBiasModule::generate_double_diffractive_t(void){
  return 1.0;
}

//_____________________________________________________________________________

int  MinBiasModule::average_multiplicity(double amx){

  //**********************************************************
  //
  // This subroutines generates the total multiplicity for
  // the fragmentation of the current diffractive mass, whose
  // value is fetched form the common variable AMX. The resulting
  // multiplicity is stored in the other common variable NT.
  // The mass actually used for multiplicity generation
  // is the current decaying mass AMX minus the proton mass.
  //   Author:     S. Belforte  July 1984
  //  Modified:    J. Grunhaus  June 1988
  //		   T. Chapin    July 1989
  // average total multiplicity is computed from available mass AMX
  // avntot is the calculated average  multiplicity

  int multi;
  double  am,am2,avntot,avnstar,wmuu;
  int nmax;

  double amxold=0.0;

  if(amx!=amxold) {
    if(amx<1.1) {
      //' AVMUL: fragmenting mass is too low 
      //         it is impossible to generate even',
      //         ' 2 particles',/,'  Program STOPs')
      //stop
      multi=0;
      return multi;
    }
    else {
      am=amx-AMP;
      am2=am*am;
      double tmp=pow(log(am2),2);
      avntot=3.0/2.0*(2.0+0.13*log(am2)+.175*tmp);

      avntot=avntot*multiplicityScaleFactor;
                                  // Scale it if user desires May 5,1999
      if (am<=1.) avntot=2.0;
      avnstar=2./3.*avntot;

      double aa8=-0.104;
      double bb8=0.058;
      double cc8=6.0;
      wmuu=1.0/(aa8+bb8*log(am+cc8));

      nmax=1000;
      if (amx<2.0){ nmax=4;}
      if (amx<1.5){ nmax=3;}
      if (amx<1.3){ nmax=2;}
      amxold=amx;
    }
  }

  //     The total multiplicity is gotten from the subroutine dino
  //     and it is stored in the variable nt. (June 1988, J. GRUNHAUS)
  //  --- total multiplicity must of course be larger then 2 !


  do {  
    multi=total_multiplicity_by_dino(wmuu,avnstar);
  } while (multi <=1 || multi>nmax);
  return multi;
}

//_____________________________________________________________________________

int MinBiasModule::two_body_decay(int ngenmx,int ip1,int ip2){

  //**********************************************************
  //
  // This subroutine fragment a particle into 2 bodies
  // with exact phase space and angular distribution
  // of the first one proportional to cos**2(theta)
  //
  //   Author:     S. Belforte    June 1984
  //----------------------------------------------------------

  //  INPUT:
  //
  //         NGENM : I*4 is the particle number of the system in the
  //                     /MbrEvt/ common
  //         IP1   : I*4 is the particle identifier of the first
  //                     product (the one to be distributed as cos**2)
  //         IP2   : I*4 is the particle identifier of the second body
  //
  //  OUTPUT:
  //
  //         IERR :  I*4 is the error return code:
  //	      	      0 = successfull calling
  //		      1 = particle mass was too low
  //   
  //================================================================

  CdfHepevt* hepevt = CdfHepevt::Instance();    
  HepRandomEngine* engine = MinBiasModule::minBiasEngine;

  // Get particle masses and test parent against product masses

					// kludge for EGCS/SGI
  double* hp = hepevt->HepevtPtr()->PHEP[ngenmx-1];
  double  am0=hp[4];
  double  am1=pamass(ip1);
  double  am2=pamass(ip2);

  if (am0<=(am1+am2)) {
    //	  print 1000,am1+am2,am0
    //(' FRA2B: attempt to fragment into a total mass of Gev a mass of only Gev')
    return 1;
  }

  //  compute energy and momentum for outgoing particles

  double e1=.5*(am0*am0+am1*am1-am2*am2)/am0;
  double p=sqrt(e1*e1-am1*am1);

  double theta = generate_random_o2();
  double phi=twopi*RandFlat::shoot(engine);

  double px=p*sin(theta)*cos(phi);
  double py=p*sin(theta)*sin(phi);
  double pz=p*cos(theta);

  
  double p1[4]={px,py,pz,sqrt(px*px+py*py+pz*pz+am1*am1)};
  double p2[4]={-px,-py,-pz,sqrt(px*px+py*py+pz*pz+am2*am2)};

  //For the decaying particle this must have been already done by caller routine (FRAGMX)
  // old status=103
  // set isthep=1 for final stateparticles
  
  add_particle_HepEvt(ip1,ngenmx,0,stable,p1,am1);
  add_particle_HepEvt(ip2,ngenmx,0,stable,p2,am2);

  return 0;
}

//_____________________________________________________________________________

int MinBiasModule::three_body_decay(int ngenmx,int ip1,int ip2, int ip3){
  //********************************************************** 
  //
  // This subroutine fragment a particle into 3 bodies
  // with exact phase space and angular distribution
  // of the first body proportional to cos**2(theta)
  //
  //  This routine is a modification of routine DALITZ from
  //  MB1 generator of Y. Takaiwa, from where the algorithm
  //  for energy and momentum ripartition among the three
  //  outgoing particles is taken. Any question about this 
  //  part of the code must be addressed to Y.T.
  //   Author:     S. Belforte  June 1984

  //
  //  INPUT:
  //
  //         NGENM : I*4 is the particle number of the system in the 
  //                     /MbrEvt/ common
  //	  IP1   : I*4 is the particl eidentifier of the first
  //                     decay product (the one to be distributed
  //		      according to cos**2(theta))
  //	  IP2   : I*4 particle identifier for the second body
  //         IP3   : I*4 same as above for the third one
  //
  //  OUTPUT:
  //
  //         IERR :  I*4 is the error return code:
  //	      	      0 = successfull calling
  //		      1 = particle mass was too low
  //   
  //================================================================

  CdfHepevt* hepevt = CdfHepevt::Instance();
  HepRandomEngine* engine = MinBiasModule::minBiasEngine;

  int ierr=0;

  // Get particle masses and test total producted mass against initial decaying one

					// kludge for EGCS/SGI
  double* hp = hepevt->HepevtPtr()->PHEP[ngenmx-1];
  double am0=hp[4];
  double am1=pamass(ip1);
  double am2=pamass(ip2);
  double am3=pamass(ip3);

  if (am0<=(am1+am2+am3)) {
    //print 1000,am1+am2+am3,am0
    //' FRA3B: attempt to fragment into a total mass of Gev a mass of only)
    ierr=1;
    return ierr;
  }

  //  Generate energy and momentum for outgoing particles 1 and 2
  //  for which they can be chosen indipendentely

  double dmx1=0.5*(pow(am0,2)-pow((am2+am3),2)+pow(am1,2))/am0-am1;
  double dmx2=0.5*(pow(am0,2)-pow((am1+am3),2)+pow(am2,2))/am0-am2;

  double pm1,pm2;
  double cost2=-2.0;

  while (cost2<-1.||cost2>1.) {
    double e1=RandFlat::shoot(engine)*dmx1+am1;
    double e2=RandFlat::shoot(engine)*dmx2+am2;
    pm1=sqrt(e1*e1-am1*am1);
    pm2=sqrt(e2*e2-am2*am2);

    cost2=(am0*am0+am1*am1+am2*am2-am3*am3-2.*(e1*am0+e2*am0-e1*e2))/(2.*pm1*pm2);
  }

  double sint2=sqrt(1.-cost2*cost2);

  //  generate THETA,PHI,PSI for the nucleon

  double theta =generate_random_o2();
  double phi=twopi*RandFlat::shoot(engine);
  double psi=twopi*RandFlat::shoot(engine);

  //  fix the kinematics

  double p1[4],p2[4],p3[4];

  p1[0]=pm1*sin(theta)*cos(phi);
  p1[1]=pm1*sin(theta)*sin(phi);
  p1[2]=pm1*cos(theta);
  p1[3]=sqrt(p1[0]*p1[0]+p1[1]*p1[1]+p1[2]*p1[2]+am1*am1);

  p2[0]=pm2*sint2;
  p2[1]=0.0;
  p2[2]=pm2*cost2;
  rotate_three_vector(phi,theta,psi,p2[0],p2[1],p2[2]);
  p2[3]=sqrt(p2[0]*p2[0]+p2[1]*p2[1]+p2[2]*p2[2]+am2*am2);
  
  for(int i=0;i<3;i++){
    p3[i]=-p1[i]-p2[i];
  }
  p3[3]=sqrt(p3[0]*p3[0]+p3[1]*p3[1]+p3[2]*p3[2]+am3*am3);


  add_particle_HepEvt(ip1,ngenmx,0,stable,p1,am1);
  int current=hepevt->HepevtPtr()->NHEP;

  int* hd = hepevt->HepevtPtr()->JDAHEP[ngenmx-1];
  hd[0]   = current;

  add_particle_HepEvt(ip2,ngenmx,0,stable,p2,am2);
  add_particle_HepEvt(ip3,ngenmx,0,stable,p3,am3);
  current = hepevt->HepevtPtr()->NHEP;
  hd[1]   = current;

  ierr=0;
  return ierr;
}


//_____________________________________________________________________________

void  MinBiasModule::generate_one_particle_from_fire_ball(int ngenpa, int itype,int nt){

  //**********************************************************
  //
  // This subroutine generates from an hadronic fire-ball
  // one particle ( specie ITYPE in the standard CDF code).
  // The parent fire-ball number must be NGENPA in common
  // /GENP/. The subroutine also updates that common with
  // the new particle information.
  // The particle rapidity and transverse momentum are generated
  // in a totally uncorrelated way by means of the routines
  // GENETA and GENPT.
  //
  //   Author:     S. Belforte    June 1984
  //   Modified:   J. Grunhaus    June 1988
  //----------------------------------------------------------
  // INPUT:   NGENPA: I*4 is the parent particle number in the /GENP/ common
  //          ITYPE : I*4 is the type code of the particle to  be generated
  //-------------------------------------------------------

  CdfHepevt* hepevt = CdfHepevt::Instance();
  HepRandomEngine* engine = MinBiasModule::minBiasEngine;

  double* hp  = hepevt->HepevtPtr()->PHEP[ngenpa-1];
  double  amx = hp[4];

  double y=generate_rapidiy(amx);
  double pt=generate_pt(amx,nt);
  double phi=twopi*RandFlat::shoot(engine);
  double mass=pamass(itype);

  //  compute kinematical variables using rapidity,pt and mass

  double px=pt*cos(phi);
  double py=pt*sin(phi);

  double pz;

  if(y==0.0){
    pz = 0.0;
  }
  else {
    double top=pt*pt+mass*mass;
    double bota=(1.+exp(2.*y))/(exp(2.*y)-1.);
    bota=bota*bota;
    double pzsq=(top/(bota-1.));
    pz=sqrt(pzsq)*y/fabs(y);
  }


  double p[4]={px,py,pz,sqrt(px*px+py*py+pz*pz+mass*mass)};

  add_particle_HepEvt(itype,ngenpa,0,stable,p,pamass(itype));
       
}

//_____________________________________________________________________________

int  MinBiasModule::balance_energy_momentum(double amx,int ngen0){
  //*********************************************************
  //
  // This subroutine balances momentum and energy for particles
  // generated by FRAGMX for the current diffractive mass AMX.
  // Momentum balance is achieved via
  // a scale transformation which is exact in principle and whose
  // accuracy is therefore only limited by the computer precision
  // in calculations. Energy conservation is implemented via a 
  //recursive algorithm which will give a .1% precison or an
  // error condition (ierr=2) asking to try with new initial
  // choises for momenta. The energy balance algorithm will
  // leave unchanged the previous balanced sum of Pz, again in
  // the limits of computer arithmetics. This is why we can
  // expect at the end SUM(Px) and SUM(Py) to be <1.E-5 but
  // SUM(Pz) only <1.E-3 GeV.

  //===============================================================
  //
  //  IMPORTANT NOTE : This subroutine will work only if all
  //  generated particles are outgoing ones. i.e. no decay in
  //  the generator is allowed. Otherway the energy conservation
  //  algorithm has to be modified.
  //
  //===============================================================
  //   Author:     S. Belforte   August 1984
  //================================================================
  //
  //   Argument description:
  //
  //  INPUT:
  //
  //  NGEN0 : I*4 is the starting point in the GENP common
  //		      for the list of generated particle to be
  //		      balanced in energy
  //
  //  OUTPUT:
  //
  //         IERR :  I*4 is the error return code:
  //	      	      0 = successfull calling
  //		      1 = geometrical error: all the particles
  //			  are in the same side of the vertex in
  //			  at least one direction.NOT SEVERE.
  //			  Caller can safely try again.
  //		      2 = phisical error: there is too much
  //			  transverse energy or particle with
  //			  too high pz or energy is too close to
  //			  multiplicity threshold. SEVERE ERROR.
  //			  Caller may try again, but... not too much!
  //================================================================

  double  pzpsum,pznsum,pzscal,epscal,enscal,pxscal,pyscal;

  double  esum=0.;
  double  pxsum=0.;
  double  pysum=0.;
  double  pzsum=0.;
  double  apxsum=0.;
  double  apysum=0.;
  double  apzsum=0.;
  double  pz3sum=0.;
  double  alpha=3.0;

  double px,py,pz,mass;

  CdfHepevt* hepevt = CdfHepevt::Instance();
  int numberOfParticles=hepevt->HepevtPtr()->NHEP;

  if(inelasticEvent) {
    double smult=1.5*(-7.+7.2*pow((amx*amx),0.127));
    if(numberOfParticles<=0.2*smult)                                 alpha=1.5;
    if(numberOfParticles>=0.2*smult&&numberOfParticles<=0.400*smult) alpha=1.7;
    if(numberOfParticles>=0.4*smult&&numberOfParticles<=0.795*smult) alpha=2.0;
    if(numberOfParticles>=0.795*smult)                               alpha=3.0;
  }



  for(int i=ngen0-1;i<numberOfParticles;i++){
    double* hp  = hepevt->HepevtPtr()->PHEP[i];
    px=hp[0];
    py=hp[1];
    pz=hp[2];

    pxsum+=px;
    pysum+=py;
    pzsum+=pz;
    apxsum+=fabs(px);
    apysum+=fabs(py);
    apzsum+=fabs(pz);
    pz3sum+=pow(fabs(pz),alpha);
  }

   //  check that for each directions there are particles in both
   //  sides, otherwise it would be impossible to balance momentum
   //  with only a scale change

   if(fabs(pxsum)==apxsum || fabs(pysum)==apysum || fabs(pzsum)==apzsum) {
     return 1;
   }

   pxscal=pxsum/apxsum;
   pyscal=pysum/apysum;
   pzscal=pzsum/pz3sum;

   //-------------------------------------------------
   //
   //  Adjuste momentum in order to have sump = 0.
   //  The momentum shift for each particle is proportional
   //  to its momentum, so to distorce not too much the
   //  original distribution of Pt and eta. This will be done
   //  obviously for daughters as same as parents, but the
   //  new outgoing sums will be computed only on daughters,
   //  i.e. really outgoing particles (termination code =0).

   esum=0.0;
   apzsum=0.;
   pzpsum=0.;
   pznsum=0.;

   for(int i=ngen0-1;i<numberOfParticles;i++){
					// kludge for EGCS/SSGI
     double* hp = hepevt->HepevtPtr()->PHEP[i];

     px   = hp[0];
     py   = hp[1];
     pz   = hp[2];
     mass = hp[4];

     px  -= fabs(px)*pxscal;
     py  -= fabs(py)*pyscal;
     pz  -= pow(fabs(pz),alpha)*pzscal;

     hp[0] = px;
     hp[1] = py;
     hp[2] = pz;

     if(hepevt->HepevtPtr()->ISTHEP[i]==stable) {
       if (pz>0.) pzpsum+=pow(pz,alpha);
       if (pz<0.) pznsum+=pow(fabs(pz),alpha);
       apzsum+=fabs(pz);
       esum+=sqrt(pow(px,2)+pow(py,2)+pow(pz,2)+pow(mass,2));
     }
     
   }

   //  Longitudinal momemtum is now changed to have
   //  total energy conservation. The change is proportional
   //  to Pz**3 for each particle, so to blame only very forward
   //  and/or backward particles for energy non conservation. This
   //  will disturbe very little input pseudorapidity distribution,
   //  giving rise only to some tails which anyway must be there!
   //  To save at the same time SUM(Pz)=0, particles are divided
   //  into two groups: those with Pz>0 and those with Pz<0, for each
   //  group a different scale is made in order to end with abs(SUM(Pz+))
   //  equal to abs(SUM(Pz-)) as in the beginning. This condition is not
   //  always automatically guaranteeded by the algorithm an so a check
   //  will be made at each iteration, actually the condition will be
   //  that Pz does not change it sign due to the rescale. The procedure
   //  is iterated up to a maximum of 15 times or till the total outgoing
   //  energy differs from AMX by less then 0.1%. A severe error condition
   //  is generated if 15 iterations are made without success or if it turns
   //  out impossible to guarantee longitudinal momentum conservation.

   //  If there is more transvere energy than total requested
   //  energy (AMX) of course very little can be done !

   if((esum-apzsum)>=amx) {
     //std::cout << " MBR : Energy momentum balance : esum-apzsim > amx " << std::endl;
     return 2;
   }


   int irep=0;
   double test=fabs(esum-amx);
 
   while (test >0.001*amx) {
     if(irep++>15) {
       //std::cout << "MBR : Energy momentum balance : irep++>15 " << std::endl;
       return 2;
     }

     if(pzpsum==0.||pznsum==0.) {
       //std::cout << "MBR : Energy momentum balance :pzpsum==0.  " << std::endl;
       return 2;
     }
     else {
       epscal=(amx-esum)/(2.*pzpsum);
       enscal=(amx-esum)/(2.*pznsum);
     }


     // scale the z-componenet to balance energy-momentum

     esum=0.;
     pznsum=0.;
     pzpsum=0.;
     double pz_old;

     for(int i=ngen0-1;i<numberOfParticles;i++){
       double* hp  = hepevt->HepevtPtr()->PHEP[i];
       pz= hp[2];
       pz_old=pz;

       if(pz>=0){
	 pz+=pow(pz,alpha)*epscal;
       }
       else {
	 pz-=pow(fabs(pz),alpha)*enscal;
       }

       if(pz*pz_old>0) {
         hp[2]=pz;
       }
       else {
         //std::cout << "MBR : Energy momentum balance : wrong sign " << std::endl;
	 return 2;
       }
	
       if(hepevt->HepevtPtr()->ISTHEP[i]==stable) {
					// kludge for SGI/EGCS
         double pz  = hp[2];

   	 if(pz>=0.) {
	   pzpsum+=pow(pz,alpha);
	 }
	 else {
	   pznsum+=pow(fabs(pz),alpha);
	 }
	 esum +=sqrt(hp[0]*hp[0]+hp[1]*hp[1]+hp[2]*hp[2]+hp[4]*hp[4]);
       }
     }
     //  test present total energy
     test = fabs(esum-amx);
   }
   return 0;
}

//_____________________________________________________________________________

int  MinBiasModule::total_multiplicity_by_dino(double wmu,double avnstar){
  //**********************************************************
  //
  // The  Subroutine DINO  generates the total multiplicity.
  // It is called by AVMUL with  2 S-dependent input parameters:
  // WMU, an S- dependent parameter which is calculated in the
  // Subroutine AVMUL  and AVNSTAR, the average no. of fictitious
  // pseudo-particles which decay into pairs of pi+,pi_ or into
  // pizeros. (See CDF NOTE # 675) 
  // The Subroutine DINO returns the multiplicity for the present
  // event in the value of MULTI.
  //================================================================
  //   Author:      J. Grunhaus   June 1988
  //   Modified:    T. Chapin     July 1989 Corrected to give total multiplicity
  //================================================================
  //
  //   Argument description:
  //
  //   INPUT:
  //
  //   WMU:      R*4  IS A S-DEPENDENT PARAMETER CALCULATED IN SUB. AVMUL
  //   AVNSTAR:  R*4  AVERAGE NO. OF CHARGED SECONDARIES CALCULATED IN SUB. AVMUL
  //
  //   OUTPUT:   MULTI:  I*4  TOTAL MULTIPLICITY
  //
  //================================================================
  //	GAMA(X)=GAMMA(X/2.)*GAMMA(X/2.+0.5)/(SQRT(3.1416)*2.**(1.-X))

  double ztop,gmax,fact,z,gaa,xmulti;
  double funct;
  HepRandomEngine* engine = MinBiasModule::minBiasEngine;

  ztop=( wmu-1.)/wmu;
  if(wmu<=10.) {
    double gamma_wmu=mbr_gamma(wmu);
    gmax=pow(wmu,wmu)/gamma_wmu*pow(ztop,(wmu-1.))*exp(-wmu*ztop);
  }
  else {
    fact=(wmu-1.)*log(ztop)+wmu*(1.-ztop)+0.5*log(wmu)-0.5*log(6.28);
    fact=exp(fact);
    gmax=fact/(1.+1./(12.*wmu));
  }

  double zmax= 8.0;

  do {
    z= RandFlat::shoot(engine)*zmax;
    if(wmu<=10.) {
      double gamma_wmu=mbr_gamma(wmu);
      gaa=pow(wmu,wmu)/gamma_wmu*pow(z,(wmu-1.))*exp(-wmu*z);
    }
    else {
      fact=(wmu-1.)*log(z)+wmu*(1.-z)+0.5*log(wmu)-0.5*log(6.28);
      fact=exp(fact);
      gaa=fact/(1.+1./(12.*wmu));
    }
    funct= RandFlat::shoot(engine)*gmax; 
  } while (funct > gaa);

      //  changed to give total multiplicity

  xmulti=3./2.*z*avnstar;
  int multi=int(xmulti);
  return multi;
}

//_____________________________________________________________________________

double  MinBiasModule::generate_random_o2(void){
  //********************************************************** 
  //
  //  This is a general subroutine to generate random numbers
  //  in the interval [0,1] according to a given probability
  //  distribution. One has simply to change the name of the 
  //  routine according to the function to use, and sustituing
  //  
  //  - the desidered prob. density as right side of the definition
  //    of PFUN. The function does not need to be normalized,
  //    but must of course be strictely positive. No check
  //    of concistency is made.
  //  - the minimum and maximun range for the argument of that
  //    function in the right side of XMIN and XMAX.
  //  
  //   Author:     S. Belforte    June 1984
  //
  //   Argument description:
  //  OUTPUT:  XRAN R*4 is the generated random number in [0,1]

  HepRandomEngine* engine = MinBiasModule::minBiasEngine;
  double xran;

  int    ipt;
  double xmin,xmax,dx,fsum,rn;
  double f[501];
  double x[501];
  int    init=0;

  // PROBABILITY DENSITY  function
  //  pfun(y)=1.0+cos(y)**2           inline function

  xmin=0.0;
  xmax=pi;

  //*********************************************************
  if (init==0) {

    // integrate and invert numerically the function PFUN
    // at the end F[i] will contain the normalized integral
    // from XMIN to I, and X[i] the value of the corrisponding
    // inverse function

    dx=(xmax-xmin)/500.0;
    x[0]=xmin;
    f[0]=0.;
    for(int i=0;i<500;i++){
      x[i+1]=x[i]+dx;
      f[i+1]=f[i]+pfun(x[i]-.5*dx);
    }
          
    fsum=f[500];

    //  normalize the integral

    for(int i=0;i<500;i++){
      f[i+1]=f[i+1]/fsum;
    }
    init=1;
  }

  ipt =-1;        
  while(ipt<0) {
    rn=1.1;
    while (rn<=0.||rn>1.) {
      rn=RandFlat::shoot(engine);
    }

    //   invert the integral funcion
    for (int i=0;i<500;i++) {
      ipt=i;
      if(rn <=f[i+1]) {
        ipt=i;
        //  interpolate linearly for better accuracy
        xran=x[i]+(rn-f[i])*((x[i+1]-x[i])/(f[i+1]-f[i]));
        return xran;
      }
    }
  }
  return 0.0;
}

//_____________________________________________________________________________

void  MinBiasModule::rotate_three_vector(double phi,double theta,double psi,
                          double &x,double &y,double &z){

  double  ax,ay,bx;

  ax=x*cos(psi)-y*sin(psi);  
  ay=x*sin(psi)+y*cos(psi);  
  bx=z*sin(theta)+ax*cos(theta);   

  z=z*cos(theta)-ax*sin(theta);    

  x=bx*cos(phi)-ay*sin(phi); 
  y=bx*sin(phi)+ay*cos(phi); 
}

//_____________________________________________________________________________

double  MinBiasModule::generate_rapidiy(double amx){

  //*********************************************************
  //
  // This subroutine generate the rapidity for a
  // particle from the decay of the current diffractive mass AMX
  // in its own center of mass, the mass and total multiplicity
  // are stored in the common MBRCOM. The distribution used is
  // a trapezoid centered around Y.average= 0 and extending
  // up to Y.max=ln(AMX).
  // Author:  S. Belforte
  //                                 September 1984

  HepRandomEngine* engine = MinBiasModule::minBiasEngine;
  double y;
  double rn,eee,fact,rrr,bot,yyy;
      
  int count=0;

  eee=flatRapidityRegion;
                             //eee=0.35; change for 0.4 in init
  fact=eee*log(amx);
  bot=log(amx)*(1.-eee);

  for(;;) {
    rn=RandFlat::shoot(engine);        
    y=log(amx)*(2.*rn-1.);
    if ( y > -fact &&  y < fact) return y;

    rrr=RandFlat::shoot(engine);
    if(y<= -fact)  yyy=(log(amx)+y)/bot;
    if(y>=  fact)  yyy=(log(amx)-y)/bot;
    if(rrr <= yyy) return y;

    if (count++>500) return 0;
  }

}

//_____________________________________________________________________________

double    MinBiasModule::generate_pt(double amx,int nt){

  //**********************************************************
  //
  // This subroutine generates the trsnsverse momentum from
  // a power law distribution. The mean value of Pt is in general
  // a raising function of the mass, but for masses below 60 GeV
  // the function for 60 GeV mass is used. For very low masses
  // (below 4 GeV) the maximum allowed Pt is fixed at the total
  // available energy divided by the current multiplicity.
  // It is an almost unmodified version of MB1 routine
  // PTDIST.
  //   Author:     S. Belforte  July 1984
  //================================================================

  HepRandomEngine* engine = MinBiasModule::minBiasEngine;
  double pt;
  double am,ptcut,dxpt,xpt,fptsum,rn;

  double fpt3[501];
  double pt3[501];

  double ptmax=50.;
  double ptsoft=1.27;
  double power=4.0;
  double amo=0.0;

      //      f3(x)=x/pow((1.+x/ptsoft),power);

  am=amx;
  if (amx<60.) am=60.;
  ptcut=amx/double(nt);
  if (am!=amo) {
    power=4.+35.83/log(am/0.3);
    amo=am;
    dxpt=0.5*ptmax/500.;
    pt3[0]=0.;
    fpt3[0]=0.;
    for(int i=0;i<500;i++){
      xpt=double(i+1)*ptmax/500.;
      pt3[i+1]=xpt;
      fpt3[i+1]=fpt3[i]+f3(xpt-dxpt,ptsoft,power);
    }
    fptsum=fpt3[500];
      for(int i=0;i<500;i++){
        fpt3[i+1]=fpt3[i+1]/fptsum;
      }
  }

  int count=0;

  for (;;) {
    if(count++>500) return 0;       
    do {
      rn=RandFlat::shoot(engine);
    } while (rn <0.0 || rn > 1.0);

    for(int i=0;i<500;i++){
      if(rn <= fpt3[i+1]) {
        pt=pt3[i]+(rn-fpt3[i])*((pt3[i+1]-pt3[i])/(fpt3[i+1]-fpt3[i]));
        if(amx<4.0 && pt>ptcut) break;
        return pt;
      }
    }
  } 
}

//_____________________________________________________________________________

int MinBiasModule::fragment_cluster(int ngenmx,double pxcm[4],int icharg){
  //
  //     This is a general subroutine to simulate at least in an
  //     approssimate way the decay of a mass AMX according to
  //     phenomenogical distributions of multiplicity, rapidity 
  //     and transverse momentum. The method is quite general
  //     and is intended to be used for each kind of event:
  //     single diffractive, double diffractive and hard core 
  //     collision. In any case the mass effectively used for
  //     generating the multiplicity is MX-M(proton) which the
  //     doublely available mass.
  //
  //      Author:  S.Belforte   June 1984
  //
  //   Argument description:
  //
  //  INPUT:
  //
  //         NGENM : I*4 is the particle number of the system in the
  //                     /GENP/ common
  //         PXCM[4]:R*4 is the beta of the Lorentz transformation from
  //		      CM-X ref. system to CM-event system
  //	  ICHARG: I*4 is the total charge of the decaying system
  //
  //  OUTPUT:
  //
  //         IERR :  I*4 is the error return code:
  //	      	      0 = successfull calling
  //		      1 = unable to balance at least approximately
  //		          the energy in the maximun allowed number
  //		          of trials
  //


  CdfHepevt* hepevt = CdfHepevt::Instance();       
  HepRandomEngine* engine = MinBiasModule::minBiasEngine;

  int ngen0,nch,nn,ip1,ip2,ip3;
  int nnsav,ier1,nchsav;
  int nt;
  
  // define the maximum number of iterations of multiplicity
  // and kinematics generation before changing something at a higher level
  
  int mxking=10;
  int mxmulg=10;
  int imulc=0; 
  int ikinc=0;
  
  int ierr=0;
  double flag=0.0;
  
  double* hp = hepevt->HepevtPtr()->PHEP[ngenmx-1];
  double amx = hp[4];
  
  //std::cout << " hardcore:  index " << ngenmx << " mass  " << amx << std::endl;
  
  int currentParticle=hepevt->HepevtPtr()->NHEP;
  ngen0=currentParticle+1;
  
 A20: imulc=imulc+1;
  
  if(imulc>=2) {
    mulflg=1;
    iermlg++;
  }
  
  if(imulc>mxmulg) {
    //	Too many tries without producing an avent such to be balanced by BALAN.
    //	Return with IERR=1. The subroutine was  called for particle 
    // which had mass  and charge last generated multiplicity
    nt=0;
    ierr=1;
    return ierr;
  }
  
  do {
    do {
      nt=average_multiplicity(amx);
    }  while (nt < 2 || nt >MAXGEN);
    
    nch=0;  nn=0; ip1=0;  ip2=0;  ip3=0;
    
    //
    // First track is always a baryon, to have things not too wrong
    // for little masses. If total charge (ICHARG) =0 then it is a
    // hard core collision with MX=sqrt(S) and there is no need of
    // such baryon. If the baryon turns out to be neutral (according
    // to PNRAT branching ratio) a charged pi-meson is also generated
    // to account for charge conservation, so only a total zero
    // charge is left to subsequent code.
    
    
     if(icharg!=0) {
       flag= (RandFlat::shoot(engine)<pnrat);    //decide if baryion is charged
       if(icharg==1) {                    // proton fragments
         if(flag) {                       //     proton
           ip1=PSPRO;
	   nch=nch+1;
	   nt=nt-1;
         }
	 else	{	     //neutron + pi+
	   ip1=PSNTN;
	   ip2=PSPIP;
	   nch=nch+1;
	   nn=nn+1;
	   nt=nt-2;
	 }
       }
       else {                 //  antiproton fragments
	 if(flag) {           //  antiproton
	   ip1=PSPROB;
	   nch=nch+1;
	   nt=nt-1;
	 }
	 else {               //   antineutron + pi-
	   ip1=PSNTNB;
	   ip2=PSPIM;
	   nch=nch+1;
	   nn=nn+1;
	   nt=nt-2;
	 }
       }
     }	 

     //  now compute the total number of pi0 and pi+pi- pairs to generate

     double n=int(0.5+double(nt)/3.*2.);
     if(icharg== 0 && n==1) {
       n=2;
     }

     // now decide for each of these N 'object' if it is a neutral
     // or a +- pair, this will give the total definitive multiplicity
     // for this fragmentation

     for(int i=0;i<n;i++){
       if(RandFlat::shoot(engine)>0.5) {
	 nn=nn+1;
       }
       else {
	 nch=nch+2;
       }
     }
     nt=nch+nn;
   } while (nt>MAXGEN);
    
   nchsav=nch;
   nnsav=nn;
   
   // Update /GENP/ informations on MX with decay informations

   currentParticle=hepevt->HepevtPtr()->NHEP;
   int* jd = hepevt->HepevtPtr()->JDAHEP[ngenmx-1];
   jd[0]   = currentParticle-1;

   //   MbrEvt.set_statusCode(ngenmx,201);

   // Separate 2 and 3-body decays of the diffraccted mass for which
   // exact phase space can be used. If only the nucleon for the two body
   // case has been fixed, then a pi0 it is added, a pi0 is added in any
   // case to the exixting nucleon-pi pair in the three body case if the
   // nucleon was neutral, if the nucleon was charged a pi+pi- or pi0pi0
   // pair is added with equal probability. If the 
   // total charge is zero and no nucleon was generated, then all particle
   // types are setted according to generated NCH and NN.

   if ((nt==2)&&(icharg!=0)) {
     if(ip2==0) ip2=PSPIZ;
     ier1=two_body_decay(ngenmx,ip1,ip2);
     if(ier1!=0) {
       return ier1;
     }
   }
   else if ((nt==3)&&(icharg!=0)) {
     if(flag){
       if(RandFlat::shoot(engine)>.5) {
         ip2=PSPIP;
         ip3=PSPIM;
       }
       else {
         ip2=PSPIZ;
         ip3=PSPIZ;
       }
     }
     else {
       ip3=PSPIZ;
     }
     ier1=three_body_decay(ngenmx,ip1,ip2,ip3);
     if(ier1!=0) {
       return ier1;
     }
   }
   else {

     ikinc=0;

     do {
       if(ip1!=0){
         generate_one_particle_from_fire_ball(ngenmx,ip1,nt);
         if(flag) {
           nch=nch-1;
         }
         else { 
           nn=nn-1;
         }
       }
       if(ip2!=0) {
         generate_one_particle_from_fire_ball(ngenmx,ip2,nt);
         nch=nch-1;
       }

       // Now loop over the other particles MX decays in

       for(int i=0;i<nch/2;i++){
         generate_one_particle_from_fire_ball(ngenmx,PSPIP,nt);
         generate_one_particle_from_fire_ball(ngenmx,PSPIM,nt);
       }
       for(int i=0;i<nn;i++){
         generate_one_particle_from_fire_ball(ngenmx,PSPIZ,nt);
       }

       ier1=balance_energy_momentum(amx,ngen0); 

       if(ier1!=0) {         // reset pointers for the new try
         hepevt->HepevtPtr()->NHEP=(ngen0-1);
         nn=nnsav;
         nch=nchsav;

         if(ier1>=2) {
           ikinc=ikinc+1;
           if(ikinc>=2) {
             iknflg=1;
             ierkng++;
           } 
           if(ikinc>mxking) {
             goto A20;
	   }
         }
       }   //  ier1 != 0
     } while (ier1!=0);
   }

  //    Boost event to center of mass frame

   double v1[4],v2[4];

   int numberOfParticles=hepevt->HepevtPtr()->NHEP;
   for(int i=ngen0-1;i<numberOfParticles;i++){
					// kludge for EGCS/SGI
     double* hp = hepevt->HepevtPtr()->PHEP[i];
     for (int j=0;j<4;j++) {
       v1[j]=hp[j];
     }

     v1[3]=sqrt(v1[0]*v1[0]+v1[1]*v1[1]+v1[2]*v1[2]+hp[4]*hp[4]);

     add_lorentz_boost(pxcm,v1,v2);

     for (int j=0;j<4;j++) {
       hp[j]=v2[j];
     }
   }
   return 0;
}

//_____________________________________________________________________________

double  MinBiasModule::mbr_gamma(double x) {
   
//Gamma function (approx from Gradshteyn+Ryzhik)
    double g=pow(x,(x-0.5))*exp(-x)*sqrt(2*3.141592654)*(1.+1./(12.*x)
        +1./(288.*x*x)-139./(51840.*x*x*x)-571./(2488320.*x*x*x*x));
  return g;
}

//_____________________________________________________________________________

void MinBiasModule::print(void){

  std::cout <<" Minimum Bias Rockefeller Monte Carlo Generator " << std::endl;
  std::cout <<" Center of mass energy                 " << tecm   << std::endl;
  std::cout <<" Total cross section (mb)              " << sigtot << std::endl;

  std::cout <<" Hard Core cross section (mb)          "<<sig0  <<"   "<< sgrat[0]<<std::endl;
  std::cout <<" Double diffractive cross section (mb) "<<sigdd <<"   "<< sgrat[1]<<std::endl;
  std::cout <<" Single diffractive cross section (mb) "<<sigsd <<"   "<< sgrat[2]<<std::endl;
  std::cout <<" Elastic cross section (mb)            "<<sigel <<"   "<< sgrat[3]<<std::endl;
  std::cout <<std::endl;

  std::cout <<" Resonance  contribution (mb)          "<<sigres<<"   "<<std::endl;
  std::cout <<" Relative contribution to single diffractive events are :" << std::endl;
  std::cout <<"   Continuum  "  << sgdrat[0]  
       <<"   M**2 =2.2  "  << sgdrat[1]  
       <<"   M**2 =2.8  "  << sgdrat[2]  
       <<"   M**2 =4.4  "  << sgdrat[3] <<std::endl;

  std::cout <<" Initial rectangular region " << sgdrat[4] << std::endl;
  std::cout <<" Minimum and maximum for diffractive mass " << ammin << " " <<ammax << std::endl;


  std::cout <<" Incremental range for different sources "
       <<rate[0] <<"  "<<rate[1] <<  "  "<<rate[2] <<"  "<<rate[3]<<std::endl;

  std::cout <<" Incremental range for different masses "
       <<drate[0] <<"  "<<drate[1]<<"  "<<drate[2]<<"  "<<drate[3]<<"  "<<drate[4]<<std::endl;

}
//******************************************************************************
//*  Minimum bias event generator  adopated from mbr                           *
//*  Anwar Ahmad Bhatti            December 24, 1998                           *
//*                                                                            *
// *0001 Feb 22 1999 P.Murat: put in kludges for EGCS/SGI                      *
//******************************************************************************

void MinBiasModule::add_particle_HepEvt(int type,int firstMother,int lastMother,
			            int status,double p[4],double mass){

  CdfHepevt* hepevt = CdfHepevt::Instance();

   hepevt->HepevtPtr()->NHEP=hepevt->HepevtPtr()->NHEP+1;

   int nhep=hepevt->HepevtPtr()->NHEP;

   if(nhep>=1 && nhep<=NMXHEP) {
     int i=nhep-1;
     hepevt->HepevtPtr()->IDHEP[i]  = type;
     hepevt->HepevtPtr()->ISTHEP[i] = status;

     int* hm = hepevt->HepevtPtr()->JMOHEP[i];
     hm[0]=firstMother;
     hm[1]=lastMother;

     int* hd = hepevt->HepevtPtr()->JDAHEP[i];
     hd[0]=0;
     hd[1]=0;

     double* hp = hepevt->HepevtPtr()->PHEP[i];
     double* hv = hepevt->HepevtPtr()->VHEP[i];

     for (int j=0;j<4;j++){
       hp[j]=p[j];
       hv[j]=0.0;
     }
     hp[4]=mass;
   }

}



















