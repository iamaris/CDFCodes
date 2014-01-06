#ifndef SIM_DUMPFACTORYCMD_INCLUDED
#define SIM_DUMPFACTORYCMD_INCLUDED 1

#include <string>

#include "FrameUtil/APPCommand.hh"
#include "SimulationMods/G3Data.hh"

class AppModule;

namespace sim {

	class DumpFactoryCmd : public APPCommand
	{
	public:
		DumpFactoryCmd(AppModule* m) :
			APPCommand("dumpFactory",m),
			desc("Dump the Factory")
			{ }

		DumpFactoryCmd(char* n, char* des, AppModule* m) :
			APPCommand(n,m),
			desc(des) 
			{ }

		~DumpFactoryCmd() 
			{ }

		void show() const 
			{ }

		bool isShowable() const 
			{ return false; }
	
		std::string description() const 
			{ return desc; }

		int handle(int argc, char* argv[]);

	protected:
		std::string desc;

	}; // class DumpFactoryCmd

} // namespace sim

#endif // SIM_DUMPFACTORYCMD_INCLUDED
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
