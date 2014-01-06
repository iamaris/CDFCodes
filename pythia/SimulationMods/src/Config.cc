////////////////////////////////////////////////////////////////////////
//
// File: Config.cc
// $Id: Config.cc,v 1.1 2000/12/04 15:33:33 paterno Exp $
// Purpose: Implementation of the stream insertion method for 
//          ConfigInfoMap.
//
// Created: 25-Sep-2000 Marc Paterno
// Modified:
//
//
////////////////////////////////////////////////////////////////////////
#include "SimulationMods/Config.hh"

using namespace std;
namespace sim
{
	ostream& operator<< (ostream& os, const ConfigInfoMap& cim)
	{
		ConfigInfoMap::const_iterator it;
		ConfigInfoMap::const_iterator end = cim.end();
		os << "There are " 
			 << cim.size() 
			 << " entries in this ConfigInfoMap\n";

		if (it != end)
			{
				os << "The recognized detector element types are:\n";
			}
		for (it = cim.begin(); it != end; ++it)
				os << "\t" << (*it).first << "\n";

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

