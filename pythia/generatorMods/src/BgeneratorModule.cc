//------------------------------------------------------------------------------
// Description:
// ------------
//      Class BgeneratorModule
//
// Author List: K.Anikeev, P.Murat, Ch.Paus
//
//  revision history:
//  -----------------
// *0001 Feb 08 1999 K.Anikeev: created
// *0002 Jul 14 1999 Ch.Paus:   created AC++ talk-to
// *0003 Feb 26 2001 Ch.Paus:   ability to switch off fragmentation
// *0004 Mar 06 2001 Ch.Paus:   package Bgenerator
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
#include "Bgenerator/Bgenerator.hh"
#include "generatorMods/BgeneratorModule.hh"

// Local constants, macros & typedefs ...
static const char rcsid[]  = "$Id: BgeneratorModule.cc,v 1.13 2007/07/03 00:37:28 paus Exp $";
const long BgeneratorModule::_defaultRandomSeed1 = 9223591;
const long BgeneratorModule::_defaultRandomSeed2 =  109736;

//------------------------------------------------------------------------------
extern "C" {
  void  obsppt_   (int *lun);
  void  obsvpt_   (int *lun);
  void  genppt_   (int *lun);
  void  obsbtc_   (int *ierr);
  void  gnpbtc_   (int *ierr);
  void  stdhepctb_();
  void  stdhepgnp_(int* ierr);
}

//------------------------------------------------------------------------------
BgeneratorModule::BgeneratorModule(const BgeneratorModule&):
  AbsGenModule("Bgenerator::copy","ROOT Bgenerator module::copy"),
  _VERBOSE  ("VERBOSE",    this,    0,     0,         4),
  _PT_MIN_B1("PT-MIN-b1",  this,  5.0,   0.0,     100.0),
  _PT_MIN_B2("PT-MIN-b2",  this,  5.0,   0.0,     100.0),
  _RAPMIN   ("RAPMIN-b1",  this, -2.0, -10.0,      10.0),
  _RAPMAX   ("RAPMAX-b1",  this,  2.0, -10.0,      10.0),
  _FRAGMENT ("FRAGMENT",   this,    1,     0,         1),
  _EPS      ("EPS-b",      this,0.006,0.0001,       1.0),
  _MASS_BQ  ("BMASS",      this, 4.75,   0.0,      10.0),
  _MASS_CQ  ("CMASS",      this, 1.50,   0.0,      10.0),
  _DOMIX    ("MIXING",     this, false),
  _X_D      ("X_D",        this, 0.72,   0.0,     999.9),
  _X_S      ("X_S",        this, 15.0,   0.0,     999.9),
  _DEFINE   ("DEFINE",     this),
  _NDE      ("NDE",        this),
  _MNR      ("MNR",        this),
  _BMESON   ("BMESON",     this),
  _CMESON   ("CMESON",     this),
  _rndmSeed1("RandomSeed1",this,BgeneratorModule::_defaultRandomSeed1),
  _rndmSeed2("RandomSeed2",this,BgeneratorModule::_defaultRandomSeed2) {
  if (_VERBOSE.value() > 0)
    cout << endl << "BgeneratorModule: Constructor" << endl << endl;
}

