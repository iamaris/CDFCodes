#ifndef SIM_INTEGRATIONLEG_INCLUDED
#define SIM_INTEGRATIONLEG_INCLUDED
////////////////////////////////////////////////////////////////////////
//
// File: IntegrationLeg.hh
// $Id: IntegrationLeg.hh,v 1.4 2001/04/23 16:03:53 paterno Exp $

// Purpose: Declaration of the struct IntegrationLeg. Record the
// information associated with a all the steps in the trajectory that
// are in one "leg"; this means all the consecutive steps in the same
// "region". The typedef location_t is used to define the concept of
// "region"; two steps that have equal locations (as determined by
//         operator== (const location_t& a, const location_t& b)
// are by definition in the same "region".
//
// Author:   Marc Paterno
// Created:  26 Feb 2001
// Modified: 12 Apr 2001: Added _start and _end; removed _pointCahce
//           20 Apr 2001: Added _materialName
//
////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <ostream>
#include <string>

#include "SimulationMods/IntegrationData.hh"
#include "geant_i/G3StepdataSupport.hh"
#include "CLHEP/Geometry/Point3D.h"

class CdfTangible;

namespace sim
{

	class IntegrationLeg
	{
	public:
		typedef G3::PhysicalVolumeID     location_t;
		typedef HepPoint3D               point_t;

		// Default IntegrationLeg has no CdfTangible (because we can't
		// create a default CdfTangible). Start and end points are the
		// origin. Material name is the empty string.
		IntegrationLeg();

		// Create an IntegrationLeg for the given location, optionally
		// associated with the given CdfTangible. Start and end points are
		// xyz. If a tangible is supplied, it will be queried for the
		// material name.
		IntegrationLeg(const location_t& where, 
									 const CdfTangible* piece,
									 const point_t& xyz);

		// Create an IntegrationLeg for the given location, containing the
		// given IntegrationData as information for this step, optionally
		// associated with the given CdfTangible. The start end end points
		// are xyz. If a tangible is supplied, it will be queried for the
		// material name.
		IntegrationLeg(const location_t& where,
									 const CdfTangible* piece,
									 const point_t& xyz,
									 const IntegrationData& data);

		// Create an IntegrationLeg for the given location, copying
		// accumulated information from the given IntegrationLeg, and
		// accumulating the given IntegrationData, and optionally
		// associated with the given CdfTangible. The start of the new
		// IntegrationLeg is the end of the previous leg; the end of the
		// new leg is at newEnd.  If a tangible is supplied, it will be
		// queried for the material name.

		IntegrationLeg(const location_t & where,
									 const CdfTangible* piece,
									 const point_t& newEnd,
									 const IntegrationLeg& previous,
									 const IntegrationData& newdata);
		
		IntegrationLeg(const IntegrationLeg& rhs);
		~IntegrationLeg();
		void swap(IntegrationLeg& other);
		IntegrationLeg& operator= (const IntegrationLeg& rhs);

		// Add the given IntegrationData to *this, updating the end point
		// to newEnd.
		void accumulate(const point_t& newEnd, 
										const IntegrationData& data);

		const IntegrationData& current() const;
		const IntegrationData& cumulative() const;
		location_t             where() const;

		// Coordinates of where this leg begins.
		point_t                startPoint() const;

		// Coordinates of where this leg ends.
		point_t                endPoint() const;

		// Coordinates of the center of this leg. Note this works
		// correctly only for string paths; if curved tracks become
		// important, this will have to be fixed (and we'll need to
		// accumluate more data, to do it correctly).
		point_t                midPoint() const;

		void print(std::ostream& os) const;
		void concisePrint(std::ostream& os) const;

		bool operator== (const IntegrationLeg& rhs) const;

	private:
		IntegrationData    _current;    // data in just this step
		IntegrationData    _cumulative; // data up to and including this step
		location_t         _location;   // name of where this step was taken
		point_t            _start;      // position of starting point
		point_t            _end;        // posisiton of ending point
		std::string        _materialName; // name of the material we're in

		// We do not own the pointed-to CdfTangible. It may be null, if
		// we're in a part of the detector that does not have a
		// corresponding CdfTangible.
		mutable const CdfTangible *      _pieceCache;

	}; // class IntegrationLeg

	//
	// Associated functions
	//

	inline
	std::ostream&
	operator<< (std::ostream& os, 
							const IntegrationLeg& s)
	{
		s.concisePrint(os);
		return os;
	}

	inline
	IntegrationLeg::point_t
	midpoint(const IntegrationLeg::point_t& a, 
					 const IntegrationLeg::point_t& b)
{ return 
		IntegrationLeg::point_t ( (a.x() + b.x())/2,
															(a.y() + b.y())/2,
															(a.z() + b.z())/2);
}

	// Inline member functions
	inline const IntegrationData& IntegrationLeg::current() const
{	return _current; }
	
	inline const IntegrationData& IntegrationLeg::cumulative() const
{ return _cumulative; }
	
	inline IntegrationLeg::point_t IntegrationLeg::startPoint() const
{ return _start; }

	inline IntegrationLeg::point_t IntegrationLeg::endPoint() const
{ return _end; }


	inline IntegrationLeg::point_t IntegrationLeg::midPoint() const
{ return midpoint(_start, _end); }


} // namespace sim

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

#endif // #ifndef SIM_INTEGRATIONLEG_INCLUDED
