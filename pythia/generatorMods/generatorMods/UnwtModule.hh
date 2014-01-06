//--------------------------------------------------------------------------
// Description:
//      C++ wrapper to put VEGAS/PAPAGENO unweighting into AC++
// Environment:
//	Software developed for the CDF Detector.
//
// Author List:
//      G.Velev                
//
// revision history:
// -----------------
// July 9, 2001, G.Velev, Add talk-to
// Aug 30 2001 lena: added ct and genId
//------------------------------------------------------------------------

#ifndef VECUNWMODULE_HH__
#define VECUNWMODULE_HH__

//----------------------
// Base Class Headers --
//----------------------

class AbsEvent;

#include "Edm/StorableObject.hh"
#include "Framework/APPModule.hh"
#ifdef CDF
#include "BaBar/Cdf.hh" 
#endif
#include "Framework/AbsParmBool.hh"
#include "vecbos_i/vecbos_i_common_blocks.hh"
 
class VecunwModule : public  AppModule {
//--------------------
// Instance Members --
//--------------------

public:

  static const char* genId;
					// Constructors
  VecunwModule();

  // Destructor
  virtual ~VecunwModule( );
  // Operations
  virtual AppResult   beginRun( AbsEvent* event);
  virtual AppResult   beginJob( AbsEvent* event);
  virtual AppResult   endRun  ( AbsEvent* event);
  virtual AppResult   endJob  ( AbsEvent* event);
  virtual AppResult   event   ( AbsEvent* event);
  
  bool        passed       () const {return _passed;}
  
	             		// ****** other methods
  void        putParametersToUnwt(VECBOS_I_COMMON_BLOCKS *);
  void        getParametersFromUnwt(VECBOS_I_COMMON_BLOCKS *);
private:
  // private event counter
  int _events;
  
  VECBOS_I_COMMON_BLOCKS * common;   // private pointer to comman blocks
  
  // Parameters that control module operation
  // These can be set via the talk to
  
  AbsParmBool _summary;
  AbsParmGeneral<int> _rndmSeed;
  AbsParmGeneral<double>  _max_weight;

private:
  VecunwModule( const char* const theName, 
	      const char* const theDescription) : 
    AppModule(theName,theDescription),
    _summary("EndOfJobSummary", this, false),
    _max_weight("Weight",this,0.00167778966), 
    _rndmSeed("Seed",this,65307836)  
  {}

  VecunwModule( const VecunwModule& m) : 
    AppModule( "x", "x"),
    _summary("EndOfJobSummary", this, false),
    _max_weight("Weight",this,0.00167778966), 
    _rndmSeed("Seed",this,65307836)  
  {}

};

#endif


















