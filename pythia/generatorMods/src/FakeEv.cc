//<pre>-------------------------------------------------------------------------
//   File and Version Information:
//   gntMods/FakeEv.cc
//
// Author List:
//   Marjorie Shapiro
//   Lawrence Berkeley Laboratory
//
// Description:
// ------------
//   This module generates one or more particles and puts the results
//   in the appropriate TRYBOS banks.  The talk_to for the module allows
//   the user to specify the species and number of particles as well
//   as to choose the distribution of pt (or momentum), eta (or Theta)
//   and phi of the particles.
//
//   The random number generation is done using the GenerateParams class.
//
// Setting of the generated parameters:
// ------------------------------------
//
//           SET PARAMETER_NAME MEAN SIGMA PMIN PMAX POWER MODE
// 
// where PARAMETER_NAME is one of P, PT, ETA, THETA, PHI, CDFCODE, NPARTICLES
// comments:
// - NPARTICLES: number of particles generated per event
// - MODE = 1  : gaussian distribution  
//              (use MEAN and SIGMA, PMIN and PMAX have no effect)
//        = 2  : flat (POWER = 0) or power law (POWER != 0) distribution, 
//               (use PMIN and PMAX, MEAN and SIGMA have no effect)
//        = 4  : exponential (use MEAN, PMIN and PMAX, SIGMA not used)
//        = 5  : power law with offset : (x-MEAN)^POWER in [PMIN,PMAX]
// - keep in mind that to the moment no check for P/PT <= 0 is being made,
//   user is responsible for defining limits correctly

#ifdef __GNUG__
#pragma implementation
#endif

#include <sstream>
#include "inc/misc.hh"
#include "generatorMods/FakeEv.hh"
#include "evt/Event.hh"
#include "stdhep_i/CdfHepevt.hh"
#include "ParticleDB/ParticleDb.hh"
#include "Framework/APPFramework.hh"

// CLHEP Random Number headers
#include "r_n/CdfRn.hh"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGaussT.h"
#include "CLHEP/Random/RandExponential.h"

const long FakeEv::_defaultRandomSeed1 = 922883591;
const long FakeEv::_defaultRandomSeed2 = 109735476;
const char* FakeEv::genId = "FAKE_EVENT";

using std::ostream;
using std::cout;
using std::endl;
using std::ostringstream ;


FakeEv::FakeEv() : 
  AbsGenModule( FakeEv::genId, "Single Particle Gun Module" ),
  _useCommand("use",this),
  _genCommand("generate",this),
  _randomSeed1("RandomSeed1" ,this,FakeEv::_defaultRandomSeed1),
  _randomSeed2("RandomSeed2" ,this,FakeEv::_defaultRandomSeed2),
  _antiParticle("antiParticle",this,0),
  _genMom(50.,0.,  1.,100.,0.,1), // default parameters
  _genPt(50.,0.,  1.,100.,0.,1),
  _genMass(50.,0.,  1.,100.,0.,1),
  _genTheta(90.,0.,-45., 45.,0.,1),
  _genEta(0.0,0., -1.,  1.,0.,2),
  _genY(0.0,0., -1.,  1.,0.,2),
  _genPhi(7.5,0.,  0.,360.,0.,2),
  _usePtHist("usePtHist",this,false),
  _lowPt("lowPt",this,0.),
  _highPt("highPt",this,0.,0.),
  _nbinPt("nbinPt",this,0),
  _valPt("valPt",this,1,200)
{
  commands()->append(&_useCommand);
  commands()->append(&_genCommand);
  commands()->append(&_antiParticle);
  _initializeRandomTalkTo();
  _initializeUsePtHistTalkTo();
				        // <a name="qq">
					// Fill default values for data
					// Order of arguments to init:  
					// value, sigma, minRange, maxRange, power
					// (all floats)
					// mode=1. means use gaussian distribution
					// mode=2  means flat or power law 
					// distribution
					// mode=4 means exponential distribution
                         
					// by default simulate 1 particle/event
  NParticles = 1;
					// generate in PT rather than P
  UsePt      = 1;
					// Generate not in Eta not Theta nor Y
  PolarChoice     = NOCHOICE;
					// default particle is Pi+
  CdfCode    = 401;
  memset(_myValPt,0,200*sizeof(double));
}

