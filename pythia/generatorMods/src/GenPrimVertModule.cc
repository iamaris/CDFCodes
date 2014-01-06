//------------------------------------------------------------------------------
// $Id: 
//
// Description:
//	this module does smearing of the primary vertices. Note that
//      Geant3Module does it as well, however sometimes (for quasi 
//      generator-level studies) it is necessary to have the primary vertices 
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

#include "Edm/EventRecord.hh"
#include "Edm/ConstEventIter.hh"
#include "Edm/Handle.hh"
#include "Edm/ConstHandle.hh"

#include "r_n/CdfRn.hh"
#include "CLHEP/Random/RandGaussT.h"
#include "CLHEP/Random/RandFlat.h"
#include "generatorMods/GenPrimVertModule.hh"
#include "SimulationObjects/HEPG_StorableBank.hh"
#include "SimulationObjects/MVTX_StorableBank.hh"

 
//Cot and Svx beam positions
#include "Alignment/CotBeam.hh"
#include "Alignment/SvxBeam.hh"
#include "VertexObjects/Beamline.hh"

#include <iostream>
#include <iomanip>
#include <sstream>
using std::dec;
using std::setw;
using std::hex;
using std::cout;
using std::endl;
using std::ostringstream ;

const long GenPrimVertModule::_defaultRandomSeed1 = 922883591;
const long GenPrimVertModule::_defaultRandomSeed2 = 109735476;
// Accelerator design parameters:
//     Emittance = 1.40E-7 cm
//     BetaStar  = 35 cm
//     z0      =  0 cm
// Use numbers from Stan Lai blessed at Joint Physics meeting 05/02/03
const float GenPrimVertModule::_defaultEmittanceX = 1.26E-7;
const float GenPrimVertModule::_defaultEmittanceY = 1.24E-7;
const float GenPrimVertModule::_defaultBetaStarX = 38.6;
const float GenPrimVertModule::_defaultBetaStarY = 38.0;
const float GenPrimVertModule::_defaultZ0X = 14.2;
const float GenPrimVertModule::_defaultZ0Y = -9.2;

//_____________________________________________________________________________
GenPrimVertModule::GenPrimVertModule( const char* const theName,
                                      const char* const theDescription )
  : AppModule( theName, theDescription ),
    _sigmaX        ("sigma_x"       ,this, 25.7E-4),
    _sigmaY        ("sigma_y"       ,this, 25.8E-4),
    _sigmaZ        ("sigma_z"       ,this,    30.),
    _sigmaT        ("sigma_t"       ,this,     1.3),
    _nBunches      ("n_bunches"     ,this,      0),
    _bunchSpacing  ("bunch_spacing" ,this,   400.),
    _randomSeed1("RandomSeed1",this,GenPrimVertModule::_defaultRandomSeed1),
    _randomSeed2("RandomSeed2",this,GenPrimVertModule::_defaultRandomSeed2),
    _pvCentralX( "pv_central_x", this, 0., -10000., 10000.),
    _pvCentralY( "pv_central_y", this, 0., -10000., 10000.),
    _pvCentralZ( "pv_central_z", this, 0., -10000., 10000.),
    _pvSlope_dXdZ("pv_slope_dxdz",this,0.0,-10000., 10000.),
    _pvSlope_dYdZ("pv_slope_dydz",this,0.0,-10000., 10000.),
    _emittanceX("beamEmittanceX", this, GenPrimVertModule::_defaultEmittanceX, 0., 1.),
    _emittanceY("beamEmittanceY", this, GenPrimVertModule::_defaultEmittanceY, 0., 1.),
    _betaStarX("beamBetaStarX", this, GenPrimVertModule::_defaultBetaStarX, 0., 10000.),
    _betaStarY("beamBetaStarY", this, GenPrimVertModule::_defaultBetaStarY, 0., 10000.),
    _z0X("beamZ0X", this, GenPrimVertModule::_defaultZ0X, -10000., 10000),
    _z0Y("beamZ0Y", this, GenPrimVertModule::_defaultZ0Y, -10000., 10000),
    _useDB("BeamlineFromDB",this,false),
    _useBetaStarWidth("UseBetaStarBeamWidth", this, false),
    _useBetaStardLdz("UseBetaStarZVertex", this, false)