//------------------------------------------------------------------------------
BgeneratorModule::BgeneratorModule(const char* name, const char* desc):
  AbsGenModule(name, desc),
  _VERBOSE  ("VERBOSE",    this,    0,     0,         4),
  _PT_MIN_B1("PT-MIN-b1",  this,  5.0,   0.0,     100.0),
  _PT_MIN_B2("PT-MIN-b2",  this,  5.0,   0.0,     100.0),
  _RAPMIN   ("RAPMIN-b1",  this, -2.0, -10.0,      10.0),
  _RAPMAX   ("RAPMAX-b1",  this,  2.0, -10.0,      10.0),
  _FRAGMENT ("FRAGMENT",   this,    1,     0,         1),
  _EPS      ("EPS-b",      this,0.006,0.0001,       1.0),
  _MASS_BQ  ("BMASS",      this, 4.75,   0.0,      10.0),
  _MASS_CQ  ("CMASS",      this, 1.50,   0.0,      10.0),
  _DOMIX    ("MIXING",     this, false),
  _X_D      ("X_D",        this, 0.72,   0.0,     999.9),
  _X_S      ("X_S",        this, 15.0,   0.0,     999.9),
  _DEFINE   ("DEFINE",     this),
  _NDE      ("NDE",        this),
  _MNR      ("MNR",        this),
  _BMESON   ("BMESON",     this),
  _CMESON   ("CMESON",     this),
  _rndmSeed1("RandomSeed1",this,_defaultRandomSeed1),
  _rndmSeed2("RandomSeed2",this,_defaultRandomSeed2) {

  if (_VERBOSE.value() > -1)
    cout << endl
         << "Bgenerator Version: " << rcsId()
         << endl;
  if (_VERBOSE.value() > 0)
    cout << endl << "BgeneratorModule: Constructor" << endl << endl;

  // Initialize all the menus
  // ------------------------
  _BgMenu.initialize ("Bgenerator",this);
  _BgMenu.initTitle  ("Bgenerator Menu:  parameters from Bgenerator");

  // Descriptions of Bgenerator Parameter
  // -----------------------------------
  _VERBOSE  .addDescription("\t\t\tModule's verbosity level about actions.");
  _PT_MIN_B1.addDescription("\t\t\tMinimum b quark pt (first b) [GeV]");
  _PT_MIN_B2.addDescription("\t\t\tMinimum b quark pt (second b) [GeV]");
  _RAPMIN   .addDescription("\t\t\tMimimum b quark rapidity (first b)");
  _RAPMAX   .addDescription("\t\t\tMaximum b quark rapidity (first b)");
  _FRAGMENT .addDescription("\t\t\tFragment - 1  Take input as is - 0");
  _EPS      .addDescription("\t\t\tPeterson epsilon (b quark fragmentation)");
  _MASS_BQ  .addDescription("\t\t\tBottom quark mass [GeV]");
  _MASS_CQ  .addDescription("\t\t\tCharm quark mass (needed for Bc) [GeV]");
  _DOMIX    .addDescription("\t\t\tApply mixing to neutral mesons?");
  _X_D      .addDescription("\t\t\tEnter Xd");
  _X_S      .addDescription("\t\t\tEnter Xs");

  commands()->append(&_BgMenu);
  _BgMenu.commands()->append(&_VERBOSE  );
  _BgMenu.commands()->append(&_PT_MIN_B1);
  _BgMenu.commands()->append(&_PT_MIN_B2);
  _BgMenu.commands()->append(&_RAPMIN   );
  _BgMenu.commands()->append(&_RAPMAX   );
  _BgMenu.commands()->append(&_EPS      );
  _BgMenu.commands()->append(&_MASS_BQ  );
  _BgMenu.commands()->append(&_MASS_CQ  );
  _BgMenu.commands()->append(&_FRAGMENT );
  _BgMenu.commands()->append(&_DOMIX    );
  _BgMenu.commands()->append(&_X_D      );
  _BgMenu.commands()->append(&_X_S      );
  _BgMenu.commands()->append(&_DEFINE   );
  _BgMenu.commands()->append(&_NDE      );
  _BgMenu.commands()->append(&_MNR      );
  _BgMenu.commands()->append(&_BMESON   );
  _BgMenu.commands()->append(&_CMESON   );

  // Initialize submenu for random numbers
  _randomNumberMenu.initialize("RandomNumberMenu",this);
  _randomNumberMenu.initTitle ("Random number menu");
  commands()->append(&_randomNumberMenu);
  _randomNumberMenu.commands()->append(&_rndmSeed1);
  _randomNumberMenu.commands()->append(&_rndmSeed2);

  std::ostringstream tmpSt1;
  std::ostringstream tmpSt2;

  // Describe them
  tmpSt1 << "\t\t\t\tSeed #1 for the random number generator"
	<< "\n\t\t\t(default " << _rndmSeed1.value() << ").";
  _rndmSeed1.addDescription(tmpSt1.str());
  tmpSt2 << "\t\t\t\tSeed #2 for the random number generator"
	<< "\n\t\t\t(default " << _rndmSeed2.value() << ").";
  _rndmSeed2.addDescription(tmpSt2.str());

  // Initialise and copy talk-to parameter
  Bgenerator::Instance()->Init();
  AbsParm2Bgen();
}

