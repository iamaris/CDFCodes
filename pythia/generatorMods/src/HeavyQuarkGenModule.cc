//------------------------------------------------------------------------------
// Description:
// ------------
//      Class HeavyQuarkGenModule
//
//      CDF interface for HeavyQuarkGen generator.
//
// Author List: S.Gromoll, Ch.Paus
//
//  revision history:
//  -----------------
// *0001 Apr 25 2002 S.Gromoll+Ch.Paus: created
// *0002 Apr 14 2003 G. Giurgiu, M. Paulini add D*0, D*+ and conjugates
//------------------------------------------------------------------------------

// C Headers
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

// C++ Headers
#include <iostream>

// Collaborating Class Headers
#include "BaBar/Cdf.hh"
#include "evt/Event.hh"
#include "cern_i/packlib.h"
#include "ParticleDB/hepevt.hh"
#include "evt/Event.hh"
#include "inc/bcs.h"
#include "r_n/CdfRn.hh"
#include <sstream>

// Local headers
#include "HeavyQuarkGen/HeavyQuarkGen.hh"
#include "generatorMods/HeavyQuarkGenModule.hh"

// Local constants, macros & typedefs ...
static const char rcsid[]  = "$Id: HeavyQuarkGenModule.cc,v 1.2 2003/04/15 18:26:09 paulini Exp $";
const long HeavyQuarkGenModule::_defaultRandomSeed1 = 9223591;
const long HeavyQuarkGenModule::_defaultRandomSeed2 =  109736;

//------------------------------------------------------------------------------
HeavyQuarkGenModule::HeavyQuarkGenModule(const char* name, const char* desc):
  AbsGenModule (name, desc),
  _Debug       ("Debug",       this,     1,       0,         4),
  _Verbose     ("Verbose",     this,     0,       0,         4),
  _doFrag      ("Fragment",    this,  true),
  _doMix       ("Mixing",      this, false),
  _Generate    ("Generate",    this),
  _SetFraction ("SetFraction", this),
  _SetBMixture ("SetBMixture", this),
  _SetBEps     ("SetBEps",     this),
  _SetDMixture ("SetDMixture", this),
  _SetDEps     ("SetDEps",     this),
  _Mixes       ("Mixes",       this),
  _Define      ("Define",      this),
  _rndmSeed1   ("RandomSeed1", this, _defaultRandomSeed1),
  _rndmSeed2   ("RandomSeed2", this, _defaultRandomSeed2)
{

  if (_Debug.value() > -1)
    cout << endl
         << "HeavyQuarkGen Version: " << rcsId()
         << endl;
  if (_Debug.value() > 0)
    cout << endl << "HeavyQuarkGenModule: Constructor" << endl << endl;

  // Initialize all the menus
  // ------------------------
  _HQgMenu.initialize ("HeavyQuarkGen",this);
  _HQgMenu.initTitle  ("HeavyQuarkGen Menu:  parameters from HeavyQuarkGen");

  // Descriptions of HeavyQuarkGen Parameter
  // -----------------------------------
  _Debug      .addDescription("\t\t\tModule's verbosity level about actions.");
  _Verbose    .addDescription("\t\t\tHeavyQuarkGen's verbosity level.");
  _doFrag     .addDescription("\t\t\tFragment - 1  Take input as is - 0");
  _doMix      .addDescription("\t\t\tApply mixing to neutral mesons?");

  commands()->append(&_HQgMenu);
  _HQgMenu.commands()->append(&_Debug       );
  _HQgMenu.commands()->append(&_Verbose     );
  _HQgMenu.commands()->append(&_doFrag      );
  _HQgMenu.commands()->append(&_doMix       );
  _HQgMenu.commands()->append(&_Generate    );
  _HQgMenu.commands()->append(&_Define      );
  _HQgMenu.commands()->append(&_SetFraction );
  _HQgMenu.commands()->append(&_SetDMixture );
  _HQgMenu.commands()->append(&_SetBMixture );
  _HQgMenu.commands()->append(&_SetDEps     );
  _HQgMenu.commands()->append(&_SetBEps     );
  _HQgMenu.commands()->append(&_Mixes       );

 // Initialize submenu for random numbers
  _randomNumberMenu.initialize("RandomNumberMenu",this);
  _randomNumberMenu.initTitle ("Random number menu");
  commands()->append(&_randomNumberMenu);
  _randomNumberMenu.commands()->append(&_rndmSeed1);
  _randomNumberMenu.commands()->append(&_rndmSeed2);

  std::ostringstream tmpSt1;
  std::ostringstream tmpSt2;

  // Describe them
  tmpSt1 << "\tSeed #1 for the random number generator"
	<< "\n\t(default " << _rndmSeed1.value() << ").";
  _rndmSeed1.addDescription(tmpSt1.str());
  tmpSt2 << "\tSeed #2 for the random number generator"
	<< "\n\t(default " << _rndmSeed2.value() << ").";
  _rndmSeed2.addDescription(tmpSt2.str());

  // Initialise and copy talk-to parameter
  HeavyQuarkGen::Instance()->Init();
}