FakeEv::~FakeEv() {
}

//------------------------------------------------------------------------------
//  user interface handler for FAKE_EVENT_MODULE
static int process_show_command(FakeEv* module, int argc, char** argv) {
  char cmd[100];

  for (int i=0; i<argc; i++) {
    for (int j=0; j<=strlen(argv[i]); j++) cmd[j]=toupper(argv[i][j]);

    if      (strcmp(cmd,"P") == 0) {
      module->genMom()->print();
    }
    else if (strcmp(cmd,"PT") == 0) {
      module->genPt()->print();
    }
    else if (strcmp(cmd,"MASS") == 0) {
      module->genMass()->print();
    }
    else if (strcmp(cmd,"THETA") == 0) {
      module->genTheta()->print();
    }
    else if (strcmp(cmd,"ETA") == 0) {
      module->genEta()->print();
    }
    else if (strcmp(cmd,"PHI") == 0) {
      module->genPhi()->print();
    }
    else if (strcmp(cmd,"Y") == 0) {
      module->genY()->print();
    }
    else if (strcmp(cmd,"ALL") == 0) {
      module->genMom()->print();
      module->genPt()->print();
      module->genMass()->print();
      module->genTheta()->print();
      module->genEta()->print();
      module->genPhi()->print();
      module->genY()->print();
      std::cout << "CdfCode= " << module->getCdfCode() << std::endl;
    }
  }
  return 1;
}

static int process_set_command(FakeEv* module, int argc, char** argv) {

					// "FORTRAN equivalence"
  FakeEv::GenPars*  genpar;
  float*           par;
  int*             ipar;
  char             cmd[100];
  int              len_cmd;

  if (argc <= 0) return -1;
  len_cmd   = strlen(argv[0]);

  for (int j=0; j<=len_cmd; j++) cmd[j]=toupper(argv[0][j]);

  if      (strcmp(cmd,"P") == 0) {
					// set momentum (what about Mode ????)
    genpar = module->genMom();
  }
  else if (strcmp(cmd,"PT") == 0) {
    genpar = module->genPt();
  }
  else if (strcmp(cmd,"MASS") == 0) {
    genpar = module->genMass();
  }
  else if (strcmp(cmd,"THETA") == 0) {
    genpar = module->genTheta();
  }
  else if (strcmp(cmd,"Y") == 0) {
    genpar = module->genY();
  }
  else if (strcmp(cmd,"ETA") == 0) {
    genpar = module->genEta();
  }
  else if (strcmp(cmd,"PHI") == 0) {
    genpar = module->genPhi();
  }
  else if (strcmp(cmd,"CDFCODE") == 0) {
    if(argc>1) {
      int code = atoi(argv[1]); 
      module->setCdfCode(code);
      return 1 ;
    }
  }
  else if (strncmp(cmd,"NPARTICLES",len_cmd) == 0) {
    if(argc>1) {
      int np = atoi(argv[1]); 
      module->setNParticles(np);
      return 1;
    }
  }
  else {
    std::cout << "process_set_command : unknown parameter" << std::endl;
    return -2;
  }

  par = (float*) genpar;
  ipar = (int* ) genpar;

  if(argc<6) {
    std::cout << " Not all parameters specified: Use show to check status\n"
         << std::endl;
  }
  for (int i=1; i<argc; i++) {
    if (i < 6) {
					// floats : val, sig, min, max, power

      par[i-1] = (float) atof(argv[i]);
    }
    else if (i == 6) {
					// mode flag

      ipar[i-1] = atoi(argv[i]);
      if(ipar[i-1]<1 || ipar[i-1]>5) {
	std::cout << " illegal mode " << ipar[i-1] << " set to " << 1 << std::endl;
        ipar[i-1] = 1;
      }
    }
    else {
      std::cout << "Too many parameters given:  only 1st 6 used\n";
    }
  }
  if (par[3] <= par[2] ) {
    std::cout << " FAKEEV: generated Min value is larger than Max value:"
              << " min: " << par[2] << " max: " << par[3]
              << ". Reseting values to min = " << par[2];
    //par[2]=0;
    par[3]=par[2]+1.0e-5;
    std::cout << ", max = " << par[3] << std::endl;
  }
  return 1;
}