//------------------------------------------------------------------------------
BgeneratorModule::~BgeneratorModule( ) {
}

//------------------------------------------------------------------------------
// Operations
//------------------------------------------------------------------------------
AppResult BgeneratorModule::genBeginJob( ) {

  CdfRn* rn = CdfRn::Instance();
  if ( !rn->isReadingFromFile() ) {
    rn->SetEngineSeeds(_rndmSeed1.value(), _rndmSeed2.value(),"BGENERATOR");
  }

  if (_VERBOSE.value() > 0)
    cout << endl
         << "BgeneratorModule: genBeginJob" << endl << endl;

  AbsParm2Bgen();

  Bgenerator::Instance()->BeginJob();

  if (_VERBOSE.value() > 0)
    Bgenerator::Instance()->Show();

  return AppResult::OK;
}

//------------------------------------------------------------------------------
AppResult BgeneratorModule::genBeginRun( AbsEvent* event ) {

  if (_VERBOSE.value() > 0)
    cout << endl << "BgeneratorModule: genBeginRun" << endl << endl;

  if (event != NULL)
    Bgenerator::Instance()->BeginRun();
  else {
    if (_VERBOSE.value() > 0) {
      cout << " -- AbsEvent is NULL" << endl;
      cout << " -- Nothing done." << endl;
    }
  }

  return AppResult::OK;
}


//------------------------------------------------------------------------------
int BgeneratorModule::callGenerator( AbsEvent* event ) {

  if (_VERBOSE.value() > 0)
    cout << endl << "BgeneratorModule: callGenerator" << endl << endl;

  Bgenerator* bgen = Bgenerator::Instance();

  bgen->Event();
  return 0;
}


//------------------------------------------------------------------------------
AppResult BgeneratorModule::genEndRun( AbsEvent* ) {

  if (_VERBOSE.value() > 0)
    cout << endl << "BgeneratorModule: genEndRun" << endl << endl;

  return AppResult::OK;
}


AppResult BgeneratorModule::genEndJob() {

  if (_VERBOSE.value() > 0)
    cout << endl << endl<< "BgeneratorModule: genEndJob" << endl;

  Bgenerator::Instance()->EndRun();

  return AppResult::OK;
}

//------------------------------------------------------------------------------
void BgeneratorModule::fillHepevt() {
  // this is done somewhere inside Bgenerator
}

//------------------------------------------------------------------------------
void BgeneratorModule::Bgen2AbsParm() {
  // convert Bgen Common Blocks to the AbsParms of the Bgenerator Module

  if (_VERBOSE.value() > 0)
    cout << endl << endl<< "BgeneratorModule: Bgen2AbsParm" << endl;

  Bgncom_t* bgc = Bgenerator::Instance()->GetBgncom();

  _PT_MIN_B1.set(bgc->PTMN[0]);
  _PT_MIN_B1.set(bgc->PTMN[1]);
  _RAPMIN   .set(bgc->RAPMIN);
  _RAPMAX   .set(bgc->RAPMAX);
  _EPS      .set(bgc->EPS);
  _MASS_BQ  .set(bgc->MASS_BQ);
  _MASS_CQ  .set(bgc->MASS_CQ);
  _FRAGMENT .set(bgc->FRAGMENT);
  _DOMIX    .set(bgc->DOMIX);
  _X_D      .set(bgc->X_D);
  _X_S      .set(bgc->X_S);
}

