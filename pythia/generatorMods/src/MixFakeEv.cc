#ifdef __GNUG__
#pragma implementation
#endif

//------------------------
// String Header First  --
//------------------------
#include <string>

#include <iostream> 

#include <sstream>
using std::ostream;
using std::cout;
using std::endl;
using std::ostringstream ;

#include "inc/misc.hh"

//-----------------------
// This Class's Header --
//-----------------------
#include "generatorMods/MixFakeEv.hh"

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "evt/Event.hh"
#include "stdhep_i/CdfHepevt.hh"
#include "ParticleDB/ParticleDb.hh"
#include "Framework/APPFramework.hh"
#include "AbsEnv/AbsEnv.hh"

// CLHEP Random Number headers
#include "r_n/CdfRn.hh"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGaussT.h"
#include "CLHEP/Random/RandomEngine.h"
#include "SimulationObjects/HEPG_StorableBank.hh"
#include "Edm/Handle.hh"
//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

const long MixFakeEv::_defaultRandomSeed1 = 922883591;
const long MixFakeEv::_defaultRandomSeed2 = 109735476;
const char* MixFakeEv::genId = "MIX_FAKE_EVENT";
HepRandomEngine* MixFakeEv::mixFakeEvEngine = 0;

//-->--> default constructor

MixFakeEv::MixFakeEv() : 
  AbsGenModule( MixFakeEv::genId, "Single Particle Gun Module" ),
  _useCommand("use",this),
  _genCommand("generate",this),
  _randomSeed1("RandomSeed1",this,MixFakeEv::_defaultRandomSeed1),
  _randomSeed2("RandomSeed2",this,MixFakeEv::_defaultRandomSeed2),
  _genMom(50.,0.,  1.,100.,0.,1), // default parameters
  _genPt(50.,0.,  1.,100.,0.,1),
  _genTheta(90.,0.,-45., 45.,0.,1),
  _genEta(0.0,0., -1.,  1.,0.,2),
  _genPhi(7.5,0.,  0.,360.,0.,2)
{

  commands()->append(&_useCommand);
  commands()->append(&_genCommand);
  _initializeRandomTalkTo();
                                        // <a name="qq">
                                        // Fill default values for data
                                        // Order of arguments to init:  
                                        // value, sigma, minRange, maxRange, power
                                        // (all floats)
                                        // mode=1. means use gaussian distribution
                                        // mode=2  means flat or power law 
                                        // distribution

                                        // by default simulate 1 particle/event
  NParticles = 1;
                                        // generate in PT rather than P
  UsePt      = 1;
                                        // Generate in Eta not Theta
  UseEta     = 1;
                                        // default particle is Pi+
  CdfCode    = 401;

}

//--------------
// Destructor --
//--------------

MixFakeEv::~MixFakeEv() {
}

//------------------------------------------------------------------------------
//  user interface handler for FAKE_EVENT_MODULE
//------------------------------------------------------------------------------

static int process_show_command(MixFakeEv* module, int argc, char** argv) {
  char cmd[100];

  for (int i=0; i<argc; i++) {
    for (int j=0; j<=strlen(argv[i]); j++) cmd[j]=toupper(argv[i][j]);

    if      (strcmp(cmd,"P") == 0) {
      module->genMom()->print();
    }
    else if (strcmp(cmd,"PT") == 0) {
      module->genPt()->print();
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
    else if (strcmp(cmd,"ALL") == 0) {
      module->genMom()->print();
      module->genPt()->print();
      module->genTheta()->print();
      module->genEta()->print();
      module->genPhi()->print();

      std::cout << "CdfCode= " << module->getCdfCode() << std::endl;
    }
  }
  return 1;
}



static int process_set_command(MixFakeEv* module, int argc, char** argv) {

                                        // "FORTRAN equivalence"
  MixFakeEv::GenPars*  genpar;
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
  else if (strcmp(cmd,"THETA") == 0) {
    genpar = module->genTheta();
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
      if(ipar[i-1]<1 || ipar[i-1]>2) {
        std::cout << " illegal mode " << ipar[i-1] << " set to " << 1 << std::endl;
        ipar[i-1] = 1;
      }
    }
    else {
      std::cout << "Too many parameters given:  only 1st 6 used\n";
    }
  }
  return 1;
}