{

					// Define parameters for talk-to

  _sigmaX.addDescription(" \t beam sigma(x)[cm]");
  commands()->append(&_sigmaX);
  _sigmaY.addDescription(" \t beam sigma(Y)[cm]");
  commands()->append(&_sigmaY);
  _sigmaZ.addDescription(" \t sigma(Z) of the interaction region[cm]");
  commands()->append(&_sigmaZ);
  _sigmaT.addDescription(" \t sigma(T) of the primary interaction[ns]");
  commands()->append(&_sigmaT);
  _nBunches.addDescription(" \t number of bunches to simulate");
  commands()->append(&_nBunches);
  _bunchSpacing.addDescription(" \t spacing between the bunches[ns]");
  commands()->append(&_bunchSpacing);
  _pvCentralX.addDescription(" \t beam position(x) [cm]");
  commands()->append(&_pvCentralX);
  _pvCentralY.addDescription(" \t beam position(y) [cm]");
  commands()->append(&_pvCentralY);
  _pvCentralZ.addDescription(" \t beam position(z) [cm]");
  commands()->append(&_pvCentralZ);


  _pvSlope_dXdZ.addDescription(" \t beam slope (dx/dz)");
  commands()->append(&_pvSlope_dXdZ);
  _pvSlope_dYdZ.addDescription(" \t beam slope (dy/dz)");
  commands()->append(&_pvSlope_dYdZ);
  _useDB.addDescription("\t use Beamline DB for beam position and slope");
  commands()->append(&_useDB);
  _useBetaStarWidth.addDescription("\t use z-dependent beam widths");
  commands()->append(&_useBetaStarWidth);

  ostringstream tmpSstream0;
  tmpSstream0 << "\t use betaStar z-vertex distribution;"
	      << "\n\t\t NOTE: BetaStarBeamWidth parameters do not affect this function";
  _useBetaStardLdz.addDescription(tmpSstream0.str());
  commands()->append(&_useBetaStardLdz);

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

  // Initialize z-dependent transverse beam width  parameterization submenu
  _beamWidthMenu.initialize("BeamBetaStarMenu",this);
  _beamWidthMenu.initTitle("BetaStar beam width menu");
  commands()->append(&_beamWidthMenu);

  // Add commands to the SigmaXY(z) submenu
  _beamWidthMenu.commands()->append(&_emittanceX);
  _beamWidthMenu.commands()->append(&_emittanceY);
  _beamWidthMenu.commands()->append(&_betaStarX);
  _beamWidthMenu.commands()->append(&_betaStarY);
  _beamWidthMenu.commands()->append(&_z0X);
  _beamWidthMenu.commands()->append(&_z0Y);

  ostringstream tmpSstream3;
  ostringstream tmpSstream4;
  ostringstream tmpSstream5;
  ostringstream tmpSstream6;
  ostringstream tmpSstream7;
  ostringstream tmpSstream8;

  // Describe the parameters for the transverse beam widths
  tmpSstream3 << "      \t\t\tUnnormalized Transverse Emittance for x [cm]"
              << "\n\t\t\t(default " << _emittanceX.value() << ").";
  _emittanceX.addDescription(tmpSstream3.str());
  tmpSstream4 << "      \t\t\tUnnormalized Transverse Emittance for y [cm]"
              << "\n\t\t\t(default " << _emittanceY.value() << ").";
  _emittanceY.addDescription(tmpSstream4.str());
  tmpSstream5 << "      \t\t\tAmplitude function for x at the interaction point [cm]"
              << "\n\t\t\t(default " << _betaStarX.value() << " cm).";
  _betaStarX.addDescription(tmpSstream5.str());
  tmpSstream6 << "      \t\t\tAmplitude function for y at the interaction point [cm]"
              << "\n\t\t\t(default " << _betaStarY.value() << " cm).";
  _betaStarY.addDescription(tmpSstream6.str());
  tmpSstream7 << "      \t\t\tz position of the min of the amplitude function for x [cm]"
              << "\n\t\t\t(default " << _z0X.value() << " cm).";
  _z0X.addDescription(tmpSstream7.str());
  tmpSstream8 << "      \t\t\tz position of the min of the amplitude function for y [cm]"
              << "\n\t\t\t(default " << _z0Y.value() << " cm).";
  _z0Y.addDescription(tmpSstream8.str());

  _svxBeam = 0;
  _cotBeam = 0;
  _beamline = 0;
  _zVtxGenerator = 0;
}

