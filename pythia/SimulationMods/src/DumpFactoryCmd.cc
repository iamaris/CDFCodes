#include <iostream>
#include "SimulationMods/DumpFactoryCmd.hh"
#include "FrameUtil/AbsInterp.hh"
#include "SimulationMods/G3Data.hh" // for G3Factory

namespace sim {
	// ------------------------------------------------
	int 
	DumpFactoryCmd::handle(int , char**) {

		G3Factory* factory = G3Factory::instance();
		factory->dump(std::cout);
		return AbsInterp::OK;
	}
	
} // namespace sim
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
