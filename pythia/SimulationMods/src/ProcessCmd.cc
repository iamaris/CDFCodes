#include <iostream>
#include "SimulationMods/ProcessCmd.hh"
#include "FrameUtil/AbsInterp.hh"
#include "geant_i/TGeant3.h"

using std::cout;

namespace sim {
	// ------------------------------------------------
	int 
	ProcessCmd::handle(int argc, char* argv[]) {

		// Get the G3 common blocks:
    Gcphys_t             fGcphys;
		TGeant3 *g3 = TGeant3::Instance();
    g3->GetGcphys(fGcphys);

    if( argc!=3) {
      std::cout << " Wrong number of arguments.  PROCESS  unchanged" <<std::endl;
      return AbsInterp::ERROR;
     }
     int val = atoi(argv[2]);
     if(val < 0 || val > 4) {
       std::cout << "Illegal control value: " << val 
						<< " process: " << argv[1] << " unchanged"
	    << std::endl;
      return AbsInterp::ERROR;
     }
     if(strncmp(argv[1],"DCAY",4)==0) {
       fGcphys.IDCAY = val;
     }
     else if(strncmp(argv[1],"MULS",4)==0) {
       fGcphys.IMULS = val;
     }
     else if(strncmp(argv[1],"LOSS",4)==0) {
       fGcphys.ILOSS = val;
     }
     else if(strncmp(argv[1],"PHOT",4)==0) {
       fGcphys.IPHOT = val;
     }
     else if(strncmp(argv[1],"COMP",4)==0) {
       fGcphys.ICOMP = val;
     }
     else if(strncmp(argv[1],"PAIR",4)==0) {
       fGcphys.IPAIR = val;
     }
     else if(strncmp(argv[1],"BREM",4)==0) {
       fGcphys.IBREM = val;
     }
     else if(strncmp(argv[1],"DRAY",4)==0) {
       fGcphys.IDRAY = val;
     }
     else if(strncmp(argv[1],"ANNI",4)==0) {
       fGcphys.IANNI = val;
     }
     else if(strncmp(argv[1],"HADR",4)==0) {
       fGcphys.IHADR = val;
     }
     else {
       std::cout << " Unknown process " << argv[1] << std::endl;
       return AbsInterp::ERROR;
     }
     g3->SetGcphys(fGcphys);

		return AbsInterp::OK;
	}

 std::string ProcessCmd::description() const 
 {  std::string retval;
    retval += "Specify process keyword and control number \n";
    retval += "\t \t Syntax is: process KEYWORD CONTROL_NUMBER\n";
    retval += "\t \t Keywords: DCAY MULS LOSS PHOT COMP \n";
    retval += "\t \t \t   PAIR BREM DRAY ANNI HADR \n";
    retval += "\t \t Control_number  0=process ignored\n";
    retval += "\t \t \t \t 1=process considered, secondaries tracked\n";
    retval += "\t \t \t \t 2=process considered, secondaries NOT tracked\n";
    retval += "\t \t See Geant3 PHYS001 writeup for details\n";
    return retval;
 }

 void ProcessCmd::show() const 
 {
		// Get the G3 common blocks:
    Gcphys_t             fGcphys;
		TGeant3 *g3 = TGeant3::Instance();
    g3->GetGcphys(fGcphys);

    std::cout << "Status of Physics Processes: " << std::endl;
    std::cout << "\t \t  DCAY =  " << fGcphys.IDCAY << std::endl;
    std::cout << "\t \t  MULS =  " << fGcphys.IMULS << std::endl;
    std::cout << "\t \t  LOSS =  " << fGcphys.ILOSS << std::endl;
    std::cout << "\t \t  PHOT =  " << fGcphys.IPHOT << std::endl;
    std::cout << "\t \t  COMP =  " << fGcphys.ICOMP << std::endl;
    std::cout << "\t \t  PAIR =  " << fGcphys.IPAIR << std::endl;
    std::cout << "\t \t  BREM =  " << fGcphys.IBREM << std::endl;
    std::cout << "\t \t  DRAY =  " << fGcphys.IDRAY << std::endl;
    std::cout << "\t \t  ANNI =  " << fGcphys.IANNI << std::endl;
    std::cout << "\t \t  HADR =  " << fGcphys.IHADR << std::endl;
 }	

} // namespace sim
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