static void process_use_command(FakeEv* module, int argc, char** argv) {
  char cmd[100];
  for (int i=0; i<argc; i++) {
    for (int j=0; j<=strlen(argv[i]); j++) cmd[j]=toupper(argv[i][j]);
    if (strcmp(cmd,"MASS") == 0) {
      module->setUseMass(1);
    }
    if (strcmp(cmd,"PT") == 0) {
      module->setUsePt(1);
    }
    else if (strcmp(cmd,"P") == 0) {
      module->setUsePt(0);
    }
    else if (strcmp(cmd,"ETA") == 0) {
      if (module->useY()) {
	cout << " \t \t FAKE_EVENT " 
	     << " Using Eta overrides Using Rapidity " << endl;
      }
      else if (module->useTheta()) {
	cout << " \t \t FAKE_EVENT " 
	     << " Using Eta overrides Using Theta " << endl;
      }
      module->setUseEta();
    }
    else if (strcmp(cmd,"THETA") == 0) {
      if (module->useY()) {
	cout << " \t \t FAKE_EVENT " 
	     << " Using Theta overrides Using Rapidity " << endl;
      }
      else if (module->useEta()) {
	cout << " \t \t FAKE_EVENT "   
	     << " Using Theta overrides Using Eta " << endl;
      }
      module->setUseTheta();
    }
    else if (strcmp(cmd,"Y") == 0) {
     if (module->useEta()) {
       cout << " \t \t FAKE_EVENT "  
	    << " Using Rapidity overrides Using Eta " << endl;
     } else if (module->useTheta()) {
       cout << " \t \t FAKE_EVENT "   
	    << " Using Rapidity overrides Using Theta " << endl;
     }
     module->setUseY();
    } 
  }
}

void FakeEv::menuHandler(char* menu, char* command, int argc, char** argv) {
  char cmd[100];
  for (int j=0; j<=strlen(command); j++) cmd[j]=toupper(command[j]);
  if      (strcmp(cmd,"SHOW") == 0) {
    process_show_command(this,argc,argv);
  }
  else if (strcmp(cmd,"SET") == 0) {
    process_set_command(this,argc,argv);
  }
  else if (strcmp(cmd,"USE") == 0) {
    process_use_command(this,argc,argv);
  }
}

int FakeEv::callGenerator(AbsEvent* event) {
  double pt, p, mass, eta, theta, phi;
  double px, py, pz, rn;
  HepRandomEngine* engine = CdfRn::Instance()->GetEngine("FAKE_EVENT");

  for (int i=0; i<NParticles; i++) {
					// Generate Eta or Theta
//-----------------------------------------------------------------------------
// add particle to /HEPEVT/
//-----------------------------------------------------------------------------
    int code = CdfCode;
    if (_antiParticle.value() > 0) {
//-----------------------------------------------------------------------------
// generate mix of particles and antiparticles, check for antiparticle
//-----------------------------------------------------------------------------
      rn = RandFlat::shoot(engine,0.,1.);
      if (rn < _antiParticle.value()) {
	code = -CdfCode;
      }
    }

    mass = ParticleDb::Instance()->Mass(code);
    if(useMass) {
      mass = generateValue(_genMass);
    }

    if(useY()) {
      eta = generateValue(_genY);
    }
    else if(useEta()) {
      eta   = generateValue(_genEta);
      theta = 2.*atan(exp(-eta));
    }
    else {
      theta = DEGRAD*generateValue(_genTheta);
    }
					// Generate Phi

    phi = DEGRAD*generateValue(_genPhi);
					// Generate energy or Et
    if(UsePt) {
      if(_usePtHist.value()) { 
	pt = generatePtUsingHist(); 
	if (verbose()) Plot("pt", (float)pt, _nbinPt.value(),  
			    (float)_lowPt.value(),
			    (float)_highPt.value());
      }
      else {
	pt   = generateValue(_genPt); }
      if (useY()) {
        double transverseMass = sqrt(pt*pt + mass*mass);
        theta = atan((pt/(transverseMass*sinh(eta))));
      }
      p    = pt/sin(theta);
    }
    else {
      p    = generateValue(_genMom);
      if (useY()) {
        double transverseMass = sqrt(p*p + mass*mass)/cosh(eta);
        theta = atan( sqrt(transverseMass*transverseMass - mass*mass)/(transverseMass*sinh(eta)));
      }
      pt   = p*sin(theta);
    }
					// define particle 4-momentum in the 
					// vertex
    px     = pt*cos(phi);
    py     = pt*sin(phi);
    pz     = p*cos(theta);

    Hepevt_t* hepevt = CdfHepevt::Instance()->HepevtPtr();

    hepevt->IDHEP[i]     = ParticleDb::Instance()->ParticlePdgCode(code);

					// final state particles have 
					// status code equal to 1
    hepevt->ISTHEP[i]    = 1;
    hepevt->JMOHEP[i][0] = 0;
    hepevt->JMOHEP[i][1] = 0;
      
    hepevt->JDAHEP[i][0] = 0;
    hepevt->JDAHEP[i][0] = 0;

    hepevt->PHEP[i][0]   = px;
    hepevt->PHEP[i][1]   = py;
    hepevt->PHEP[i][2]   = pz;
    hepevt->PHEP[i][3]   = sqrt( px*px + py*py + pz*pz + mass*mass );
    hepevt->PHEP[i][4]   = mass;

    hepevt->VHEP[i][0]   = 0.;
    hepevt->VHEP[i][1]   = 0.;
    hepevt->VHEP[i][2]   = 0.;
    hepevt->VHEP[i][3]   = 0.;
  }

  CdfHepevt::Instance()->HepevtPtr()->NHEP = NParticles;
  return 0;
}