//------------------------------------------------------------------------------
void BgeneratorModule::AbsParm2Bgen() {
  // convert AbsParms of the Bgenerator Module to the Bgen Common Blocks

  if (_VERBOSE.value() > 0)
    cout << endl << endl<< "BgeneratorModule: AbsParm2Bgen" << endl;

  Bgncom_t* bgc = Bgenerator::Instance()->GetBgncom();

  bgc->PTMN[0]  = _PT_MIN_B1.value();
  bgc->PTMN[1]  = _PT_MIN_B2.value();
  bgc->RAPMIN   = _RAPMIN   .value();
  bgc->RAPMAX   = _RAPMAX   .value();
  bgc->EPS      = _EPS      .value();
  bgc->MASS_BQ  = _MASS_BQ  .value();
  bgc->MASS_CQ  = _MASS_CQ  .value();
  bgc->FRAGMENT = _FRAGMENT .value();
  bgc->DOMIX    = _DOMIX    .value();
  bgc->X_D      = _X_D      .value();
  bgc->X_S      = _X_S      .value();
}

//------------------------------------------------------------------------------
const char* BgeneratorModule::rcsId() const { return rcsid; }

//------------------------------------------------------------------------------
// Constructor for DefineCommand
//------------------------------------------------------------------------------
DefineCommand::DefineCommand(const char* const comd,
                             BgeneratorModule* targ) :
  APPCommand(comd,targ),
  _cmd      ( "-ipart=0 -bmass=-1. -blife=-1. -bidhep=-999 -name= -aname=" ),
  _ipart    ( 0 ),
  _bm       ( -1. ),
  _bl       ( -1. ),
  _bidhep   ( 9999 ),
  _name     ( "ND" ),
  _aname    ( "ND" )
{
}

//------------------------------------------------------------------------------
// Handler Method for DefineCommand
//------------------------------------------------------------------------------
int DefineCommand::handle(int argc, char* argv[])
{
  int     i;
  double  df;
  string  s;

  BgeneratorModule *targetModule = (BgeneratorModule*) target();
  AbsInterp *Interpreter = AbsInterp::theInterpreter();

  int result = Interpreter->parseArgs(_cmd,argc,argv);

  if (result == AbsInterp::ERROR) {
    cerr << "Parser failed in DefineCommand." << endl;
  }
  else {
    if (Interpreter->getInt   ("ipart" ,i)  == AbsInterp::OK) { _ipart=i; }
    if (Interpreter->getDouble("bmass" ,df) == AbsInterp::OK) { _bm=df; }
    if (Interpreter->getDouble("blife" ,df) == AbsInterp::OK) { _bl=df; }
    if (Interpreter->getInt   ("bidhep",i)  == AbsInterp::OK) { _bidhep=i; }
    if (Interpreter->getString("name"  ,&s) == AbsInterp::OK) { _name=s; }
    if (Interpreter->getString("aname" ,&s) == AbsInterp::OK) { _aname=s; }
  }

  Bgenerator::Instance()->DefineParticle(_ipart,_bidhep,
                                          _name.c_str(),_aname.c_str(),_bm,_bl);
  return(result);
}

//------------------------------------------------------------------------------
// Show Method for DefineCommand
//------------------------------------------------------------------------------
void DefineCommand::show() const
{
  cout << "DEFINE " << description();
  cout << endl << endl;
  cout <<"\t\t\t The actual parameter values are:" << endl;
  cout <<"\t\t\t " << endl;
  cout <<"\t\t\t    Particle to define      [-ipart]: " << _ipart << endl;
  cout <<"\t\t\t    B particle mass         [-bmass]: " << _bm << endl;
  cout <<"\t\t\t    B particle lifetime     [-blife]: " << _bl << endl;
  cout <<"\t\t\t    HEP id                 [-bidhep]: " << _bidhep << endl;
  cout <<"\t\t\t    Particle name            [-name]: " << _name << endl;
  cout <<"\t\t\t    Anti-particle name      [-aname]: " << _aname << endl;
  cout <<"\t\t\t " << endl;
  cout <<"\t\t\t The three last parameters are only necessary when a" << endl;
  cout <<"\t\t\t user defines a new particle (-ipart=6)." << endl;
  cout <<"\t\t\t " << endl;
}

//------------------------------------------------------------------------------
// Showable Method for DefineCommand
//------------------------------------------------------------------------------
bool DefineCommand::isShowable() const
{
  return true;
}