//_____________________________________________________________________________
GenPrimVertModule::~GenPrimVertModule() {
}

//_____________________________________________________________________________
AppResult GenPrimVertModule::beginJob(AbsEvent* aJob) {
  CdfRn* rn = CdfRn::Instance();
  if ( !rn->isReadingFromFile() ) {
    rn->SetEngineSeeds(_randomSeed1.value(), _randomSeed2.value(),"GEN_PRIM_VERT");
  }
  //Alignment and beam position
  if(_useDB.value()) {
     _svxBeam = new SvxBeam();
     _cotBeam = new CotBeam();
  } 

  if (_useBetaStardLdz.value()) {
    _zVtxGenerator = new GendLdz(this->verbose());
    if ( _zVtxGenerator->init() ) {
      // something wrong with betaStar dL/dz class; fall back to Gaussian mode
      errlog(ELwarning,"GenPrimVertModule") 
	<< " Encountered problems initializing the betaStar z vertex class. Falling back to Gaussian z vertex mode." << endmsg;
      _useBetaStardLdz.set(false);
      if (_zVtxGenerator != 0) delete _zVtxGenerator;
    }
  }

  return AppResult::OK;
}

//_____________________________________________________________________________
AppResult GenPrimVertModule::beginRun( AbsEvent* aRun ) {

  if(_useDB.value()) {
    _svxBeam->loadRun();
    _cotBeam->loadRun();
    //change svt beamline accessor logic from isValid() to isFitted()
    if(_svxBeam->isFitted()) { 
       _beamline = new Beamline(_svxBeam->getBeamline());  
    }
    else {
      if(_cotBeam->isValid()) {
        _beamline = new Beamline(_cotBeam->getBeamline());
	errlog(ELwarning,"GenPrimVertModule") << 
	"No Fitted SVX beamline ... using COT beamline for current run " << endmsg;
      }
      else {
	errlog(ELerror,"GenPrimVertModule") << 
	  " No beamline found for current run " << endmsg;
      }
    }
    if(_useDB.value() && _beamline != 0) {
      _pvCentralX.set(_beamline->BeamX());
      _pvCentralY.set(_beamline->BeamY());
      _pvCentralZ.set(_beamline->BeamZ());
      _pvSlope_dXdZ.set(_beamline->SlopeX());
      _pvSlope_dYdZ.set(_beamline->SlopeY());
      // Don't set x and y from database since they are zero there...
      _sigmaZ.set(_beamline->WidthZ());
    } 
  }

  if(_useBetaStardLdz.value()) {
    int runNo = AbsEnv::instance()->runNumber();
//     if ( _zVtxGenerator->getParms(runNo) ) {
    if ( _zVtxGenerator->getParms(runNo, _cotBeam) ) {
      errlog(ELwarning,"GenPrimVertModule")
	<< " Encountered problems initializing the betaStar z vertex class for run "
	<< runNo << ".  Falling back to Gaussian z vertex mode." << endmsg;
      _useBetaStardLdz.set(false);
    }
  }

  return AppResult::OK;

}

