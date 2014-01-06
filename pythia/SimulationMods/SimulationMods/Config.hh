#ifndef SIM_CONFIGINFO_INCLUDED
#define SIM_CONFIGINFO_INCLUDED 1

// $Id: Config.hh,v 1.5 2000/12/04 15:36:01 paterno Exp $

#include <map>
#include <list>
#include <string>
#include <iostream>

namespace sim {

	struct ConfigInfo
	{
		ConfigInfo() { }
		ConfigInfo(const std::string& digi, const std::string& group):
			digi_class(digi),group_name(group) { }

		std::string digi_class;
		std::string group_name;
	};

	// string Key = detector element class name

	typedef std::list< ConfigInfo > ConfigInfoList;
	typedef std::map<std::string,ConfigInfoList> ConfigInfoMap;

	std::ostream& operator<<(std::ostream& os,
													 const ConfigInfoMap& cim);

}
#endif
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