//------------------------------------------------------------------------------
// Description Method for DefineCommand
//------------------------------------------------------------------------------
string DefineCommand::description() const
{
  string retval("");
  retval += _cmd;
  retval += "\n\t\t\t\n";
  retval += "\t\t\t The DEFINE command is used to define the B particles.   \n";
  retval += "\t\t\t A list of standard particles can be produced and there  \n";
  retval += "\t\t\t mass and liftime can be adjusted.                       \n";
  retval += "\t\t\t                                                         \n";
  retval += "\t\t\t Also a fully user defined particle can be created which \n";
  retval += "\t\t\t needs somewhat more input.                              \n";
  retval += "\t\t\t                                                         \n";
  retval += "\t\t\t \n";
  retval += "\t\t\t -ipart:     bgen particle ID (1-5: 6 user defined)      \n";
  retval += "\t\t\t -bidhep:    hep particle ID                             \n";
  retval += "\t\t\t -name:      particle name                               \n";
  retval += "\t\t\t -aname:     anti-particle name                          \n";
  retval += "\t\t\t -bmass:     b particle mass                             \n";
  retval += "\t\t\t -blife:     b particle liftime                          \n";
  return retval;
}

//------------------------------------------------------------------------------
// Constructor for NdeCommand
//------------------------------------------------------------------------------
NdeCommand::NdeCommand(const char* const comd, BgeneratorModule* targ) :
  APPCommand(comd,targ),
  _cmd      ( "-gentype=0 -file=no-file -ihist=-1 -bxsec=-1." ),
  _gentype  ( 1 ),
  _file     ( "NOT-DEFINED" ),
  _ihist    ( 20 ),
  _bxsec    ( 0.0 )
{
}

//------------------------------------------------------------------------------
// Handler Method for NdeCommand
//------------------------------------------------------------------------------
int NdeCommand::handle(int argc, char* argv[])
{
  int     i;
  double  df;
  string  s;

  BgeneratorModule *targetModule = (BgeneratorModule*) target();
  AbsInterp *Interpreter = AbsInterp::theInterpreter();

  int result = Interpreter->parseArgs(_cmd,argc,argv);

  if (result == AbsInterp::ERROR) {
    cerr << "Parser failed in NdeCommand." << endl;
  }
  else {
    if (Interpreter->getInt   ("gentype",i)  == AbsInterp::OK) { _gentype=i; }
    if (Interpreter->getString("file"   ,&s) == AbsInterp::OK) { _file=s; }
    if (Interpreter->getInt   ("ihist"  ,i)  == AbsInterp::OK) { _ihist=i; }
    if (Interpreter->getDouble("bxsec"  ,df) == AbsInterp::OK) { _bxsec=df; }
  }

  Bgenerator::Instance()->DefineNde(_gentype,_file.c_str(),_ihist,_bxsec);

  return(result);
}

//------------------------------------------------------------------------------
// Show Method for NdeCommand
//------------------------------------------------------------------------------
void NdeCommand::show() const
{
  cout << "NDE " << description();
  cout << endl << endl;
  cout <<"\t\t\t The actual parameter values are:" << endl;
  cout <<"\t\t\t " << endl;
  cout <<"\t\t\t    Generation type       [-gentype]: " << _gentype << endl;
  cout <<"\t\t\t    Spectrum file name       [-file]: " << _file << endl;
  cout <<"\t\t\t    Histogram ID            [-ihist]: " << _ihist << endl;
  cout <<"\t\t\t    Total B cross section   [-bxsec]: " << _bxsec << endl;
  cout <<"\t\t\t " << endl;
  cout <<"\t\t\t The three last parameters are only necessary when a" << endl;
  cout <<"\t\t\t user defined spectrum is used." << endl;
  cout <<"\t\t\t " << endl;
}

//------------------------------------------------------------------------------
// Showable Method for NdeCommand
//------------------------------------------------------------------------------
bool NdeCommand::isShowable() const
{
  return true;
}

