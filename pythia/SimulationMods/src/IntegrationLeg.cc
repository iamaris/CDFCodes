////////////////////////////////////////////////////////////////////////
//
// File: IntegrationLeg.cc
// $Id: IntegrationLeg.cc,v 1.5 2001/04/23 16:03:55 paterno Exp $
// Purpose: Definition of the class IntegrationLeg. See the header
// for details.
//
// Author:  Marc Paterno
// Created: 26 Feb 2001
// Modified:
//
////////////////////////////////////////////////////////////////////////
#include <cassert>
#include "SimulationMods/IntegrationLeg.hh"
#include "GeometryBase/CdfTangible.hh"
#include "GeometryBase/CdfPhysicalVolume.hh"
#include "GeometryBase/CdfLogicalVolume.hh"
#include "GeometryBase/CdfMaterial.hh"

using namespace std;

namespace  // avoid polluting global namespace
{
	inline
	string 
	GetMaterialName(const CdfTangible* p)
	{
		return ( p == 0 )
			?  string("unk")
			: p->physicalVolume()->getLogicalVolume()->getMaterial()->getName();
	}
}

namespace sim
{
	IntegrationLeg::IntegrationLeg() :
		_current(),
		_cumulative(),
		_location(),
		_start(0.0, 0.0, 0.0),
		_end(0.0, 0.0, 0.0),
		_pieceCache(0),
		_materialName()
	{ }

	IntegrationLeg::IntegrationLeg(const location_t& where,
																 const CdfTangible* piece,
																 const point_t& xyz) :
		_current(),
		_cumulative(),
		_location(where),
		_start(xyz),
		_end(xyz),
		_pieceCache(piece),
		_materialName(GetMaterialName(piece))
	{ }

	IntegrationLeg::IntegrationLeg(const location_t& where,
																 const CdfTangible* piece,
																 const point_t& xyz,
																 const IntegrationData& data) :
		_current(data),
		_cumulative(data),
		_location(where),
		_start(xyz),
		_end(xyz),
		_pieceCache(piece),
		_materialName(GetMaterialName(piece))
	{ }

	IntegrationLeg::IntegrationLeg(const location_t& where,
																 const CdfTangible* piece,
																 const point_t& newEnd,
																 const IntegrationLeg& previous,
																 const IntegrationData& newdata) :
		_current(newdata),
		_cumulative( previous.cumulative() + newdata ),
		_location(where),
		_start(previous.endPoint()),
		_end(newEnd),
		_pieceCache(piece),
		_materialName(GetMaterialName(piece))
	{ }

	IntegrationLeg::IntegrationLeg(const IntegrationLeg& rhs) :
		_current(rhs._current),
		_cumulative(rhs._cumulative),
		_location(rhs._location),
		_start(rhs._start),
		_end(rhs._end),
		_pieceCache(rhs._pieceCache),
		_materialName(rhs._materialName)
	{ }

	IntegrationLeg::~IntegrationLeg()
	{
	}

	void
	IntegrationLeg::swap(IntegrationLeg& other)
	{
		std::swap(_current, other._current);
		std::swap(_cumulative, other._cumulative);
		std::swap(_location, other._location);
		std::swap(_start, other._start);
		std::swap(_end, other._end);
		std::swap(_pieceCache, other._pieceCache);		
		_materialName.swap(other._materialName);
	}

	IntegrationLeg&
	IntegrationLeg::operator= (const IntegrationLeg& rhs)
	{
		IntegrationLeg temp(rhs);
		swap(temp);
		return *this;
	}

	IntegrationLeg::location_t
	IntegrationLeg::where() const
	{
		return _location;
	}


	void
	IntegrationLeg::concisePrint(ostream& os) const
	{
		os << '(' 
			 << _location 
			 << ")\t("
			 << _materialName
			 << ")\t"
			 << _start
			 << "\t["
			 << _current 
			 << "]\t["
			 << _cumulative
			 << "]\t"
			 << _end;
	}

	void
	IntegrationLeg::print(ostream& os) const
	{
		os << "Location:   " << _location << '\n'
			 << "Material:   " << _materialName << '\n'
			 << "Current   : " << _current << '\n'
			 << "Cumulative: " << _cumulative << '\n';
	}

	void
	IntegrationLeg::accumulate(const point_t& newEnd, 
														 const IntegrationData& data)
	{
		_end         = newEnd;
		_current    += data;
		_cumulative += data;
	}

	bool 
	IntegrationLeg::operator== (const IntegrationLeg& rhs) const
	{ 
		return ( _start      == rhs._start          &&
						 _end        == rhs._end            &&
						 _location   == rhs._location       &&
						 _current    == rhs._current        &&
						 _cumulative == rhs._cumulative     &&
						 _materialName == rhs._materialName);
	}

} // namespace sim

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