//------------------------------------------------------------------------------
HeavyQuarkGenModule::~HeavyQuarkGenModule()
{
}

//------------------------------------------------------------------------------
// Operations
//------------------------------------------------------------------------------
AppResult HeavyQuarkGenModule::genBeginJob()
{
  CdfRn* rn = CdfRn::Instance();
  if ( !rn->isReadingFromFile() ) {
    rn->SetEngineSeeds(_rndmSeed1.value(), _rndmSeed2.value(),"HEAVYQUARKGEN");
  }

  if (_Debug.value() > 0)
    cout << endl
         << "HeavyQuarkGenModule: genBeginJob" << endl << endl;

  HeavyQuarkGen::Instance()->EnableFragmentation (_doFrag.value());
  HeavyQuarkGen::Instance()->EnableMixing        (_doMix.value());
  HeavyQuarkGen::Instance()->SetVerbose          (_Verbose.value());

  HeavyQuarkGen::Instance()->BeginJob();

  return AppResult::OK;
}

//------------------------------------------------------------------------------
AppResult HeavyQuarkGenModule::genBeginRun( AbsEvent* event )
{
  if (_Debug.value() > 0)
    cout << endl << "HeavyQuarkGenModule: genBeginRun" << endl << endl;

  if (event != NULL)
    HeavyQuarkGen::Instance()->BeginRun();
  else {
    if (_Debug.value() > 0) {
      cout << " -- AbsEvent is NULL" << endl;
      cout << " -- Nothing done." << endl;
    }
  }

  return AppResult::OK;
}


//------------------------------------------------------------------------------
int HeavyQuarkGenModule::callGenerator(AbsEvent* event)
{
  if (_Debug.value() > 0)
    cout << endl << "HeavyQuarkGenModule: callGenerator" << endl << endl;
  HeavyQuarkGen* hqgen = HeavyQuarkGen::Instance();
  hqgen->Event();
  return 0;
}


//------------------------------------------------------------------------------
AppResult HeavyQuarkGenModule::genEndRun(AbsEvent*)
{
  if (_Debug.value() > 0)
    cout << endl << "HeavyQuarkGenModule: genEndRun" << endl << endl;

  HeavyQuarkGen::Instance()->EndRun();

  return AppResult::OK;
}


AppResult HeavyQuarkGenModule::genEndJob()
{
  if (_Debug.value() > 0)
    cout << endl << endl<< "HeavyQuarkGenModule: genEndJob" << endl;

  HeavyQuarkGen::Instance()->EndJob();

  return AppResult::OK;
}

//------------------------------------------------------------------------------
void HeavyQuarkGenModule::fillHepevt()
{
  // this is done somewhere inside HeavyQuarkGen
}

//------------------------------------------------------------------------------
const char* HeavyQuarkGenModule::rcsId() const { return rcsid; }

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//
// HQDefineCommand
//
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HQDefineCommand::HQDefineCommand(const char* const comd,
                                 HeavyQuarkGenModule* targ) :
  APPCommand(comd,targ),
  _cmd ( "-idhep=0 -name= -mass=-1 -life=-1 -eps=-1 -spec_file= -spec_name=" ),
  _idhep     (   0  ),
  _name      (  ""  ),
  _mass      ( -1.0 ),
  _lifetime  ( -1.0 ),
  _epsilon   ( -1.0 ),
  _spec_file (  ""  ),
  _spec_name (  ""  )
{
}

//------------------------------------------------------------------------------
// Handler Method for HQDefineCommand
//------------------------------------------------------------------------------
int HQDefineCommand::handle(int argc, char* argv[])
{ 
  int      i;
  double  df;
  string   s;

  HeavyQuarkGenModule *targetModule = (HeavyQuarkGenModule*) target();
  AbsInterp *Interpreter = AbsInterp::theInterpreter();

  int result = Interpreter->parseArgs(_cmd,argc,argv);

  _idhep     = 0;
  _name      = "";
  _mass      = -1;
  _lifetime  = -1;
  _epsilon   = -1;
  _spec_file = "";
  _spec_name = "";

  if (result == AbsInterp::ERROR) {
    cerr << "Parser failed in HQDefineCommand." << endl;
  }
  else {
    if (Interpreter->getInt   ("idhep",     i) == AbsInterp::OK)  _idhep=i;
    if (Interpreter->getString("name",     &s) == AbsInterp::OK)  _name=s;
    if (Interpreter->getDouble("mass",     df) == AbsInterp::OK)  _mass=df;
    if (Interpreter->getDouble("life",     df) == AbsInterp::OK)  _lifetime=df;
    if (Interpreter->getDouble("eps",      df) == AbsInterp::OK)  _epsilon=df;
    if (Interpreter->getString("spec_file",&s) == AbsInterp::OK)  _spec_file=s;
    if (Interpreter->getString("spec_name",&s) == AbsInterp::OK)  _spec_name=s;
  }

  HeavyQuarkGen::Instance()->DefineParticle(_idhep, _name.c_str(), _mass,
                                            _lifetime);

  if(_spec_file != "" && _spec_name != "")
    HeavyQuarkGen::Instance()->SetSpectrum(_idhep, _spec_file.c_str(),
                                           _spec_name.c_str());

  if(_epsilon != -1)
    HeavyQuarkGen::Instance()->SetPetersonEps(_idhep, _epsilon, false);

  return(result);
}