//------------------------------------------------------------------------------
// Description Method for NdeCommand
//------------------------------------------------------------------------------
string NdeCommand::description() const
{
  string retval("");
  retval += _cmd;
  retval += "\n\t\t\t\n";
  retval += "\t\t\t The NDE command is used to specify the b-quark spectrum.\n";
  retval += "\t\t\t Using this command automatically means that only one    \n";
  retval += "\t\t\t b-quark per event is generated. The spectrum follow     \n";
  retval += "\t\t\t calculations of Nason, Dawson and Ellis.                \n";
  retval += "\t\t\t \n";
  retval += "\t\t\t -gentype: spectrum type (1-9) see bgenerator print out  \n";
  retval += "\t\t\t -file:    file where to read spectrum histogram         \n";
  retval += "\t\t\t -ihist:   histogram ID for spectrum                     \n";
  retval += "\t\t\t -bxsec:   total b cross section                         \n";
  return retval;
}

//------------------------------------------------------------------------------
// Constructor for MnrCommand
//------------------------------------------------------------------------------
MnrCommand::MnrCommand(const char* const comd, BgeneratorModule* targ) :
  APPCommand(comd,targ),
  _cmd      ( "-gentype=0 -file=no-file -ihistpp=-1 -ihistdp=-1 -ihistyy=-1" ),
  _gentype  ( 1 ),
  _file     ( "NOT-DEFINED" ),
  _ihistpp  ( 200000 ),
  _ihistdp  ( 320000 ),
  _ihistyy  ( 210000 )
{
}

//------------------------------------------------------------------------------
// Handler Method for MnrCommand
//------------------------------------------------------------------------------
int MnrCommand::handle(int argc, char* argv[])
{
  int     i;
  string  s;

  BgeneratorModule *targetModule = (BgeneratorModule*) target();
  AbsInterp *Interpreter = AbsInterp::theInterpreter();

  int result = Interpreter->parseArgs(_cmd,argc,argv);

  if (result == AbsInterp::ERROR) {
    cerr << "Parser failed in MnrCommand." << endl;
  }
  else {
    if (Interpreter->getInt   ("gentype",i)  == AbsInterp::OK) { _gentype=i; }
    if (Interpreter->getString("file"   ,&s) == AbsInterp::OK) { _file=s; }
    if (Interpreter->getInt   ("ihistpp",i)  == AbsInterp::OK) { _ihistpp=i; }
    if (Interpreter->getInt   ("ihistdp",i)  == AbsInterp::OK) { _ihistdp=i; }
    if (Interpreter->getInt   ("ihistyy",i)  == AbsInterp::OK) { _ihistyy=i; }
  }

  Bgenerator::Instance()->DefineMnr(_gentype,_file.c_str(),
                                     _ihistpp,_ihistdp,_ihistyy);
  return(result);
}

//------------------------------------------------------------------------------
// Show Method for MnrCommand
//------------------------------------------------------------------------------
void MnrCommand::show() const
{
  cout << "MNR " << description();
  cout << endl << endl;
  cout <<"\t\t\t The actual parameter values are:" << endl;
  cout <<"\t\t\t " << endl;
  cout <<"\t\t\t    Generation type       [-gentype]: " << _gentype << endl;
  cout <<"\t\t\t    Spectrum file name       [-file]: " << _file << endl;
  cout <<"\t\t\t    Hist ID Ptmn vs Ptmx  [-ihistpp]: " << _ihistpp << endl;
  cout <<"\t\t\t    Hist ID delta-phi     [-ihistdp]: " << _ihistdp << endl;
  cout <<"\t\t\t    Hist ID Y* vs Yboost  [-ihistyy]: " << _ihistyy << endl;
  cout <<"\t\t\t " << endl;
  cout <<"\t\t\t The four last parameters are only necessary when a" << endl;
  cout <<"\t\t\t user defined spectrum is used." << endl;
  cout <<"\t\t\t " << endl;
}

//------------------------------------------------------------------------------
// Showable Method for MnrCommand
//------------------------------------------------------------------------------
bool MnrCommand::isShowable() const
{
  return true;
}

