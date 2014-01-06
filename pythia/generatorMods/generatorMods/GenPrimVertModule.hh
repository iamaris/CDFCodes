//------------------------------------------------------------------------------
// $Id: 
//
// Description:
//	this module does smearing of the primary vertices. Note that
//      Geant3Module does it as well, however sometimes (for quasi 
//      generator-level studies it is necessary to have the primary vertices 
//      smeared w/o running the detector simulation code.
//
// Input: 
//      list of HEPG banks, corresponding to the primary interactions
// Output: 
//      MVTX bank, containing the list of primary vertices smeared with the
//      given resolutions
//
// Author List:
//	Pasha Murat, Jason Galyardt
//----------------------------------------------------------------------------
#ifndef GenPrimVertModule_hh
#define GenPrimVertModule_hh

# include "BaBar/Cdf.hh"
# include "AbsEnv/AbsEnv.hh"
# include "Framework/APPModule.hh"
# include "Framework/APPMenu.hh"
# include "Framework/AbsParmBool.hh"
# include "Framework/AbsParmEnum.hh"
# include "Framework/AbsParmGeneral.hh"
# include "generatorMods/GendLdz.hh"
typedef AbsParmGeneral<long> AbsParmGeneral_long;

class Beamline;
class SvxBeam;
class CotBeam;

class GenPrimVertModule : public AppModule {
//------------------------------------------------------------------------------
//  data members
//------------------------------------------------------------------------------
protected:
					// handle primary vertex smearing
  AbsParmGeneral<int>   _nBunches;
  AbsParmGeneral<float> _bunchSpacing;
  AbsParmGeneral<float> _sigmaX;
  AbsParmGeneral<float> _sigmaY;
  AbsParmGeneral<float> _sigmaZ;
  AbsParmGeneral<float> _sigmaT;
  AbsParmGeneral<float> _pvCentralX;
  AbsParmGeneral<float> _pvCentralY;
  AbsParmGeneral<float> _pvCentralZ;
  AbsParmGeneral<float> _pvSlope_dXdZ;
  AbsParmGeneral<float> _pvSlope_dYdZ;
  AbsParmGeneral<float> _emittanceX;
  static const float _defaultEmittanceX;
  AbsParmGeneral<float> _emittanceY;
  static const float _defaultEmittanceY;
  AbsParmGeneral<float> _betaStarX;
  static const float _defaultBetaStarX;
  AbsParmGeneral<float> _betaStarY;
  static const float _defaultBetaStarY;
  AbsParmGeneral<float> _z0X;
  static const float _defaultZ0X;
  AbsParmGeneral<float> _z0Y;
  static const float _defaultZ0Y;
  AbsParmGeneral<bool> _useDB;
  AbsParmGeneral<bool> _useBetaStarWidth;
  AbsParmGeneral<bool> _useBetaStardLdz;

  // Random number menu
  APPMenu _randomNumberMenu;
  
  // Initial random number seeds (2) (ignored if restoreState is requested)
  AbsParmGeneral_long _randomSeed1;
  static const   long _defaultRandomSeed1;
  AbsParmGeneral_long _randomSeed2;
  static const   long _defaultRandomSeed2;

  // SigmaX(z), SigmaY(z) parameterization menu
  APPMenu _beamWidthMenu;

  //Parameters for beam-spot position
  SvxBeam* _svxBeam;
  CotBeam* _cotBeam;
  Beamline* _beamline;

  // betaStar z vertex simulation
  GendLdz* _zVtxGenerator;

public:
					// ****** constructors and destructor

  GenPrimVertModule(const char* name  = "GenPrimVert", 
		    const char* title = "GenPrimVert");

  GenPrimVertModule(const GenPrimVertModule& m);

  virtual ~GenPrimVertModule();
					// ****** operations
    
  AppResult          beginJob      ( AbsEvent* run);
  AppResult          beginRun      ( AbsEvent* run);
  AppResult          endRun        ( AbsEvent* run);
  AppResult          endJob        ( AbsEvent* job);
  AppResult          event         ( AbsEvent* event );
  AppResult          abortJob      ( AbsEvent* event );

};

#endif
