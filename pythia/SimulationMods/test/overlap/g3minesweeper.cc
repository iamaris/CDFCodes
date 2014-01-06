//----- f r a m e w o r k  &  f r a m e  m o d s --
#include "Framework/APPUserBuild.hh"
#include "FrameMods/addCDFrequiredModules.hh"
#include "FrameMods/root/HepRootManager.hh"

//----- s i m u l a t i o n -----------------------
#include "SimulationMods/SimInitManager.hh"
#include "SimulationMods/SimulationControl.hh"
#include "SimulationBase/FactoryMacros.hh"

//----- g e n e r a t o r s -----------------------
#include "generatorMods/GenInputManager.hh"

//----- overlap detection -------------------------
#include "SimulationMods/G3MineSweeper.hh"

//----------------
// Constructors --
//----------------

AppUserBuild::AppUserBuild( AppFramework* theFramework )
    : AppBuild( theFramework )
{
  AppModule* aMod;

  //----- f r a m e m o d s ----------------
  addCDFrequiredModules( this );
  
  //-----  f r a m e m o d s - HepRootManager ---
  aMod = new HepRootManager(); add(aMod); aMod->setEnabled(false);

  //----- g e n e r a t o r s --------------
  //gen input manager - input module
  add( new GenInputManager() );

  //----- s i m u l a t i o n ---------------
  add(new SimInitManager());  
  add(new sim::SimulationControl());

  //----- overlap detection -----------------
  add(new G3MineSweeper());

}

//--------------
// Destructor --
//--------------

AppUserBuild::~AppUserBuild( )
{
}