//------------------------------------------------------------------------------
// Show Method for HQDefineCommand
//------------------------------------------------------------------------------
void HQDefineCommand::show() const
{
  cout << "-------------------------------------------------------" << endl;
  cout << "Define " << description();
  cout << endl << endl;
  cout <<"\t The actual parameter values are:" << endl;
  cout <<"\t " << endl;
  cout <<"\t    HEP id                        [-idhep]: "<<_idhep<< endl;
  cout <<"\t    Particle name                  [-name]: "<<_name<< endl;
  cout <<"\t    Particle mass                  [-mass]: "<<_mass<< endl;
  cout <<"\t    Particle lifetime              [-life]: "<<_lifetime<<endl;
  cout <<"\t    pt vs y spectrum file     [-spec_file]: "<<_spec_file<<endl;
  cout <<"\t    Name of pt vs y spectrum  [-spec_name]: "<<_spec_name<<endl;
  cout << endl;
  cout <<"\t The last two  are optional.  If a spectrum file is" << endl;
  cout <<"\t specified though a spectrum name must be specified as" << endl;
  cout <<"\t well. " << endl;
  cout << endl << endl;
}

//------------------------------------------------------------------------------
// Showable Method for HQDefineCommand
//------------------------------------------------------------------------------
bool HQDefineCommand::isShowable() const
{
  return true;
}