// part of the code should be moved from callGenerator - lena
void FakeEv::fillHepevt() {}

//------------------------------------------------------------------------------
void FakeEv::PrintState(std::ostream& output) const {
  output << " " << std::endl;
  output << "----- " << name( ) << " Parameters of Generation" << std::endl;
  output << "     CdfCode    "  << CdfCode    << std::endl;
  output << "     NParticles "  << NParticles << std::endl;

  if (UsePt == 1) {
    output << "     Pt       "; _genPt.print(output);
  }
  else {
    output << "     Momentum "; _genMom.print(output);
  }
  if (PolarChoice == USE_ETA){
    output << "     Eta      "; _genEta.print(output);
  }
  else if (PolarChoice == USE_Y){
    output << "     Y      ";  _genY.print(output);
  }
  else if (PolarChoice == USE_THETA) {
    output << "     Theta    "; _genTheta.print(output);
  }
  else {
    output << " No Y/ETA/THETA Choice made.  Will use ETA as default " << endl
	   << "     Eta      "; _genEta.print(output);
  }
  output <<   "     Phi      "; _genPhi.print(output);
  output <<   "     Mass     "; _genMass.print(output);
}

//______________________________________________________________________________
FakeEvCommand::FakeEvCommand() {
}

//______________________________________________________________________________
FakeEvCommand::~FakeEvCommand(){
}

//______________________________________________________________________________
FakeEvCommand::FakeEvCommand(const char* const theCommand, 
       FakeEv* theTarget) : APPCommand( theCommand, theTarget)
{
  _thisModule = theTarget;
}

//______________________________________________________________________________
int FakeEvCommand::handle(int argc, char* argv[])
{
  int rc = 0;
  if(strncmp(command(),"generate",8)==0) {
    target()->menuHandler("FAKE_EVENT","SET",argc-1,argv+1);
  }
  else if(strncmp(command(),"use",3)==0) {
    target()->menuHandler("FAKE_EVENT","USE", argc-1, argv+1);
  }
  else {
    std::cout << " unknown command " << command() << std::endl;
  }
  
  return rc;

}

//______________________________________________________________________________
void FakeEv::Set(char* nm, float mean, float sigma, float pmax, float pmin,
		 float power, int mode) {

  char  name[100];
  GenPars* par;

  for (int i=0; i<=strlen(nm); i++) name[i] = toupper(nm[i]);

  if      (strcmp(name,"P") == 0) {
    par = &_genMom;
    UsePt = 0;
  }
  else if (strcmp(name,"PT") == 0) {
    par = &_genPt;
    UsePt = 1;
  }
  else if (strcmp(name,"MASS") == 0) {
    par = &_genMass;
    setUseMass(1);
  }
  else if (strcmp(name,"THETA") == 0) {
    par = &_genTheta;
    setUseTheta();
  }
  else if (strcmp(name,"ETA") == 0) {
    par = &_genEta;
    setUseEta();
  }
  else if (strcmp(name,"Y") == 0) {
    par = &_genY;
    setUseY();
  }
  else if (strcmp(name,"PHI") == 0) {
    par = &_genPhi;
  }
  else {
    std::cout << " FakeEv::Set detected wrong parameter name : " << nm << std::endl;
    return;
  }
  par->init(mean,sigma,pmin,pmax,power,mode);
}