//------------------------------------------------------------------------------
// Description Method for MnrCommand
//------------------------------------------------------------------------------
string MnrCommand::description() const
{
  string retval("");
  retval += _cmd;
  retval += "\n\t\t\t\n";
  retval += "\t\t\t The MNR command is used to specify the b-quark spectrum.\n";
  retval += "\t\t\t Using this command automatically means that two         \n";
  retval += "\t\t\t b-quarks per event are generated. The spectra follow    \n";
  retval += "\t\t\t calculations of Mangano, Nason and Ridolfi.             \n";
  retval += "\t\t\t \n";
  retval += "\t\t\t -gentype: spectrum type (1-9) see bgenerator print out  \n";
  retval += "\t\t\t -file:    file where to read spectrum histogram         \n";
  retval += "\t\t\t -ihistpp: histogram ID - Ptmin vs Ptmax                 \n";
  retval += "\t\t\t -ihistdp: histogram ID - delta-phi                      \n";
  retval += "\t\t\t -ihistyy: histogram ID - Y* vs Yboost                   \n";
  return retval;
}


//------------------------------------------------------------------------------
// Constructor for BMesonCommand
//------------------------------------------------------------------------------
BMesonCommand::BMesonCommand(const char* const comd,
                             BgeneratorModule* targ) :
  APPCommand(comd,targ),
  _cmd    ( "-gencode=1 -bmeson=1 -mixu=0. -mixd=0. -mixs=0. -mixc=0. -mixl=0. -mixx=0." ),
  _gencode( 1 ),
  _bmeson ( 1 ),
  _mixu   ( 0.0 ),
  _mixd   ( 0.0 ),
  _mixs   ( 0.0 ),
  _mixc   ( 0.0 ),
  _mixl   ( 0.0 ),
  _mixx   ( 0.0 )
{
}

//------------------------------------------------------------------------------
// Handler Method for BMesonCommand
//------------------------------------------------------------------------------
int BMesonCommand::handle(int argc, char* argv[])
{
  int     i;
  double  df;

  BgeneratorModule *targetModule = (BgeneratorModule*) target();
  AbsInterp *Interpreter = AbsInterp::theInterpreter();

  int result = Interpreter->parseArgs(_cmd,argc,argv);

  if (result == AbsInterp::ERROR) {
    cerr << "Parser failed in DefineCommand." << endl;
  }
  else {
    if (Interpreter->getInt   ("gencode",i)  == AbsInterp::OK) { _gencode=i; }
    if (Interpreter->getInt   ("bmeson" ,i)  == AbsInterp::OK) { _bmeson=i; }
    if (Interpreter->getDouble("mixu"   ,df) == AbsInterp::OK) { _mixu=df; }
    if (Interpreter->getDouble("mixd"   ,df) == AbsInterp::OK) { _mixd=df; }
    if (Interpreter->getDouble("mixs"   ,df) == AbsInterp::OK) { _mixs=df; }
    if (Interpreter->getDouble("mixc"   ,df) == AbsInterp::OK) { _mixc=df; }
    if (Interpreter->getDouble("mixl"   ,df) == AbsInterp::OK) { _mixl=df; }
    if (Interpreter->getDouble("mixx"   ,df) == AbsInterp::OK) { _mixx=df; }
  }

  Bgenerator::Instance()->DefineMix(_gencode,_bmeson,
                                     _mixu,_mixd,_mixs,_mixc,_mixl,_mixx);
  return(result);
}

//------------------------------------------------------------------------------
// Show Method for BMesonCommand
//------------------------------------------------------------------------------
void BMesonCommand::show() const
{
  cout << "BMESON " << description();
  cout << endl << endl;
  cout <<"\t\t\t The actual parameter values are:" << endl;
  cout <<"\t\t\t " << endl;
  cout <<"\t\t\t    Generation code       [-gencode]: " << _gencode << endl;
  cout <<"\t\t\t    B meson ID             [-bmeson]: " << _bmeson << endl;
  cout <<"\t\t\t    Admixtures of Bu         [-mixu]: " << _mixu << endl;
  cout <<"\t\t\t    Admixtures of Bd         [-mixd]: " << _mixd << endl;
  cout <<"\t\t\t    Admixtures of Bs         [-mixs]: " << _mixs << endl;
  cout <<"\t\t\t    Admixtures of Bc         [-mixc]: " << _mixc << endl;
  cout <<"\t\t\t    Admixtures of LambdaB    [-mixl]: " << _mixl << endl;
  cout <<"\t\t\t    Admixtures of User B     [-mixx]: " << _mixx << endl;
  cout <<"\t\t\t " << endl;
  cout <<"\t\t\t The last parameter is only necessary when a user" << endl;
  cout <<"\t\t\t defined particle exists." << endl;
  cout <<"\t\t\t " << endl;
  cout <<"\t\t\t The B meson ID is only used in case the generation" << endl;
  cout <<"\t\t\t code is lower 3. For generation code 3 the complete" << endl;
  cout <<"\t\t\t admixtures have to be specified." << endl << endl;
}

