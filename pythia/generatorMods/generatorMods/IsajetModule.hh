//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: IsajetModule.hh,v 1.1 2001/08/30 20:52:09 lena Exp $
//
// Description:
//      This is the C++ wrapper for putting Isajet into AC++
//
// Environment:
//	Software developed for the CDF Detector.
//
// Author List:
//      Marjorie Shapiro                Author
//
// Copyright Information:
//	Copyright (C) 1997
//
// revision history:
// -----------------
// *0001 Dec 11 1997 M.Shapiro: Initial Creation
// *0002 Feb 07 1999 P.Murat: introduce new base class (GeneratorModule)
//                            to allow generation of piled up events
//       Aug 29 2001 lena: clean up - remove __CINT__
//       Aug 30 2001 lena: add genId and ct, move some  ct to privates.
//------------------------------------------------------------------------

#ifndef __ISAJETMODULE_HH__
#define __ISAJETMODULE_HH__

class AbsEvent;
class AppResult;

#include "generatorMods/AbsGenModule.hh"

class IsajetModule : public AbsGenModule {
public:

  static const char* genId;
					// ****** constructors and destructor
  IsajetModule();
					// by default we are using only one 
				        // event generator in the chain

  virtual ~IsajetModule( );
					// ****** overloaded virtual functions
					// of the GeneratorModule
  int          callGenerator   (AbsEvent* event);
  AppResult    genBeginRun     (AbsEvent* event);
  AppResult    genBeginJob     ();
  AppResult    genEndRun       (AbsEvent* event);
  AppResult    genEndJob       ();
  void         fillHepevt   ();

private:				
  IsajetModule(const char* name,
	       const char* title) : 
    AbsGenModule( name, title ) {}

  IsajetModule(const IsajetModule& m): 
    AbsGenModule( "x", "x" ) {}

};

#endif
