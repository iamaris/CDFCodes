#ifndef SIM_INTEGRATIONDATA_INCLUDED
#define SIM_INTEGRATIONDATA_INCLUDED
////////////////////////////////////////////////////////////////////////
//
// File: IntegrationData.hh
// $Id: IntegrationData.hh,v 1.4 2001/04/23 16:03:53 paterno Exp $
// Purpose: Declare and define the struct IntegrationData. This struct
// contains all the information accumulated by the material
// integration program, and the functions necessary to accumulate
// those data.
//
// Author:  Marc Paterno
// Created: 16 Mar 2001
// Modified:18 Apr 2001: Added material name. See ChangeLog
//          20 Apr 2001: Moved material to IntegrationLeg.
//
////////////////////////////////////////////////////////////////////////

#include <ostream>
#include <string>
#include <iomanip>

namespace sim
{
	struct IntegrationData
	{
		//
		// Member functions
		//
		// Compiler-generator copy, assignment and d'tor do the right
		// thing. Default c'tor zero-initializes all members.
		IntegrationData();
		IntegrationData(double radl, double intl, double bdl, 
										double dist, int nsteps);

		IntegrationData&      operator+= (const IntegrationData& rhs);
		IntegrationData&      operator-= (const IntegrationData& rhs);

		// Print *this, using the string sep as separators between items.
		void print(std::ostream& os, const std::string& sep) const;
		void concisePrint(std::ostream& os) const;

		//
		// Data members
		//
		double      radiationLength;   // in GEANT3's units
		double      interactionLength; // in GEANT3's units
		double      BdL;               // in cm* kilogauss 
		double      pathLength;        // in cm.
		int         numSteps;          // number of steps during integration.
	}; // struct IntegrationData

	inline
	IntegrationData::IntegrationData() :
		radiationLength(),
		interactionLength(),
		BdL(),
		pathLength(),
		numSteps()
	{ }

	inline
	IntegrationData::IntegrationData(double radl, double intl, 
																	 double bdl, double dist,
																	 int nsteps) :
		radiationLength(radl),
		interactionLength(intl),
		BdL(bdl),
		pathLength(dist),
		numSteps(nsteps)
	{ }


	inline
	IntegrationData&
	IntegrationData::operator+= (const IntegrationData& rhs)
	{
		radiationLength   += rhs.radiationLength;
		interactionLength += rhs.interactionLength;
		BdL               += rhs.BdL;
		pathLength        += rhs.pathLength;
		numSteps          += rhs.numSteps;
		return *this;
	}

	inline
	IntegrationData&
	IntegrationData::operator-= (const IntegrationData& rhs)
	{
		radiationLength   -= rhs.radiationLength;
		interactionLength -= rhs.interactionLength;
		BdL               -= rhs.BdL;
		pathLength        -= rhs.pathLength;
		numSteps          -= rhs.numSteps;
		return *this;
	}

	inline
	void
	IntegrationData::concisePrint(std::ostream& os) const
	{
		os << radiationLength << '\t'
			 << interactionLength << '\t'
			 << BdL << '\t'
			 << pathLength << '\t'
			 << numSteps;
}
	

	inline
	void
	IntegrationData::print(std::ostream& os, const std::string& sep) const
{
	os << "Rad. length: " << radiationLength << sep
		 << "Int. length: " << interactionLength << sep
		 << "BdL        : " << BdL << sep
		 << "PathLength : " << pathLength << sep
		 << "NumSteps   : "	<< numSteps;
}
	// Sort first on pathLength, then BdL, then interactionLength, then
	// radiationLength, then numSteps
	inline
	bool 
	operator< (const IntegrationData& a, const IntegrationData& b)
{
	if ( a.pathLength == b.pathLength )
		if ( a.BdL == b.BdL )
			if ( a.interactionLength == b.interactionLength )
				if ( a.radiationLength == b.radiationLength )
					return a.numSteps < b.numSteps;
				else
					return a.radiationLength < b.radiationLength;
			else
				return a.interactionLength < b.interactionLength;
		else
			return a.BdL < b.BdL;
	else
		return a.pathLength < b.pathLength;
}


inline 
bool 
operator==(const IntegrationData& a, const IntegrationData& b) 
{
	return (a.radiationLength   == b.radiationLength   &&
					a.interactionLength == b.interactionLength &&
					a.BdL               == b.BdL               &&
					a.pathLength        == b.pathLength        &&
					a.numSteps          == b.numSteps);
}
	
inline
bool
operator!= (const IntegrationData& a, const IntegrationData& b)
{
	return ! (a == b);
}


//
// These operators return const values to prevent misbehavior of code
// like the following:
//
//     IntegrationData a(...), b(...), c,(...);
//     Integration record d = a + b + c;
//
// For details, see Meyers, "More Effective C++", item 6.
//

inline
const IntegrationData
operator+ (const IntegrationData& a, const IntegrationData& b)
{
	IntegrationData result(a);
	result += b;
	return result;
}

inline
const IntegrationData
operator- (const IntegrationData& a, const IntegrationData& b)
{
	IntegrationData result(a);
	result -= b;
	return result;
}

inline
std::ostream&
operator<< (std::ostream& os, 
						const IntegrationData& d)
{
	d.concisePrint(os);
	return os;
}
		
} // namespace sim


	/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
#endif // #ifndef SIM_INTEGRATIONDATA_INCLUDED