//------------------------------------------------------------------------------
// Description Method for HQDefineCommand
//------------------------------------------------------------------------------
string HQDefineCommand::description() const
{
  string retval("");
  retval += _cmd;
  retval += "\n\t\n";
  retval += "\t The Define command is used to define particles.         \n";
  retval += "\t A list of standard particles is provided, and can be    \n";
  retval += "\t adjusted with this command.                             \n";
  retval += "\t                                                         \n";
  retval += "\t Fully user defined particles can also be added using    \n";
  retval += "\t this command.                                           \n";
  retval += "\n";
  retval += "\t -idhep:     HEP particle ID                             \n";
  retval += "\t -name:      Particle name                               \n";
  retval += "\t -mass:      Particle mass                               \n";
  retval += "\t -spec_file: Particle spectrum file used for generation  \n";
  retval += "\t -spec_name: Name of spectrum within file                \n";
  retval += "\t -life:      Particle liftime                            \n";
  return retval;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//
// HQSetFractionCommand
//
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HQSetFractionCommand::HQSetFractionCommand(const char* const cmd,
                                          HeavyQuarkGenModule* targ) :
  APPCommand(cmd, targ),
  _cmd      ( "-hepfrom=0 -hepto=0 -frac=1.0 -anti=1" ),
  _hepfrom  (  0   ),
  _hepto    (  0   ),
  _fraction (  1.0 ),
  _anti     (  1   )
{
}

//------------------------------------------------------------------------------
// Handler Method for HQSetFractionCommand
//------------------------------------------------------------------------------
int HQSetFractionCommand::handle(int argc, char* argv[])
{ 
  string    s;
  int       i;
  double    d;

  HeavyQuarkGenModule *targetModule = (HeavyQuarkGenModule*) target();
  AbsInterp *Interpreter = AbsInterp::theInterpreter();

  int result = Interpreter->parseArgs(_cmd,argc,argv);

  _hepfrom  = 0;
  _hepto    = 0;
  _fraction = 0;
  _anti     = 1;

  if (result == AbsInterp::ERROR) {
    cerr << "Parser failed in HQSetFractionCommand." << endl;
  }
  else {
    if (Interpreter->getInt(   "hepfrom",  i) == AbsInterp::OK)  _hepfrom=i;
    if (Interpreter->getInt(   "hepto",    i) == AbsInterp::OK)  _hepto=i;
    if (Interpreter->getDouble("frac",     d) == AbsInterp::OK)  _fraction=d;
    if (Interpreter->getInt(   "anti",     i) == AbsInterp::OK)  _anti=i;
  }

  bool a = true;
  if(_anti == 0)
    a = false;
  HeavyQuarkGen::Instance()->SetFraction(_hepfrom, _hepto, _fraction, a);
  return(result);
}

//------------------------------------------------------------------------------
// Show Method for HQSetFractionCommand
//------------------------------------------------------------------------------
void HQSetFractionCommand::show() const
{
  cout << "-------------------------------------------------------" << endl;
  cout << "SetFraction " << description();
  cout << endl << endl;
  cout <<"\t The actual parameter values are:" << endl;
  cout <<"\t " << endl;
  cout <<"\t    HEP id of fragmenting particle [-from]: "<< _hepfrom<< endl;
  cout <<"\t    HEP id of fragmented particle    [-to]: "<< _hepto  << endl;
  cout <<"\t    Fraction                       [-frac]: "<<_fraction<< endl;
  cout <<"\t    Also set anti particle         [-anti]: "<<_anti    << endl;
  cout << endl;
  cout <<"\t Set the fraction used to fragment -from to -to.   " << endl;
  cout <<"\t -from and -to are HEP ids. -anti specifies whether" << endl;
  cout <<"\t the anti particle fractions should be set as well," << endl;
  cout <<"\t and defaults to true if not otherwise specified.  " << endl;
  cout << endl << endl;
}

//------------------------------------------------------------------------------
// Showable Method for HQSetFractionCommand
//------------------------------------------------------------------------------
bool HQSetFractionCommand::isShowable() const
{
  return true;
}

//------------------------------------------------------------------------------
// Description Method for HQSetFractionCommand
//------------------------------------------------------------------------------
string HQSetFractionCommand::description() const
{
  string retval("");
  retval += _cmd;
  retval += "\n\t\n";
  retval += "\t The SetFraction command is used to set the        \n";
  retval += "\t production fraction for a particle, used for      \n";
  retval += "\t fragmentation.                                    \n";
  retval += "\t -from and -to are the HEP ids.  -anti specifies   \n";
  retval += "\t whether the anti particle fractions should be set \n";
  retval += "\t as well, and defaults to true if not otherwise    \n";
  retval += "\t specified.                                        \n";
  retval += "\t E.g., to set the fractions of B_s and anti-B_s    \n";
  retval += "\t produced in the fragmentation of b and bbar quarks\n";
  retval += "\t use -from=-5 -to=531 -frac=0.11                   \n";
  retval += "\n";
  retval += "\t -from:     HEP id of fragmenting particle        \n";
  retval += "\t -to  :     HEP id of fragmented particle         \n";
  retval += "\t -frac:     Relative production fraction of the   \n";
  retval += "\t              particle for fragmentation.         \n";
  retval += "\t -anti:     Non-zero if the anti particle         \n";
  retval += "\t              fractions should also be set        \n";
  retval += "\t              (default), or zero if they should   \n";
  retval += "\t              not be set.                         \n";
  return retval;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//
// HQMixesCommand
//
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HQMixesCommand::HQMixesCommand(const char* const cmd,
                               HeavyQuarkGenModule* targ) :
  APPCommand(cmd, targ),
  _cmd      ( "-idhep=0 -deltam=0 -idhep_to=0 "),
  _idhep    (  0   ),
  _delta_m  (  0   ),
  _mixes_to (  0   )
{
}

//------------------------------------------------------------------------------
// Handler Method for HQMixesCommand
//------------------------------------------------------------------------------
int HQMixesCommand::handle(int argc, char* argv[])
{ 
  int       i;
  double    d;

  HeavyQuarkGenModule *targetModule = (HeavyQuarkGenModule*) target();
  AbsInterp *Interpreter = AbsInterp::theInterpreter();

  int result = Interpreter->parseArgs(_cmd,argc,argv);

  _idhep    = 0;
  _delta_m  = 0;
  _mixes_to = 0;

  if (result == AbsInterp::ERROR) {
    cerr << "Parser failed in HQMixesCommand." << endl;
  }
  else {
    if (Interpreter->getInt("idhep",    i) == AbsInterp::OK)  _idhep=i;
    if (Interpreter->getDouble("deltam",d) == AbsInterp::OK)  _delta_m=d;
    if (Interpreter->getInt("idhep_to", i) == AbsInterp::OK)  _mixes_to=i;
  }

  HeavyQuarkGen::Instance()->Mixes(_idhep, _delta_m, _mixes_to);
  return(result);
}

//------------------------------------------------------------------------------
// Show Method for HQMixesCommand
//------------------------------------------------------------------------------
void HQMixesCommand::show() const
{
  cout << "-------------------------------------------------------" << endl;
  cout << "Mixes " << description();
  cout << endl << endl;
  cout <<"\t The actual parameter values are:" << endl;
  cout <<"\t " << endl;
  cout <<"\t    HEP id of particle            [-idhep]: "<< _idhep  <<endl;
  cout <<"\t    Mixing parameter             [-deltam]: "<< _delta_m<<endl;
  cout <<"\t    HEP id of mixed particle   [-idhep_to]: "<<_mixes_to<<endl;
  cout << endl;
  cout <<"\t Specify mixing for a particle.  The option        " << endl;
  cout <<"\t idhep_to is optional.  If omitted, assumes that   " << endl;
  cout <<"\t the HEP id of the mixed particle is negative the  " << endl;
  cout <<"\t HEP id of the unmixed particle.                   " << endl;
  cout << endl << endl;
}

//------------------------------------------------------------------------------
// Showable Method for HQMixesCommand
//------------------------------------------------------------------------------
bool HQMixesCommand::isShowable() const
{
  return true;
}

//------------------------------------------------------------------------------
// Description Method for HQMixesCommand
//------------------------------------------------------------------------------
string HQMixesCommand::description() const
{
  string retval("");
  retval += _cmd;
  retval += "\n\t\n";
  retval += "\t The Mixes command is used to specify that a particle \n";
  retval += "\t can mix.                                             \n";
  retval += "\n";
  retval += "\t -idhep:     HEP id of particle                       \n";
  retval += "\t -deltam:    Mixing parameter                         \n";
  retval += "\t -idhtp_to:  HEP id of anti-particle.  Defaults to    \n";
  retval += "\t               negative of idhep.                     \n";
  return retval;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//
// HQGenerateCommand
//
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Constructor for HQGenerateCommand
//------------------------------------------------------------------------------
HQGenerateCommand::HQGenerateCommand(const char* const comd,
                                     HeavyQuarkGenModule* targ) :
  APPCommand(comd,targ),
  _cmd    ( "-idhep=0 -frac=1.0 -ptmin=0 -ptmax=10000 -rapmin=-1000 -rapmax=1000"),
  _idhep  (    0  ),
  _fraction(  1.0 ),
  _pt_min (   0.0 ),
  _pt_max (  100000 ),
  _rap_min( -1000 ),
  _rap_max(  1000 )
{
}

//------------------------------------------------------------------------------
// Handler Method for HQGenerateCommand
//------------------------------------------------------------------------------
int HQGenerateCommand::handle(int argc, char* argv[])
{
  int     i;
  double  d;

  HeavyQuarkGenModule *targetModule = (HeavyQuarkGenModule*) target();
  AbsInterp *Interpreter = AbsInterp::theInterpreter();

  int result = Interpreter->parseArgs(_cmd,argc,argv);

  _idhep    = 0;
  _fraction = 1.0;
  _pt_min   = 0;
  _pt_max   =  100000;
  _rap_min  = -100000;
  _rap_max  =  100000;

  if (result == AbsInterp::ERROR) {
    cerr << "Parser failed in HQGenerateCommand." << endl;
  }
  else {
    if (Interpreter->getInt   ("idhep",i)  == AbsInterp::OK) { _idhep=i; }
    if (Interpreter->getDouble("frac",d)   == AbsInterp::OK) { _fraction=d; }
    if (Interpreter->getDouble("ptmin",d)  == AbsInterp::OK) { _pt_min=d; }
    if (Interpreter->getDouble("ptmax",d)  == AbsInterp::OK) { _pt_max=d; }
    if (Interpreter->getDouble("rapmin", d)== AbsInterp::OK) { _rap_min=d; }
    if (Interpreter->getDouble("rapmax", d)== AbsInterp::OK) { _rap_max=d; }
  }

  HeavyQuarkGen::Instance()->Generate(_idhep,   _fraction,
                                      _pt_min,  _pt_max,
                                      _rap_min, _rap_max);
  return(result);
}

//------------------------------------------------------------------------------
// Show Method for HQGenerateCommand
//------------------------------------------------------------------------------
void HQGenerateCommand::show() const
{
  cout << "-------------------------------------------------------" << endl;
  cout << "Generate " << description();
  cout << endl << endl;
  cout <<"\t The actual parameter values are:" << endl;
  cout <<"\t " << endl;
  cout <<"\t    HEP id to generate        [-idhep]: " << _idhep   << endl;
  cout <<"\t    Fraction to generate       [-frac]: " << _fraction<< endl;
  cout <<"\t    Min pt for particle       [-ptmin]: " << _pt_min  << endl;
  cout <<"\t    Max pt for particle       [-ptmax]: " << _pt_max  << endl;
  cout <<"\t    Min rapidity for particle  [-ymin]: " << _rap_min << endl;
  cout <<"\t    Max rapidity for particle  [-ymax]: " << _rap_max << endl;
  cout <<"\t " << endl << endl;
}

//------------------------------------------------------------------------------
// Showable Method for HQGenerateCommand
//------------------------------------------------------------------------------
bool HQGenerateCommand::isShowable() const
{
  return true;
}

//------------------------------------------------------------------------------
// Description Method for HQGenerateCommand
//------------------------------------------------------------------------------
string HQGenerateCommand::description() const
{
  string retval("");
  retval += _cmd;
  retval += "\n\t\n";
  retval += "\t Add a particle to be generated.  The particle must be \n";
  retval += "\t defined and will be generated using the spectrum      \n";
  retval += "\t defined for the particle.                             \n";
  return retval;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//
// HQSetDMixture   (shortcut for SetFraction command)
//
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HQSetDMixtureCommand::HQSetDMixtureCommand(const char* const cmd,
                                           HeavyQuarkGenModule* targ) :
  APPCommand(cmd, targ),
  _cmd ( "-mixu=0 -mixd=0 -mixs=0 -mixl=0 -mixustar=0, -mixdstar=0" ),
  _mixu( 0.00 ),
  _mixd( 0.00 ),
  _mixs( 0.00 ),
  _mixl( 0.00 ),
  _mixustar( 0.00 ),
  _mixdstar( 0.00 )
{
}

//------------------------------------------------------------------------------
// Handler Method for HQSetDMixtureCommand
//------------------------------------------------------------------------------
int HQSetDMixtureCommand::handle(int argc, char* argv[])
{ 
  double    d;

  HeavyQuarkGenModule *targetModule = (HeavyQuarkGenModule*) target();
  AbsInterp *Interpreter = AbsInterp::theInterpreter();

  int result = Interpreter->parseArgs(_cmd,argc,argv);

  _mixu = 0;
  _mixd = 0;
  _mixs = 0;
  _mixl = 0;
  _mixustar = 0;
  _mixdstar = 0;

  if (result == AbsInterp::ERROR) {
    cerr << "Parser failed in HQSetDMixtureCommand." << endl;
  }
  else {
    if (Interpreter->getDouble("mixu",  d) == AbsInterp::OK)  _mixu = d;
    if (Interpreter->getDouble("mixd",  d) == AbsInterp::OK)  _mixd = d;
    if (Interpreter->getDouble("mixs",  d) == AbsInterp::OK)  _mixs = d;
    if (Interpreter->getDouble("mixl",  d) == AbsInterp::OK)  _mixl = d;
    if (Interpreter->getDouble("mixustar",  d) == AbsInterp::OK)  _mixustar = d;
    if (Interpreter->getDouble("mixdstar",  d) == AbsInterp::OK)  _mixdstar = d;
  }

  HeavyQuarkGen::Instance()->SetDMixture(_mixu, _mixd, _mixs, _mixl, _mixustar, _mixdstar);
  return(result);
}

//------------------------------------------------------------------------------
// Show Method for HQSetDMixtureCommand
//------------------------------------------------------------------------------
void HQSetDMixtureCommand::show() const
{
  cout << "-------------------------------------------------------" << endl;
  cout << "SetDMixture " << description();
  cout << endl << endl;
  cout <<"\t The actual parameter values are:" << endl;
  cout <<"\t " << endl;
  cout <<"\t D0     admixture             [-mixu]: " << _mixu << endl;
  cout <<"\t D+     admixture             [-mixd]: " << _mixd << endl;
  cout <<"\t Ds     admixture             [-mixs]: " << _mixs << endl;
  cout <<"\t Lambda admixture             [-mixl]: " << _mixl << endl;
  cout <<"\t D*0    admixture             [-mixustar]: " << _mixustar << endl;
  cout <<"\t D*+    admixture             [-mixdstar]: " << _mixdstar << endl;
  cout << endl << endl;
}

//------------------------------------------------------------------------------
// Showable Method for HQSetDMixtureCommand
//------------------------------------------------------------------------------
bool HQSetDMixtureCommand::isShowable() const
{
  return true;
}

//------------------------------------------------------------------------------
// Description Method for HQSetDMixtureCommand
//------------------------------------------------------------------------------
string HQSetDMixtureCommand::description() const
{
  string retval("");
  retval += _cmd;
  retval += "\n\t\n";
  retval += "\t The SetDMixture command is a shortcut for the     \n";
  retval += "\t SetFraction command, and is used to set all the   \n";
  retval += "\t fractions for c fragmentation.                    \n";
  retval += "\n";
  retval += "\t\t\t -mixu:           admixture of D0              \n";
  retval += "\t\t\t -mixd:           admixture of D+              \n";
  retval += "\t\t\t -mixs:           admixture of Ds              \n";
  retval += "\t\t\t -mixl:           admixture of Lambda D        \n";
  retval += "\t\t\t -mixustar:       admixture of D*0             \n";
  retval += "\t\t\t -mixdstar:       admixture of D*+             \n";

  return retval;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//
// HQSetBMixture   (shortcut for SetFraction command)
//
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HQSetBMixtureCommand::HQSetBMixtureCommand(const char* const cmd,
                                           HeavyQuarkGenModule* targ) :
  APPCommand(cmd, targ),
  _cmd ( "-mixu=0 -mixd=0 -mixs=0 -mixc=0 -mixl=0" ),
  _mixu( 0.00 ),
  _mixd( 0.00 ),
  _mixs( 0.00 ),
  _mixc( 0.00 ),
  _mixl( 0.00 )
{
}

//------------------------------------------------------------------------------
// Handler Method for HQSetBMixtureCommand
//------------------------------------------------------------------------------
int HQSetBMixtureCommand::handle(int argc, char* argv[])
{ 
  double    d;

  HeavyQuarkGenModule *targetModule = (HeavyQuarkGenModule*) target();
  AbsInterp *Interpreter = AbsInterp::theInterpreter();

  int result = Interpreter->parseArgs(_cmd,argc,argv);

  _mixu = 0;
  _mixd = 0;
  _mixs = 0;
  _mixc = 0;
  _mixl = 0;

  if (result == AbsInterp::ERROR) {
    cerr << "Parser failed in HQSetBMixtureCommand." << endl;
  }
  else {
    if (Interpreter->getDouble("mixu",  d) == AbsInterp::OK)  _mixu = d;
    if (Interpreter->getDouble("mixd",  d) == AbsInterp::OK)  _mixd = d;
    if (Interpreter->getDouble("mixs",  d) == AbsInterp::OK)  _mixs = d;
    if (Interpreter->getDouble("mixc",  d) == AbsInterp::OK)  _mixc = d;
    if (Interpreter->getDouble("mixl",  d) == AbsInterp::OK)  _mixl = d;
  }

  HeavyQuarkGen::Instance()->SetBMixture(_mixu, _mixd, _mixs, _mixc, _mixl);
  return(result);
}

//------------------------------------------------------------------------------
// Show Method for HQSetBMixtureCommand
//------------------------------------------------------------------------------
void HQSetBMixtureCommand::show() const
{
  cout << "-------------------------------------------------------" << endl;
  cout << "SetBMixture " << description();
  cout << endl << endl;
  cout <<"\t The actual parameter values are:" << endl;
  cout <<"\t " << endl;
  cout <<"\t Bu     admixture             [-mixu]: " << _mixu << endl;
  cout <<"\t Bd     admixture             [-mixd]: " << _mixd << endl;
  cout <<"\t Bs     admixture             [-mixs]: " << _mixs << endl;
  cout <<"\t Bc     admixture             [-mixc]: " << _mixc << endl;
  cout <<"\t Lambda admixture             [-mixl]: " << _mixl << endl;
  cout << endl << endl;
}

//------------------------------------------------------------------------------
// Showable Method for HQSetBMixtureCommand
//------------------------------------------------------------------------------
bool HQSetBMixtureCommand::isShowable() const
{
  return true;
}

//------------------------------------------------------------------------------
// Description Method for HQSetBMixtureCommand
//------------------------------------------------------------------------------
string HQSetBMixtureCommand::description() const
{
  string retval("");
  retval += _cmd;
  retval += "\n\t\n";
  retval += "\t The SetBMixture command is a shortcut for the     \n";
  retval += "\t SetFraction command, and is used to set all the   \n";
  retval += "\t fractions for b fragmentation.                    \n";
  retval += "\n";
  retval += "\t\t\t -mixu:      admixture of Bu                   \n";
  retval += "\t\t\t -mixd:      admixture of Bd                   \n";
  retval += "\t\t\t -mixs:      admixture of Bs                   \n";
  retval += "\t\t\t -mixc:      admixture of Bc                   \n";
  retval += "\t\t\t -mixl:      admixture of Lambda B             \n";

  return retval;
}




//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//
// HQSetDEps   (shortcut for SetFraction command)
//
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HQSetDEpsCommand::HQSetDEpsCommand(const char* const cmd,
                                           HeavyQuarkGenModule* targ) :
  APPCommand(cmd, targ),
  _cmd ( "-epsu=0 -epsd=0 -epss=0 -epsl=0, -epsustar=0, -epsdstar=0" ),
  _epsu( 0.00 ),
  _epsd( 0.00 ),
  _epss( 0.00 ),
  _epsl( 0.00 ),
  _epsustar( 0.00 ),
  _epsdstar( 0.00 )
{
}

//------------------------------------------------------------------------------
// Handler Method for HQSetDEpsCommand
//------------------------------------------------------------------------------
int HQSetDEpsCommand::handle(int argc, char* argv[])
{ 
  double    d;

  HeavyQuarkGenModule *targetModule = (HeavyQuarkGenModule*) target();
  AbsInterp *Interpreter = AbsInterp::theInterpreter();

  int result = Interpreter->parseArgs(_cmd,argc,argv);

  _epsu = 0;
  _epsd = 0;
  _epss = 0;
  _epsl = 0;
  _epsustar = 0;
  _epsdstar = 0;

  if (result == AbsInterp::ERROR) {
    cerr << "Parser failed in HQSetDEpsCommand." << endl;
  }
  else {
    if (Interpreter->getDouble("epsu",  d) == AbsInterp::OK)  _epsu = d;
    if (Interpreter->getDouble("epsd",  d) == AbsInterp::OK)  _epsd = d;
    if (Interpreter->getDouble("epss",  d) == AbsInterp::OK)  _epss = d;
    if (Interpreter->getDouble("epsl",  d) == AbsInterp::OK)  _epsl = d;
    if (Interpreter->getDouble("epsustar",  d) == AbsInterp::OK)  _epsustar = d;
    if (Interpreter->getDouble("epsdstar",  d) == AbsInterp::OK)  _epsdstar = d;
  }

  HeavyQuarkGen::Instance()->SetDEps(_epsu, _epsd, _epss, _epsl, _epsustar, _epsdstar);
  return(result);
}

//------------------------------------------------------------------------------
// Show Method for HQSetDEpsCommand
//------------------------------------------------------------------------------
void HQSetDEpsCommand::show() const
{
  cout << "-------------------------------------------------------" << endl;
  cout << "SetDEps " << description();
  cout << endl << endl;
  cout <<"\t The actual parameter values are:" << endl;
  cout <<"\t " << endl;
  cout <<"\t Peterson eps for D0              [-epsu]: "      << _epsu << endl;
  cout <<"\t Peterson eps for D+              [-epsd]: "      << _epsd << endl;
  cout <<"\t Peterson eps for Ds              [-epss]: "      << _epss << endl;
  cout <<"\t Peterson eps for Lambda          [-epsl]: "      << _epsl << endl;
  cout <<"\t Peterson eps for D*0             [-epsustar]: "  << _epsustar << endl;
  cout <<"\t Peterson eps for D*+             [-epsdstar]: "  << _epsdstar << endl;
  cout << endl << endl;
}

//------------------------------------------------------------------------------
// Showable Method for HQSetDEpsCommand
//------------------------------------------------------------------------------
bool HQSetDEpsCommand::isShowable() const
{
  return true;
}

//------------------------------------------------------------------------------
// Description Method for HQSetDEpsCommand
//------------------------------------------------------------------------------
string HQSetDEpsCommand::description() const
{
  string retval("");
  retval += _cmd;
  retval += "\n\t\n";
  retval += "\t The SetDEps command is a shortcut for the         \n";
  retval += "\t Define -eps= command, and is used to set all the  \n";
  retval += "\t peterson epsilons for D meson fragmentation.      \n";
  retval += "\n";
  retval += "\t\t\t -epsu:           peterson eps for D0               \n";
  retval += "\t\t\t -epsd:           peterson eps for D+               \n";
  retval += "\t\t\t -epss:           peterson eps for Ds               \n";
  retval += "\t\t\t -epsl:           peterson eps for Lambda D         \n";
  retval += "\t\t\t -epsustar:       peterson eps for D*0              \n";
  retval += "\t\t\t -epsdstar:       peterson eps for D*+              \n";

  return retval;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//
// HQSetBEps   (shortcut for SetFraction command)
//
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HQSetBEpsCommand::HQSetBEpsCommand(const char* const cmd,
                                           HeavyQuarkGenModule* targ) :
  APPCommand(cmd, targ),
  _cmd ( "-epsu=0 -epsd=0 -epsc=0 -epss=0 -epsl=0" ),
  _epsu( 0.00 ),
  _epsd( 0.00 ),
  _epsc( 0.00 ),
  _epss( 0.00 ),
  _epsl( 0.00 )
{
}

//------------------------------------------------------------------------------
// Handler Method for HQSetBEpsCommand
//------------------------------------------------------------------------------
int HQSetBEpsCommand::handle(int argc, char* argv[])
{ 
  double    d;

  HeavyQuarkGenModule *targetModule = (HeavyQuarkGenModule*) target();
  AbsInterp *Interpreter = AbsInterp::theInterpreter();

  int result = Interpreter->parseArgs(_cmd,argc,argv);

  _epsu = 0;
  _epsd = 0;
  _epsc = 0;
  _epss = 0;
  _epsl = 0;

  if (result == AbsInterp::ERROR) {
    cerr << "Parser failed in HQSetBEpsCommand." << endl;
  }
  else {
    if (Interpreter->getDouble("epsu",  d) == AbsInterp::OK)  _epsu = d;
    if (Interpreter->getDouble("epsd",  d) == AbsInterp::OK)  _epsd = d;
    if (Interpreter->getDouble("epsc",  d) == AbsInterp::OK)  _epsc = d;
    if (Interpreter->getDouble("epss",  d) == AbsInterp::OK)  _epss = d;
    if (Interpreter->getDouble("epsl",  d) == AbsInterp::OK)  _epsl = d;
  }

  HeavyQuarkGen::Instance()->SetBEps(_epsu, _epsd, _epsc, _epss, _epsl);
  return(result);
}

//------------------------------------------------------------------------------
// Show Method for HQSetBEpsCommand
//------------------------------------------------------------------------------
void HQSetBEpsCommand::show() const
{
  cout << "-------------------------------------------------------" << endl;
  cout << "SetBEps " << description();
  cout << endl << endl;
  cout <<"\t The actual parameter values are:" << endl;
  cout <<"\t " << endl;
  cout <<"\t Peterson eps for B+              [-epsu]: " << _epsu << endl;
  cout <<"\t Peterson eps for B0              [-epsd]: " << _epsd << endl;
  cout <<"\t Peterson eps for Bc              [-epsc]: " << _epsc << endl;
  cout <<"\t Peterson eps for Bs              [-epss]: " << _epss << endl;
  cout <<"\t Peterson eps for Lambda          [-epsl]: " << _epsl << endl;
  cout << endl << endl;
}

//------------------------------------------------------------------------------
// Showable Method for HQSetBEpsCommand
//------------------------------------------------------------------------------
bool HQSetBEpsCommand::isShowable() const
{
  return true;
}

//------------------------------------------------------------------------------
// Description Method for HQSetBEpsCommand
//------------------------------------------------------------------------------
string HQSetBEpsCommand::description() const
{
  string retval("");
  retval += _cmd;
  retval += "\n\t\n";
  retval += "\t The SetBEps command is a shortcut for the         \n";
  retval += "\t Define -eps= command, and is used to set all the  \n";
  retval += "\t peterson epsilons for B meson fragmentation.      \n";
  retval += "\n";
  retval += "\t\t\t -epsu:      peterson eps for B+            \n";
  retval += "\t\t\t -epsd:      peterson eps for B0            \n";
  retval += "\t\t\t -epsc:      peterson eps for Bc            \n";
  retval += "\t\t\t -epss:      peterson eps for Bs            \n";
  retval += "\t\t\t -epsl:      peterson eps for Lambda B      \n";

  return retval;
}

