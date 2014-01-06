////////////////////////////////////////////////////////////////////////
//
// File: PathIntegral.cc
// $Id: PathIntegral.cc,v 1.3 2001/04/13 13:22:29 paterno Exp $
// Purpose: Implementation of the PathIntegral class; see the header
// for details.
//
// Author:  Marc Paterno
// Created: 9 Mar 2001
// Modified:
//
////////////////////////////////////////////////////////////////////////

#include <ostream>
#include <cassert>
#include <algorithm>
#include <iterator>

#include "SimulationMods/PathIntegral.hh"

using namespace std;

//
// Note that we use vector::at rather than vector::operator[], in
// order to let the library automatically handle range checking.
//

namespace sim
{
	IntegrationData
	PathIntegral::leg(index_t i) const
	{
		return _legs.at(i).current();
	}


	PathIntegral::location_t
	PathIntegral::location(index_t i) const
	{
		return _legs.at(i).where();
	}


	
	HepPoint3D
	PathIntegral::startPoint(index_t i) const
	{
		return _legs.at(i).startPoint();
	}

	HepPoint3D
	PathIntegral::endPoint(index_t i) const
	{
		return _legs.at(i).endPoint();
	}


	PathIntegral::location_t
	PathIntegral::location() const
	{
 		return _legs.empty()
 			? location_t()
 			: _legs.back().where();
	}

	HepPoint3D
	PathIntegral::startPoint() const
	{
 		return _legs.empty()
 			? HepPoint3D(0.0, 0.0, 0.0)
 			: _legs.front().startPoint();
	}

	HepPoint3D
	PathIntegral::endPoint() const
	{
 		return _legs.empty()
 			? HepPoint3D(0.0, 0.0, 0.0)
 			: _legs.back().endPoint();
	}

	IntegrationData
	PathIntegral::integral() const
	{
		return _legs.empty()
			? IntegrationData()
			: _legs.back().cumulative();
	}


	PathIntegral::index_t
	PathIntegral::numLegs() const
	{
		return _legs.size();
	}

	IntegrationData
	PathIntegral::integral(PathIntegral::index_t begin,
												 PathIntegral::index_t end) const
	{
		if ( begin == 0 )
			return _legs.at(end).cumulative();

		IntegrationData up_to_end    = _legs.at(end).cumulative();
		IntegrationData before_begin = _legs.at(begin-1).cumulative();
		IntegrationData result = up_to_end - before_begin;
		return result;
	}

	void
	PathIntegral::_appendToEmptyPath(PathIntegral::location_t where,
																	 const CdfTangible* piece,
																	 const point_t& xyz,
																	 const IntegrationData& data)
	{
		_legs.push_back( IntegrationLeg(where, piece, xyz, data) );
	}

	void
	PathIntegral::_appendToSameLeg(const point_t& xyz,
																 const IntegrationData& data)
	{
		_legs.back().accumulate(xyz, data);
	}

	void
	PathIntegral::_appendToNewLeg(PathIntegral::location_t where,
																const CdfTangible* piece,
																const point_t& xyz,
																const IntegrationData& data)
	{
		assert ( ! _legs.empty() );
		
		_legs.push_back ( IntegrationLeg(where,        // the new location
																		 piece,        // the tangible
																		 xyz,
																		 _legs.back(), // the old one to copy
																		 data));       // the new data to add
	}

	void
	PathIntegral::append(PathIntegral::location_t where,
											 const CdfTangible* piece,
											 const point_t& xyz,
											 const IntegrationData& data)
	{
		if ( _legs.empty() ) 
			_appendToEmptyPath(where, piece, xyz, data);
		else if ( _legs.back().where() == where )
			_appendToSameLeg(xyz, data);
		else
			_appendToNewLeg(where, piece, xyz, data);
	}

	void
	PathIntegral::concisePrint(ostream& os) const
	{
		copy(_legs.begin(), 
				 _legs.end(), 
				 ostream_iterator<IntegrationLeg>(os, "\n"));
	}

	void
	PathIntegral::print(ostream& os) const
	{
		const_iter it, end = _legs.end();
		index_t i;
		for ( it = _legs.begin(), i = 0; it != end; ++it, ++i)
			os << "Leg #" << i << '\n' << (*it) << '\n';
	}


} // namespace sim

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