//______________________________________________________________________________
void FakeEvCommand::show() const {
  // Only want to print things once.
  if(strncmp(command(),"generate",8)==0) {
   _thisModule->PrintState();
  }
}

//------------------------------------------------------------------------------
bool FakeEvCommand::isShowable() const {return true;}

//______________________________________________________________________________
string FakeEvCommand::description() const {
  string retval;
  if(strncmp(command(),"generate",8)==0) {
    retval += "possible syntaxes are:: \n";
    retval += "\t\t generate PARAM_NAME mean sigma pmin pmax power mode\n";
    retval += "\t\t where PARAM_NAME is one of P, PT, MASS, ETA, THETA,Y, PHI\n";
    retval += "\t\t\t mode=1 means Gaussian (uses mean, sigma, pmin, pmax) \n";
    retval += "\t\t\t mode=2 means flat\n"; 
    retval += "\t\t\t mode=3 means Lorentzian (uses mean, width, pmin, pmax) \n";
    retval += "\t\t\t mode=4 means exponential(uses mean, pmin, pmax) \n";
    retval += "\t\t\t mode=5 means power law (uses mean, pmin, pmax, power)\n"; 
    retval += "\t\t note:  PHI is in degrees not radians!!\n";
    retval += "\t OR: \n";
    retval += "\t\t generate CDFCODE code\n";
    retval += "\t OR: \n";
    retval += "\t\t NPARTICLES number_of_particles";
  }
  else if(strncmp(command(),"use",3)==0) {
    retval += "specify if generation is in P or PT and/or THETA or ETA or Y\n";
    retval += "\t\t syntax is:  use variable\n";
    retval += "\t\t where variable means P, PT, THETA, ETA, Y\n";
  }
  else {
    std::cout << " unknown command " << command() << std::endl;
  }
  
  return retval;
}

//_____________________________________________________________________________
AppResult FakeEv::genBeginRun(AbsEvent* run) {
  return AppResult::OK;
}

//_____________________________________________________________________________
AppResult FakeEv::genEndRun(AbsEvent* run) {
  return AppResult::OK;
}

//_____________________________________________________________________________
AppResult FakeEv::genBeginJob() { 
  CdfRn* rn = CdfRn::Instance();
  if ( !rn->isReadingFromFile() ) {
    rn->SetEngineSeeds(_randomSeed1.value(), _randomSeed2.value(),FakeEv::genId);
  }
  if (PolarChoice == NOCHOICE) {
    std::cout << " FAKE_EVENT: No Choice of ETA/THETA/Y" 
	      << std::endl << " Setting default to ETA " << std::endl; 
    setUseEta();
  }

  if (_usePtHist.value() ) {
    if ( _nbinPt.value() > 200 ) { 
      std::cout << " FAKE_EVENT: usePtHist is used; \n Tcl error: Maximum number of bins is 200. \n";
      return AppResult::ERROR;
    }
    if ( _lowPt.value() >= _highPt.value() ) {
      std::cout << " FAKE_EVENT: usePtHist is used; \n Tcl error: low Pt value is greater or equal then high Pt .\n";
      return AppResult::ERROR;
    }
    if ( _nbinPt.value() <= 0 ) {
      std::cout << " FAKE_EVENT: usePtHist is used; \n Tcl error: zero or negative number of pt bins .\n";
      return AppResult::ERROR;
    }
    double min=0,max=0;
    int j = 0;
    for (AbsParmList<double>::ConstIterator i=_valPt.begin();
	 i != _valPt.end(); i++) {
      min=min <= (*i) ? min : (*i);
      max=max >= (*i) ? max : (*i);      
      _myValPt[j]=(*i);
      j++;
    }
    if (min<0.) { 
      std::cout << " FAKE_EVENT: usePtHist is used; \n Tcl error: negative Pt value in the histogram .\n";
      return AppResult::ERROR;
    }
    if (min==max && max==0.) {
      std::cout << " FAKE_EVENT: usePtHist is used; \n Tcl error: zero Pt histogram .\n";
      return AppResult::ERROR;
    }
    
  }
  return AppResult::OK;
}

