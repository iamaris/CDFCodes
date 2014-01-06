////////////////////////////////////////////////////////////////////////
//
// File: VolumeNamePrinter.
// $Id: VolumeNamePrinter.hh,v 1.1 2001/03/27 16:00:14 paterno Exp $
// Purpose: Declare and define the struct VolumeNamePrinter; this
// struct exists to translate the GEANT3 form of a volume name (four
// characters encoded in an INTEGER*4 variable) into a form that may
// be more easily printed.
//
// This was originally in SimulationControl.cc, and has been moved out
// for more general use. The implementation has also been cleaned up.
//
// Author:  Marc Paterno
// Created: 27 Mar 2001
// Modified:
//
////////////////////////////////////////////////////////////////////////

#include <ostream>
#include <iterator>
#include <algorithm>

namespace sim
{

	struct VolumeNamePrinter
	{
		VolumeNamePrinter(const int* volname);
		const char* volname_;
	};


	inline
	VolumeNamePrinter::VolumeNamePrinter(const int* volname) : 
		volname_(reinterpret_cast<const char*>(volname)) 
	{ }

	inline
	std::ostream& operator<<(std::ostream& ost, 
													 const VolumeNamePrinter& vn)
	{
		std::ostreambuf_iterator<char> out(ost);
		std::copy(vn.volname_ + 0, 
							vn.volname_ + 4, 
							out);
		return ost;
	}

} // namespace sim
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
