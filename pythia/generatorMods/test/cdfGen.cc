//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: cdfGen.cc,v 1.13 2005/01/06 18:02:43 syjun Exp $
//
// created July 30 2001, Elena Gerchtein (CMU)
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "Framework/APPUserBuild.hh"

#include "FrameMods/root/HepRootManager.hh"
#include "SimulationBase/FactoryMacros.hh"

#include "FrameMods/ManagerSequence.hh"
#include "FrameMods/APPFileInputModule.hh"
#include "FrameMods/DH/DHInputModule_2.hh"
#include "FrameMods/AppFileOutputModule.hh"
#include "SimulationObjects/addSimulationObjects.hh"

#include "generatorMods/HadScatGenSequence.hh"
#include "generatorMods/DecayPackageSequence.hh"
#include "generatorMods/MinBiasSequence.hh"
#include "generatorMods/GenOutputManager.hh"
#include "generatorMods/GenInputManager.hh"
#include "generatorMods/GenPrimVertModule.hh"

#include "GenTrig/GenTrigSequence.hh"

#include "r_n/RandomModule.hh"

static const char rcsid[] = "$Id: cdfGen.cc,v 1.13 2005/01/06 18:02:43 syjun Exp $";

//----------------
// Constructors --
//----------------

AppUserBuild::AppUserBuild( AppFramework* theFramework )
    : AppBuild( theFramework )
{
  AppModule* aMod;
  APPSequence* aSeq;

  add( new ManagerSequence ); 
  add( new APPFileInputModule ); 
  add( new DHInputModule_2( "DHInput" ) );
  add( new AppFileOutputModule( ) );

  addSimulationObjects( );

  // this should be before filters
  aMod = new HepRootManager(); add(aMod); aMod->setEnabled(false);

  //gen input manager - input module
  add( new GenInputManager() );

  //random number generators manager
  aMod = new RandomModule("RandomGenManager", "Random number generators manager");
  add(aMod); aMod->setEnabled(true);

  //Had. Scat. generators
   aSeq = new HardScatGenSequence();
  add( aSeq );

  // Decay Packages
  aSeq = new DecayPackageSequence();
  add( aSeq );

  // generator output manager:
  // writes HEPG bank from HEPEVT commonblock;
  // checks that only one generator (I) is enabled.
  
  aMod = new GenOutputManager();
  add( aMod );
  aMod->setEnabled(true);

  // currently writes HEPG by its own since MI does not work
  // placed AFTER GenOutputManager for the same reason
  aSeq = new MinBiasSequence();
  add( aSeq );

  // Primary vertex generation
  aMod = new GenPrimVertModule(); 
  add( aMod );
  aMod->setEnabled(true);

  aSeq = new GenTrigSequence();
  add( aSeq );  
}

//--------------
// Destructor --
//--------------

AppUserBuild::~AppUserBuild( )
{
}
const char * AppUserBuild::rcsId( ) const
{
   return rcsid;
}
