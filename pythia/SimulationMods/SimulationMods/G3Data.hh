#ifndef SIM_G3DATA_INCLUDED
#define SIM_G3DATA_INCLUDED 1

#include <string>
#include <memory>

#include "SimulationBase/SimulationManager.hh"
#include "SimulationBase/Factory.hh"
#include "geant_i/G3Stepdata.hh"

namespace sim {
	typedef G3Stepdata step_data_type;
	typedef SimulationManager<step_data_type> G3SimMgr;
	typedef Factory<step_data_type> G3Factory;
	typedef G3SimMgr::se_type se_type;	
}
#endif 
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
