//--------------------------------------------------------------------------
// File and Version Information:
//
// Description:
//	Class QQModule
//
//	Interface to CDF QQ Module
//
// Environment:
//	Software developed for the CDF at FNAL.
//
// Author List:
//	Marjorie Shapiro		Original Author
//
// Copyright Information:
//	Copyright (C) 1997		LBL
//
//  revision history:
//  -----------------
// *0001 Jan 23 1998 P.Murat: add menuHandler for Python interface
// aug 30 2001 lena: added genId and ct.

// oct 02 2001 lena: moved actual qq from FrameMods/GeneratorModule here.
//------------------------------------------------------------------------

#ifndef __QQMODULE_HH
#define __QQMODULE_HH

//----------------------
// Base Class Headers --
//----------------------
#include "generatorMods/AbsDecpackModule.hh"
#include "Framework/AbsParmGeneral.hh"
#ifdef CDF
#include "BaBar/Cdf.hh"
#endif
typedef AbsParmGeneral<long> AbsParmGeneral_long;

class QQInterface;

class QQModule : public AbsDecpackModule {

//--------------------
// Instance Members --
//--------------------

public:
  static const char* genId;
					// Constructors
  QQModule();

					// Destructor
  virtual ~QQModule( );
				
  AppResult            genBeginJob();
  AppResult            genEndJob();
  AppResult            genBeginRun( AbsEvent* anEvent );
  AppResult            genEndRun( AbsEvent* anEvent );
  int                  callGenerator( AbsEvent* anEvent );

protected:

  AbsParmGeneral<int> _decbud, _decbs,_decbc,_decbb,_decbyn,_dechrm;
  AbsParmGeneral<int> _lowest, _lnglif, _nolife;
  AbsParmGeneral<bool>  _forceCP;  // CP eigenstate forcing mech

  // Random number menu
  APPMenu _randomNumberMenu;

  // Initial random number seeds (2) (ignored if restoreState is requested)
  AbsParmGeneral_long _randomSeed1;
  static const   long _defaultRandomSeed1;
  AbsParmGeneral_long _randomSeed2;
  static const   long _defaultRandomSeed2;

private:

  QQInterface* _qqI;

  QQModule( const char* const theName, const char* const theDescription ) 
    : AbsDecpackModule( theName, theDescription ), 
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
    _randomSeed1("RandomSeed",this,QQModule::_defaultRandomSeed1),
    _randomSeed2("RandomSeed",this,QQModule::_defaultRandomSeed2)
  {}
  
  QQModule( const QQModule& m) 
    : AbsDecpackModule( "x", "x" ), 
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
    _randomSeed1("RandomSeed",this,QQModule::_defaultRandomSeed1),
    _randomSeed2("RandomSeed",this,QQModule::_defaultRandomSeed2)
  {}
  
};

#endif


