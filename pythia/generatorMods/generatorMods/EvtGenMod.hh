//***************************************************************************
//* W. H. Bell ************ Feb 15, 2001 ************************************
//* A module to allow HEPG Bank production from Pythia & EvtGen or EvtGen.  *
//***************************************************************************/
//
// rev: dec 07 2001 lena: inherited AbsDecpackModule; 

#ifndef EVTGENMOD_HH
#define EVTGENMOD_HH

#include "EvtGen/EvtGen.hh"

#include "generatorMods/AbsDecpackModule.hh"
#include "Framework/AbsParmGeneral.hh"
#include "Framework/AbsParmString.hh"
#include "Framework/AbsParmBool.hh"
#include "EvtGen/EvtGenInterface.hh"
#include "EvtGenBase/EvtRandomEngine.hh"

#ifdef CDF
#include "BaBar/Cdf.hh"
#endif

#include <string>
//              ---------------------
//              -- Class Interface --
//              ---------------------
 
class EvtGenMod : public AbsDecpackModule{
  
  //--------------------
  // Instance Members --
  //--------------------
  
public:
  static const char* genId;

  //
  // Constructors
  //
  EvtGenMod();
  
  //
  // Destructor
  //
  virtual ~EvtGenMod();
  
  // Operations
  AppResult genBeginJob();
  AppResult genEndJob();
  AppResult genBeginRun( AbsEvent* anEvent );
  AppResult genEndRun( AbsEvent* anEvent );
  int       callGenerator( AbsEvent* anEvent );


private:
  void _initializeTalkTo();

  AbsParmBool            _useRootParticle;
  AbsParmGeneral<double> _rootParticlePx;
  AbsParmGeneral<double> _rootParticlePy;
  AbsParmGeneral<double> _rootParticlePz;
  AbsParmGeneral<int>    _rootParticleHepID;
  AbsParmBool            _useUserDecayFile;
  AbsParmString          _userDecayFile;
  AbsParmBool            _adjustCPAsymm;
  AbsParmBool            _decayPromptCharm;
  AbsParmBool            _decayBMeson;
  AbsParmBool            _decayBBaryon;
  AbsParmGeneral<long>   _randomSeed1;
  AbsParmGeneral<long>   _randomSeed2;

  EvtGenInterface *_myevtgenInterface;
  //The generator has to be a private data member such that the
  //decay file(s) don't go out of scope after initialization !!!
  EvtGen *_generator;
  //module needs to own pointer that is passed to EvtGen
  EvtRandomEngine *_myRandomEngine;

  int _eventCount;
};

#endif //EVTGENMOD_HH

