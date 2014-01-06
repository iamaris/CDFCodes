//--------------------------------------------------------------------------
// File and Version Information:
// 	myPrintModule.cc
//
// Description:
//	Class myPrintModule
//
// Environment:
//	Software developed for the CDFII Detector
//
// Author List:
//	Marjorie Shapiro		Original Author
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "myPrintModule.hh"

//-------------
// C Headers --
//-------------
#include <assert.h>

//---------------
// C++ Headers --
//---------------
#include <iostream>
#include <fstream>
//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "BaBar/Cdf.hh"
#include "EdmObjects/ToFromYbos.hh"
#include "Trybos/TRY_Record_Iter_Same.hh"
#include "Trybos/TRY_Generic_Bank.hh"
#include "Banks/HEPG_Bank.hh"
//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

//static const char rcsid[] = "$Id: myPrintModule.cc,v 1.7 2002/01/28 00:13:52 rico Exp $";
ofstream file("hepginfo.dat");

//----------------
// Constructors --
//----------------

myPrintModule::myPrintModule(
    const char* const theName,
    const char* const theDescription )
  : AppModule( theName, theDescription )
{
}

//--------------
// Destructor --
//--------------

myPrintModule::~myPrintModule( )
{
}

//--------------
// Operations --
//--------------

void
myPrintModule::begin( AppJob* )
{
    std::cout << name( ) << " myPrintModule begin Job " << std::endl;
}

AppResult myPrintModule::event( AbsEvent* anEvent)
{

   // Uncomment the following 2 lines of code to test Fortran Interface:
   //   F77_readIntoIW( anEvent, "HEPG" );
   //   my_i_print_();

   // now find out how many HEPG banks exist
   int num_hepg = 0;
   for(TRY_Record_Iter_Same hepg_iter(anEvent,"HEPG") ;
       hepg_iter.is_valid(); ++hepg_iter)
     { num_hepg++; 
     //
     // For each bank, print info on number of particles
     //
       HEPG_Bank  hepg(hepg_iter);
       std::cout << "HEPG Bank for Event "<<
       hepg.event_number() << " has  Nhep="<< 
       hepg.n_particles() << " with " << hepg.n_displaced() << 
       " displaced tracks \n" ;
     //
     // Write full hepg info to a file
     //
       hepg.print(file);
     }
   return AppResult::OK;
}




