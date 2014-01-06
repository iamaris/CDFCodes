//--------------------------------------------------------------------------
// File and Version Information:
//
// Description:
//	Class myPrintModule.hh
//
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
//------------------------------------------------------------------------

#ifndef MYPRINTMODULE_HH
#define MYPRINTMODULE_HH

// Declarations for Fortran routines usesed by this class
extern "C" {
              void my_i_print_();
           }
//----------------------
// Base Class Headers --
//----------------------
#include "Framework/APPModule.hh"
#include "Framework/AbsParmGeneral.hh"
#ifdef CDF
#include "BaBar/Cdf.hh"
#endif
//
//              ---------------------
// 		-- Class Interface --
//		---------------------

class myPrintModule : public AppModule {

//--------------------
// Instance Members --
//--------------------

public:

    // Constructors
    myPrintModule( const char* const theName, const char* const theDescription );

    // Destructor
    virtual ~myPrintModule( );

    // Operations
    virtual void           begin( AppJob* aJob );
    virtual AppResult      event( AbsEvent* anEvent );

protected:
  // local parameter variables: none for now
};

#endif