//------------------------------------------------------------------------------
// Showable Method for BMesonCommand
//------------------------------------------------------------------------------
bool BMesonCommand::isShowable() const
{
  return true;
}

//------------------------------------------------------------------------------
// Description Method for BMesonCommand
//------------------------------------------------------------------------------
string BMesonCommand::description() const
{
  string retval("");
  retval += _cmd;
  retval += "\n\t\t\t\n";
  retval += "\t\t\t The BMESON command is used to determine the B           \n";
  retval += "\t\t\t particle(s) to be generated.                            \n";
  retval += "\t\t\t For -gencode=1 only the with bmeson given particle will \n";
  retval += "\t\t\t be generated; for -gencode=2 also its anti-particle.    \n";
  retval += "\t\t\t For -gencode=3 the admixtures will be used to generate  \n";
  retval += "\t\t\t all particles and anti-particles with the given         \n";
  retval += "\t\t\t probabilities.                                          \n";
  retval += "\t\t\t                                                         \n";
  retval += "\t\t\t \n";
  retval += "\t\t\t -gencode:   generation code (1-3)                       \n";
  retval += "\t\t\t -bmeson:    B meson to be generated (gencodes 1+2)      \n";
  retval += "\t\t\t -mixu:      admixture of Bu                             \n";
  retval += "\t\t\t -mixd:      admixture of Bd                             \n";
  retval += "\t\t\t -mixs:      admixture of Bs                             \n";
  retval += "\t\t\t -mixc:      admixture of Bc                             \n";
  retval += "\t\t\t -mixl:      admixture of Lambda B                       \n";
  retval += "\t\t\t -mixx:      admixture of user defined particle          \n";
  return retval;
}

//------------------------------------------------------------------------------
// Constructor for CMesonCommand
//------------------------------------------------------------------------------
CMesonCommand::CMesonCommand(const char* const comd,
                             BgeneratorModule* targ) :
  APPCommand(comd,targ)
{
  _active = false;
}

//------------------------------------------------------------------------------
// Handler Method for CMesonCommand
//------------------------------------------------------------------------------
int CMesonCommand::handle(int argc, char* argv[])
{
  int     i;
  double  df;

  _active = true;
  Bgenerator::Instance()->InitAsCgenerator();

  return AbsInterp::OK;
}

//------------------------------------------------------------------------------
// Show Method for CMesonCommand
//------------------------------------------------------------------------------
void CMesonCommand::show() const
{
  cout << "CMESON " << description();
  cout << endl << endl;
  cout <<"\t\t\t The CMESON option is ";
  if (_active) cout << "ACTIVE." << endl << endl;
  else cout << "NOT active." << endl << endl;
}

//------------------------------------------------------------------------------
// Showable Method for CMesonCommand
//------------------------------------------------------------------------------
bool CMesonCommand::isShowable() const
{
  return true;
}

//------------------------------------------------------------------------------
// Description Method for CMesonCommand
//------------------------------------------------------------------------------
string CMesonCommand::description() const
{
  string retval("");
  retval += _cmd;
  retval += "\n\t\t\t\n";
  retval += "\t\t\t Issue CMESON command in order to configure Bgenerator\n";
  retval += "\t\t\t to generate c quarks/mesons. This is needed in order\n";
  retval += "\t\t\t to generate and admixture of c-mesons, while a single\n";
  retval += "\t\t\t c-meson can be generated as a user defined particle.\n";
  return retval;
}

