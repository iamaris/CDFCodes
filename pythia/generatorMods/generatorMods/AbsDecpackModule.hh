//------------------------------------------------------------------------------
// Description:
//	Base class for decay package modules
//
//   PROVIDES:
//      -manages HEPEVT instances for derived class
//
// created Dec 07 2001, Elena Gerchtein (CMU)
//       
//----------------------------------------------------------------------------
#ifndef ABSDECPACKMODULE_HH__
#define ABSDECPACKMODULE_HH__

#include "BaBar/Cdf.hh"
#include "Framework/APPModule.hh"
#include "Framework/AbsParmGeneral.hh"


class AbsDecpackModule : public AppModule {

public:

  AbsDecpackModule(const char* name  = "AbsDecpackModule", 
	       const char* title = "AbsDecpackModule");

  AbsDecpackModule(const AbsDecpackModule& m);

  virtual ~AbsDecpackModule();
					// ****** operations
    
  AppResult          beginJob      ( AbsEvent* job);
  AppResult          beginRun      ( AbsEvent* run);
  AppResult          endRun        ( AbsEvent* run);
  AppResult          endJob        ( AbsEvent* job);
  AppResult          event         ( AbsEvent* event );

					// virtual functions to be overwritten 
					// in the derived classes
  
  virtual AppResult  genBeginJob() = 0;
  virtual AppResult  genEndJob  () = 0;
  virtual AppResult  genBeginRun(AbsEvent* run) = 0;
  virtual AppResult  genEndRun  (AbsEvent* run) = 0;
  virtual int        callGenerator(AbsEvent* event) = 0;

protected:

  static const char* genId;             // should be defined in derived class

private:
  int _modeval;
  //operating mode: 0 - default, generator fills hepevt;
  //                1 - QQ fills hepevt from hepg
  AbsParmGeneral<int> _mode;  

};

#endif
