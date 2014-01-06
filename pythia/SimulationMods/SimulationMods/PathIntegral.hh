////////////////////////////////////////////////////////////////////////
//
// File: PathIntegral.hh
// $Id: PathIntegral.hh,v 1.3 2001/04/13 13:22:27 paterno Exp $
// Purpose: Declaration of the class PathIntegral. This class
// represents the integral, along a given path, of a set of
// quantities. Each quantity is accumulated in "legs"; each "leg"
// consists of those steps taken consecutively in the same "location".
// The PathIntegral allows the user to obtain the values of quanitites
// at the individual legss, and also to obtain the values integrated
// over a range of legs.
//
// In this prototype version, we keep *both* the individual legs and
// the cumulative results (the integral). When the usage patterns
// become clear, we may learn it is better to keep only one, and
// calculate the other as needed.
//
// Since this is a prototype, we've not yet optimized return values;
// we return everything by value (not reference). The benefit is that
// we haven't revealed what our internal storage format is, so we can
// change it while optimizing without changing any user code. We'll
// probably have to change either the leg-access or the
// integral-access to return references, when we know which is more
// important.
//
// Author:  Marc Paterno
// Created: 9 March 2001
// Modified:
//
////////////////////////////////////////////////////////////////////////

#include <vector>
#include "CLHEP/Geometry/Point3D.h"
#include "SimulationMods/IntegrationLeg.hh"

class CdfTangible;

namespace sim
{

	class PathIntegral
	{
	public:
		typedef std::vector<IntegrationLeg>  leg_vector_t;
		typedef leg_vector_t::const_iterator const_iter;
		typedef leg_vector_t::size_type      index_t;
		typedef IntegrationLeg::location_t   location_t;
		typedef IntegrationLeg::point_t      point_t;

		// Compiler-generated default c'tor does the right thing.
		// Compiler-generated copy c'tor does the right thing.
		// Compiler-generated d'tor does the right thing.

		// Return the data for the i'th leg.
		IntegrationData leg(index_t i) const;

		// Return the location in which the i'th leg was made.
		location_t    location(index_t i) const;

		// Return the location of the end of the path.
		location_t    location() const;

		// Return the coordinates of the starting point for the i'th leg.
		point_t      startPoint(index_t i) const;

		// Return the coordinates of the ending point for the i'th leg.
		point_t      endPoint(index_t i) const;

		// Return the coordinates of the starting point for the path. This
		// is the same as the starting point of the 0th leg.
		point_t       startPoint() const;
		
		// Return the coordinates of the ending point for the path. This
		// is the same ast the ending point of the last leg.
		point_t       endPoint() const;

		// Return the integral over the range [begin, end]. This returns
		// the sum over the range [begin, end] of the quantities in
		// IntegrationData. Note that the range includes both begin and
		// end; it is *not* a half-open interval. It is an error to call
		// this with end < begin.
		IntegrationData integral(index_t begin, index_t end) const;

		// Return the cumulative data for the entire path.
		IntegrationData integral() const;

		// Return the number of integration legs.
		index_t numLegs() const;

		// Add a new integration step, in the given CdfTangible, at the
		// end of the path. If 'where' is the same location as the head of
		// the path, this will update the most recent leg; if it a new
		// location, it will begin a new leg. xyz is the absolute
		// coordinates at the end of this integration step.
		void append(location_t where,
								const CdfTangible* piece,
								const point_t& xyz,
								const IntegrationData& step);

		// Print *this to the given stream.
		void print(std::ostream& os) const;
		void concisePrint(std::ostream& os) const;

	private:
		leg_vector_t     _legs;

		void
		_appendToEmptyPath(location_t where,
											 const CdfTangible* piece,
											 const point_t& xyz,
											 const IntegrationData& data);

		void
		_appendToSameLeg(const point_t& xyz, 
										 const IntegrationData& data);
		
		void
		_appendToNewLeg(location_t where,
										const CdfTangible* piece,
										const point_t& xyz,
										const IntegrationData& data);

	}; // class PathIntegral

	inline
	std::ostream&
	operator<< (std::ostream& os, 
							const PathIntegral& pi)
	{
		pi.concisePrint(os);
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