static void process_use_command(MixFakeEv* module, int argc, char** argv) {
  char cmd[100];
  for (int i=0; i<argc; i++) {
    for (int j=0; j<=strlen(argv[i]); j++) cmd[j]=toupper(argv[i][j]);
    if (strcmp(cmd,"PT") == 0) {
      module->setUsePt(1);
    }
    else if (strcmp(cmd,"P") == 0) {
      module->setUsePt(0);
    }
    else if (strcmp(cmd,"ETA") == 0) {
      module->setUseEta(1);
    }
    else if (strcmp(cmd,"THETA") == 0) {
      module->setUseEta(0);
    }
  }
}



void MixFakeEv::menuHandler(char* menu, char* command, int argc, char** argv) {
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


int MixFakeEv::callGenerator(AbsEvent* event) {

  CdfHepevt* hepevt_my = CdfHepevt::Instance();
  hepevt_my->clearCommon();

  double pt, p, eta, theta, phi;
  double px, py, pz;

  for (int i=0; i<NParticles; i++) {
                                        // Generate Eta or Theta
    if(UseEta) {
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
      pt   = generateValue(_genPt);
      p    = pt/sin(theta);
    }
    else {
      p    = generateValue(_genMom);
      pt   = p*sin(theta);
    }
                                        // define particle 4-momentum in the 
                                        // vertex
    px     = pt*cos(phi);
    py     = pt*sin(phi);
    pz     = p*cos(theta);
                                        // add particle to /HEPEVT/

    double mass = ParticleDb::Instance()->Mass(CdfCode);

    Hepevt_t* hepevt = CdfHepevt::Instance()->HepevtPtr();

    hepevt->IDHEP[i]     = ParticleDb::Instance()->ParticlePdgCode(CdfCode);

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

void MixFakeEv::fillHepevt() { }
//void MixFakeEv::fillHepevt() { writeHEPGbank();}
/*
void  MixFakeEv::writeHEPGbank() {
  
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
*/
//------------------------------------------------------------------------------
void MixFakeEv::PrintState(std::ostream& output) const {
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
  if (UseEta == 1){
    output << "     Eta      "; _genEta.print(output);
  }
  else {
    output << "     Theta    "; _genTheta.print(output);
  }
  output <<   "     Phi      "; _genPhi.print(output);
}

//______________________________________________________________________________
MixFakeEvCommand::MixFakeEvCommand() {
}

//______________________________________________________________________________
MixFakeEvCommand::~MixFakeEvCommand(){
}

//______________________________________________________________________________
MixFakeEvCommand::MixFakeEvCommand(const char* const theCommand, 
       MixFakeEv* theTarget) : APPCommand( theCommand, theTarget)
{
  _thisModule = theTarget;
}

//______________________________________________________________________________
int MixFakeEvCommand::handle(int argc, char* argv[])
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
void MixFakeEv::Set(char* nm, float mean, float sigma, float pmax, float pmin,
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
  else if (strcmp(name,"THETA") == 0) {
    par = &_genTheta;
    UseEta = 0;
  }
  else if (strcmp(name,"ETA") == 0) {
    par = &_genEta;
    UseEta = 1;
  }
  else if (strcmp(name,"PHI") == 0) {
    par = &_genPhi;
  }
  else {
    std::cout << " MixFakeEv::Set detected wrong parameter name : " << nm << std::endl;
    return;
  }
  par->init(mean,sigma,pmin,pmax,power,mode);
}


//______________________________________________________________________________
void MixFakeEvCommand::show() const {
  // Only want to print things once.
  if(strncmp(command(),"generate",8)==0) {
   _thisModule->PrintState();
  }
}

//------------------------------------------------------------------------------
bool MixFakeEvCommand::isShowable() const {return true;}

//______________________________________________________________________________
string MixFakeEvCommand::description() const {
  string retval;
  if(strncmp(command(),"generate",8)==0) {
    retval += "possible syntaxes are:: \n";
    retval += "\t\t generate PARAM_NAME mean sigma pmin pmax power mode\n";
    retval += "\t\t where PARAM_NAME is one of P, PT, ETA, THETA, PHI\n";
    retval += "\t\t and mode=1 means Gaussian, mode=2 means power law \n";
    retval += "\t\t note:  PHI is in degrees not radians!!\n";
    retval += "\t OR: \n";
    retval += "\t\t generate CDFCODE code\n";
    retval += "\t OR: \n";
    retval += "\t\t NPARTICLES number_of_particles";
  }
  else if(strncmp(command(),"use",3)==0) {
    retval += "specify if generation is in P or PT and/or THETA or ETA\n";
    retval += "\t\t syntax is:  use variable\n";
    retval += "\t\t where variable means P, PT, THETA, ETA\n";
  }
  else {
    std::cout << " unknown command " << command() << std::endl;
  }
  
  return retval;
}

//_____________________________________________________________________________
AppResult MixFakeEv::genBeginRun(AbsEvent* run) {
  return AppResult::OK;
}

//_____________________________________________________________________________
AppResult MixFakeEv::genEndRun(AbsEvent* run) {
  return AppResult::OK;
}


//_____________________________________________________________________________
AppResult MixFakeEv::genBeginJob() { 
  CdfRn* rn = CdfRn::Instance();
  if ( !rn->isReadingFromFile() ) {
    rn->SetEngineSeeds(_randomSeed1.value(), _randomSeed2.value(),MixFakeEv::genId);
  }

  MixFakeEv::mixFakeEvEngine = CdfRn::Instance()->GetEngine("FAKE_EVENT");

  return AppResult::OK;
}

//_____________________________________________________________________________
AppResult MixFakeEv::genEndJob() {
  return AppResult::OK;
}
    
float MixFakeEv::generateValue(const GenPars& pars) const {
  HepRandomEngine* engine = MixFakeEv::mixFakeEvEngine;
  switch (pars.mode()) {
  case 1: // Gaussian
    if (!pars.sigma())
      return pars.mean();
    else {
      float tmp = 0.0;
      do {
        tmp = RandGaussT::shoot( engine, pars.mean(), pars.sigma() );
      } while // allow ranges to apply to gaussian, where non-zero
        ((pars.min() || pars.max()) && (tmp < pars.min() || tmp > pars.max()));
      return tmp;
    }
  case 2: // Flat
  case 3:
    if (!pars.power()) {
      return RandFlat::shoot(engine,pars.min(),pars.max());
    }
    else
    { 
      double p = pars.power()+1.0;
      if(abs(p) < 1.e-8) {
        std::cerr<< " MixFakeEv:generateValue() cant generate power=-1" << std::endl;
        return 0;
      }
      double a = pow( (double) pars.min(),p);
      double b = pow( (double) pars.max(),p)-a;
      double tmp = RandFlat::shoot(engine,0.,1.);
      double z = a+b*tmp;
      double val = pow(z,1./p);
      return (float) val;
    }
  default:
    std::cerr << "MixFakeEv::generateValue(): Invalid mode " 
         << pars.mode() << "!" << std::endl;
    return 0.0;
  };

}


void MixFakeEv::_initializeRandomTalkTo(void) {

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

void MixFakeEv::GenPars::print(std::ostream& os) const {
  os << " Mean=" << _mean
     << " Sigma=" << _sigma
     << " Min=" << _min
     << " Max=" << _max
     << " Power=" << _power
     << " Mode= " << _mode << std::endl;
}


