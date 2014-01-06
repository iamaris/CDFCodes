#ifndef SIM_GCCUTSCMDCMD_INCLUDED
#define SIM_GCCUTSCMDCMD_INCLUDED 1

#include <string>

#include "FrameUtil/APPCommand.hh"

class AppModule;

namespace sim {

	class GccutsCmd : public APPCommand
	{
	public:
		GccutsCmd(AppModule* m) :
			APPCommand("gccuts",m)
			{ }

		GccutsCmd(char* n, char* des, AppModule* m) :
			APPCommand(n,m)
			{ }

		~GccutsCmd() 
			{ }

		void show() const ;

		bool isShowable() const 
			{ return true; }
	
		std::string description() const ;

		int handle(int argc, char* argv[]);

	protected:

	}; // class GccutsCmd

} // namespace sim

#endif // SIM_GCCUTSCMDCMD_INCLUDED

