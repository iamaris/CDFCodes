//--------------------------------------------------------------------------
// File and Version Information:
// 	QQModule.cc
//
// Description:
//	Class QQModule
//
//      C++ interface for CDF QQ Module
//
// Environment:
//	Software developed for the CDFII Detector
//
// Author List:
//	Marjorie Shapiro		Original Author
//
//  Revision history:
//  -----------------
// *0001 Sep 10 1998 P.Murat: remove obsolete copies between YBOS and TRYBOS
//                            records
//       Aug 29 2001 lena: remove unnesessary include;
//       Aug 30 2001 lena: make include in standard way; added genId and ct
//       oct 02 2001 lena: moved actual qq from FrameMods/GeneratorModule here.
//       Dec 07 2001 lena: inherited AbsDecpackModule
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "generatorMods/QQModule.hh"

//---------------
// C++ Headers --
//---------------
#include <iostream>
#include <sstream>

//-------------------------------
// Collaborating Class Headers --
//------------------------------- 
#include "BaBar/Cdf.hh"
#include "inc/bcs.h"
#include "evt/evt.h"
#include "evt/Event.hh"
#include "stdhep_i/CdfHepevt.hh"
#include "qq_i/QQInterface.hh"
#include "stdhep_i/Heplun.hh"
#include "r_n/CdfRn.hh"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

const char* QQModule::genId = "QQModule";
const long QQModule::_defaultRandomSeed1 = 9223591;
const long QQModule::_defaultRandomSeed2 = 109736;

//----------------
// Constructors --
//----------------

QQModule::QQModule()
  : AbsDecpackModule( QQModule::genId, "AC++ QQ Module" ), 
  _decbud("Decay_B+_and_B0", this, 1), 
  _decbs ("Decay_Bs", this, 1),
  _decbc ("Decay_Bc", this, 0),
  _decbb ("Decay_Upsilon", this, 0),
  _decbyn("Decay_B_Baryons", this,0),
  _dechrm("Decay_prompt_charm", this,0),
  _lowest("Decay_only_lowest", this, 0), 
  _lnglif("Decay_K_s_Lambda", this, 0), 
  _nolife("No_lifetime_for_input", this, 0),
  _forceCP("force_CP"             , this, false),
  _qqI( 0 ),
  _randomSeed1("RandomSeed1",this,QQModule::_defaultRandomSeed1),
  _randomSeed2("RandomSeed2",this,QQModule::_defaultRandomSeed2)
{
					// Add parameters to list of command 
					// handlers
  commands( )->append( &_decbud );
  commands( )->append( &_decbs  );
  commands( )->append( &_decbc  );
  commands( )->append( &_decbb  );
  commands( )->append( &_decbyn );
  commands( )->append( &_dechrm );
  commands( )->append( &_lowest );
  commands( )->append( &_lnglif );
  commands( )->append( &_nolife );
  commands( )->append( &_forceCP );

 // Initialize the relevant submenu
  _randomNumberMenu.initialize("RandomNumberMenu",this);
  _randomNumberMenu.initTitle("Random number menu");
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

QQModule::~QQModule() {
  if (_qqI) delete _qqI;
}

//--------------
// Operations --
//--------------

AppResult QQModule::genBeginJob()
{
   _qqI = new QQInterface();   

  CdfRn* rn = CdfRn::Instance();
  if ( !rn->isReadingFromFile() ) {
    rn->SetEngineSeeds(_randomSeed1.value(), _randomSeed2.value(),"QQModule");
  }
  
  Heplun heplun;
  if (!heplun.lnhout()) heplun.lnhout()=6; 

  // initialize QQ
  _qqI->setDecayBud(_decbud.value());
  _qqI->setDecayBs( _decbs.value());
  _qqI->setDecayBc( _decbc.value());
  _qqI->setDecayUpsilon(_decbb.value());
  _qqI->setDecayBBaryon(_decbyn.value());
  _qqI->setDecayPromptCharm(_dechrm.value());
  _qqI->setDecayOnlyLowest(_lowest.value());
  _qqI->setDecayKsLambda(_lnglif.value());
  _qqI->setNoLifeInput(_nolife.value());
  _qqI->setForceCP(_forceCP.value());
  _qqI->grandInit();
  _qqI->dumpParameters();
  return AppResult::OK;
}

AppResult QQModule::genEndJob()
{
  return AppResult::OK;
}

AppResult QQModule::genBeginRun( AbsEvent* anEvent )
{
  return AppResult::OK;
}

AppResult QQModule::genEndRun( AbsEvent* anEvent )
{
  return AppResult::OK;
}

int QQModule::callGenerator( AbsEvent* anEvent ) 
{
  _qqI->event();
  return 1;
}