//_____________________________________________________________________________
AppResult FakeEv::genEndJob() {
  return AppResult::OK;
}
    
float FakeEv::generateValue(const GenPars& pars) const {
  double a, b, p, tmp, z, val;
  HepRandomEngine* engine = CdfRn::Instance()->GetEngine("FAKE_EVENT");
  switch (pars.mode()) {
  case 1: // Gaussian
    if (!pars.sigma())
      return pars.mean();
    else {
      tmp = 0.0;
      do {
	tmp = RandGaussT::shoot( engine, pars.mean(), pars.sigma() );
      } while // allow ranges to apply to gaussian, where non-zero
        ((pars.min() || pars.max()) && (tmp < pars.min() || tmp > pars.max()));
      return (float) tmp;
    }
  case 2: return RandFlat::shoot(engine,pars.min(),pars.max()); // Flat
  case 3:
    if (!pars.sigma())
      return pars.mean();
    else {
      tmp = 0.;
      do {
	a = RandGaussT::shoot( engine, 0., 1. );
	b = RandGaussT::shoot( engine, 0., 1. );
	tmp = pars.mean() + 0.5*pars.sigma()*a/b;
      } while // pars.sigma() the half-width of Breit-Wigner mass distribution
	((pars.min() || pars.max()) && (tmp < pars.min() || tmp > pars.max()));
      return (float) tmp; }
  case 4:
    {
      tmp = 0.0;
      do {
	tmp = RandExponential::shoot( engine, pars.mean());
      } while // allow ranges to apply to exponential, where non-zero
        ((pars.min() || pars.max()) && (tmp < pars.min() || tmp > pars.max()));
      return (float) tmp;
    }
  case 5:
//-----------------------------------------------------------------------------
//  case 5: (x-pmin)^p1
//-----------------------------------------------------------------------------
    if (!pars.power()) {
      return RandFlat::shoot(engine,pars.min(),pars.max());
    }
    else { 
      p = pars.power()+1.0;
      if(abs(p) < 1.e-8) {
	std::cerr<< " FakeEv:generateValue() cant generate power=-1" << std::endl;
        return 0;
      }
      a    = pow((double) (pars.min()-pars.mean()),p);
      b    = pow((double) (pars.max()-pars.mean()),p)-a;
      tmp  = RandFlat::shoot(engine,0.,1.);
      z    = a+b*tmp;
      val  = pars.mean()+pow(z,1./p);
      return (float) val;
    }
  default:
    std::cerr << "FakeEv::generateValue(): Invalid mode " 
         << pars.mode() << "!" << std::endl;
    return 0.0;
  };
}

void FakeEv::_initializeRandomTalkTo(void) {

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

}

void FakeEv::_initializeUsePtHistTalkTo(void) {

  // Initialize the relevant submenu
  _PtHistMenu.initialize("PtHistMenu",this);
  _randomNumberMenu.initTitle("FakeEvent Pt Histogram Menu");
  commands()->append(&_PtHistMenu);

  // Add the commands to the menu
  _PtHistMenu.commands()->append(&_usePtHist);
  _PtHistMenu.commands()->append(&_lowPt);
  _PtHistMenu.commands()->append(&_highPt);
  _PtHistMenu.commands()->append(&_nbinPt);
  _PtHistMenu.commands()->append(&_valPt);
}

void FakeEv::GenPars::print(std::ostream& os) const {
  os << " Mean=" << _mean
     << " Sigma=" << _sigma
     << " Min=" << _min
     << " Max=" << _max
     << " Power=" << _power
     << " Mode= " << _mode << std::endl;
}

double FakeEv::generatePtUsingHist() {
//  double ret;
  double* val = _myValPt;
  HepRandomEngine* engine = CdfRn::Instance()->GetEngine("FAKE_EVENT");
  return CdfRn::CdfHrndm1(_lowPt.value(),_highPt.value(),
			  _nbinPt.value(),val,engine);
}