//_____________________________________________________________________________
AppResult GenPrimVertModule::event( AbsEvent* event ) {
//-----------------------------------------------------------------------------
// count the primary interactions assuming there are no "holes" in HEPG numbers
// handle case when an input event already has 1 HEPG and 1 MVTX banks
//-----------------------------------------------------------------------------
  int n_hepg = 0;
  int n_mvtx = 0;

  for (AbsEvent::ConstIterator it(event,"HEPG_StorableBank"); it.is_valid(); it++) {
    n_hepg++;
  }

  for (AbsEvent::ConstIterator it(event,"MVTX_StorableBank"); it.is_valid(); it++) {
    n_mvtx++;
  }
					// create MVTX bank of the right length
					// and fill it
  int nv = n_hepg-n_mvtx;

  if ( nv > 0 ) {

    MVTX_StorableBank* mvtx = new MVTX_StorableBank(event,nv);
    
    double*  grn = new double[4*nv];
    double*  urn = new double[nv];
    HepRandomEngine* engine = CdfRn::Instance()->GetEngine("GEN_PRIM_VERT");
    // we don't need to simulate different
    // bunch-crossings if there is only one 
    // primary interaction in the event
    if (nv > 1) {
      RandFlat::shootArray(engine, nv-1, urn);
    }
    // `grn' numbers are used for P.V. 
    // smearing in Z, phi and time
    RandGaussT::shootArray( engine, 4*nv, grn, 0.0, 1.0 );
    
    int    bunch;
    for (int i=0; i<nv; i++) {
      if( _useBetaStardLdz.value() ){
	// Use the betaStar z vertex model
	mvtx->rw(4*i+3) = _zVtxGenerator->throwZ(engine);
      }
      else {
	// Approximate z-vertex dist. as Gaussian
	mvtx->rw(4*i+3) = _sigmaZ.value()*grn[4*i+2]+_pvCentralZ.value();
      }
      
      // Test whether we want to use SigmaX(z), SigmaY(z)
      if (_useBetaStarWidth.value()) {
	// Calculate the transverse widths
	_sigmaX.set(sqrt(_emittanceX.value()*(_betaStarX.value()+
					      pow((mvtx->rw(4*i+3))-_z0X.value(),2)/_betaStarX.value())));
	_sigmaY.set(sqrt(_emittanceY.value()*(_betaStarY.value()+
					      pow((mvtx->rw(4*i+3))-_z0Y.value(),2)/_betaStarY.value())));
      }
      
      mvtx->rw(4*i+1) = _sigmaX.value()*grn[4*i  ]+_pvCentralX.value()+
	_pvSlope_dXdZ.value()*(mvtx->rw(4*i+3));
      mvtx->rw(4*i+2) = _sigmaY.value()*grn[4*i+1]+_pvCentralY.value()+
	_pvSlope_dYdZ.value()*(mvtx->rw(4*i+3));
      
      // smear vertex production time within the 
      // bunch-crossing
      if (i==0) {
	// by definition primary interaction#1 -
	// physics event belongs to bunch#0
	bunch = 0;
      }
      else {
	bunch = int ((2*_nBunches.value()+1)*urn[i-1])-_nBunches.value();
      }
      // convert vertex production time into 
      // seconds
      
      mvtx->rw(4*i+4) = (_bunchSpacing.value()*bunch +
			 _sigmaT.value()*grn[4*i+3]    )*1.e-9;
    }
    
    delete [] grn;
    delete [] urn;
  }

  return AppResult::OK;
}

//_____________________________________________________________________________
AppResult GenPrimVertModule::endRun( AbsEvent* aRun ) {
  if(_beamline != 0) delete _beamline;
  return AppResult::OK;
}

//_____________________________________________________________________________
AppResult GenPrimVertModule::endJob( AbsEvent* aJob ) {
  if(_cotBeam !=0) delete _cotBeam;
  if(_svxBeam !=0) delete _svxBeam;
  if (_zVtxGenerator != 0) delete _zVtxGenerator;
  return AppResult::OK;
}


//_____________________________________________________________________________
AppResult GenPrimVertModule::abortJob( AbsEvent* aJob ){
  return AppResult::OK;
}







