//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: cdfSim.cc,v 1.40 2004/11/23 16:01:14 syjun Exp $
//
// Description:
//	Class AppUserBuild. This class must be provided by the user of
//	the framework in order to build an application. It must define
//	the modules that are to form the basis of the application.
//
// Environment:
//	Software developed for the CDF Detector
//
// Revision history:
//      03/27/2001 Include Bgenerator and GenTrig tools (Ch.Paus)
//
//------------------------------------------------------------------------

//----- f r a m e w o r k  &  f r a m e  m o d s --
#include "Framework/APPUserBuild.hh"
#include "FrameMods/addCDFrequiredModules.hh"
#include "FrameMods/root/HepRootManager.hh"

#include "TofAlgs/Modules/TofManager.hh"
//------r a n d o m  n u m b e r  g e n e r a t o r s 
#include "r_n/RandomModule.hh"

//----- s i m u l a t i o n -----------------------
#include "SimulationMods/SimInitManager.hh"
#include "SimulationMods/SimulationControl.hh"
#include "SimulationBase/FactoryMacros.hh"
#include "SimulationMods/SimValModule.hh"

//----- g e n e r a t o r s -----------------------
#include "generatorMods/HadScatGenSequence.hh"
#include "generatorMods/DecayPackageSequence.hh"
#include "GenTrig/GenTrigSequence.hh"
#include "generatorMods/MinBiasSequence.hh"
#include "generatorMods/GenOutputManager.hh"
#include "generatorMods/GenInputManager.hh"
#include "generatorMods/GenPrimVertModule.hh"

//----- t r a c k i n g ---------------------------
#include "TrackingMods/SiClusteringModule.hh"

//------ q u i c k s i m --------------------------
#include "QuickSim/Objects/addSimpleCotObjects.hh"
#include "QuickSim/Simulation/SimpleCotDigitizer.hh"
#include "QuickSim/Reconstruction/SimpleCotTrkReco.hh"
#include "QuickSim/Objects/addSimpleSiObjects.hh"
#include "QuickSim/Simulation/SimpleSiDigitizer.hh"
#include "QuickSim/Reconstruction/SimpleSiTrkReco.hh"

//------ c o m p r e s s i o n------------
#include "PADSMods/PadRawModule.hh"
//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////
// We need to make a class to hold the registry declarations.  As a
// kludge for now, we will just add the SIM_REGISTRY_DECL for each 
// digitizer class here.
//
////////////////////////////////////////////////////////////////////////
namespace cot
{
   SIM_REGISTRY_DECL(CotDigitizer);
}
namespace svx
{
   SIM_REGISTRY_DECL(SvxDigitizer);
}
namespace muon
{  // as defined in MuonDigi.cc
        SIM_REGISTRY_DECL(MuonDigiCMU);
        SIM_REGISTRY_DECL(MuonDigiCMP);
        SIM_REGISTRY_DECL(MuonDigiCMX);
        SIM_REGISTRY_DECL(MuonDigiCSX);
        SIM_REGISTRY_DECL(ImuDigiBMUGas);
        SIM_REGISTRY_DECL(ImuDigiBSUPaddle);
        SIM_REGISTRY_DECL(ImuDigiTSUPaddle);
}

namespace tof
{
        SIM_REGISTRY_DECL(TofDigi3Pack);
        SIM_REGISTRY_DECL(TofDigiBar);
}
namespace calor
{
        SIM_REGISTRY_DECL(CalorDigiCcal);
        SIM_REGISTRY_DECL(CalorDigiWcal);
        SIM_REGISTRY_DECL(CalorDigiPcal);
        SIM_REGISTRY_DECL(CalorDigiGeneric);
        SIM_REGISTRY_DECL(CalorDigiBFCoil);
        SIM_REGISTRY_DECL(CalorDigiNoBFCoil);   
}
namespace clc
{
  SIM_REGISTRY_DECL(ClcDigitizer);
}      

namespace simplesi
{
  SIM_REGISTRY_DECL(SimpleSiDigitizer);
}
namespace simplecot
{
  SIM_REGISTRY_DECL(SimpleCotDigitizer);
}
 
static const char rcsid[] = "$Id: cdfSim.cc,v 1.40 2004/11/23 16:01:14 syjun Exp $";

//----------------
// Constructors --
//----------------

AppUserBuild::AppUserBuild( AppFramework* theFramework )
    : AppBuild( theFramework )
{
  AppModule* aMod;
  APPSequence* aSeq;

  //----- f r a m e m o d s ----------------
  addCDFrequiredModules( this );
  aMod = TofManager::getInstance(); add(aMod); aMod->setEnabled(false);

  //-----  f r a m e m o d s - HepRootManager ---
  // HepRootManager should be before filter module
  aMod = new HepRootManager(); add(aMod); aMod->setEnabled(false);

  //------r a n d o m  n u m b e r  g e n e r a t o r s 
  aMod = new RandomModule("RandomGenManager", "Random number generators manager");
  add(aMod); aMod->setEnabled(true);  
  
  //----- g e n e r a t o r s --------------
  //gen input manager - input module
  add( new GenInputManager() );

  // Had. Scat. generators
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

  aMod = new GenPrimVertModule(); add(aMod); aMod->setEnabled(true);

  // Generator filters
  aSeq = new GenTrigSequence();
  add( aSeq );

  //----- s i m u l a t i o n ---------------
  add(new SimInitManager());  
  add(new sim::SimulationControl());

  //------ v a l i d a t i o n ______________
  aMod = new SimValModule(); add(aMod); aMod->setEnabled(false);

  //------ q u i c k s i m ------------------
  // Reconstruction
  aMod = new SimpleCotTrkReco();  add(aMod); aMod->setEnabled(false);
  aMod = new SimpleSiTrkReco();   add(aMod); aMod->setEnabled(false);

  //----- t r a c k i n g -------------------  
  // Add some modules so that sim can be reconstructed
  aMod = new SiClusteringModule(); add(aMod); aMod->setEnabled(false);

  //----- c o m p r e s s i o n -------------
  aMod = new CotqModule(); add(aMod); aMod->setEnabled(true);
  aMod = new SvxqModule(); add(aMod); aMod->setEnabled(true);
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
