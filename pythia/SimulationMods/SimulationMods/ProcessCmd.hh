#ifndef SIM_PROCESSCMDCMD_INCLUDED
#define SIM_PROCESSCMDCMD_INCLUDED 1

#include <string>

#include "FrameUtil/APPCommand.hh"

class AppModule;

namespace sim {

	class ProcessCmd : public APPCommand
	{
	public:
		ProcessCmd(AppModule* m) :
			APPCommand("process",m)
			{ }

		ProcessCmd(char* n, char* des, AppModule* m) :
			APPCommand(n,m)
			{ }

		~ProcessCmd() 
			{ }

		void show() const ;

		bool isShowable() const 
			{ return true; }
	
		std::string description() const ;

		int handle(int argc, char* argv[]);

	protected:

	}; // class ProcessCmd

} // namespace sim

#endif // SIM_PROCESSCMDCMD_INCLUDED
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
