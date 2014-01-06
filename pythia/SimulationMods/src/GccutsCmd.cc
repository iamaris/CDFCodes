#include <iostream>
#include "SimulationMods/GccutsCmd.hh"
#include "FrameUtil/AbsInterp.hh"
#include "geant_i/TGeant3.h"

using std::cout;

namespace sim {
	// ------------------------------------------------
	int 
	GccutsCmd::handle(int argc, char* argv[]) {

		TGeant3 *g3 = TGeant3::Instance();

    if( argc!=3) {
      std::cout << " Wrong number of arguments.  GCCUTS  unchanged" <<std::endl;
      return AbsInterp::ERROR;
     }
     float val = atof(argv[2]);
     if(val < 0) {
       std::cout << "Illegal control value: " << val 
						<< " GCCUTS: " << argv[1] << " unchanged"
	    << std::endl;
      return AbsInterp::ERROR;
     }
     if(strncmp(argv[1],"CUTGAM",6)==0) {
       g3->Gccuts()->CUTGAM = val;
     }
     else if(strncmp(argv[1],"CUTELE",6)==0) {
       g3->Gccuts()->CUTELE = val;
     }
     else if(strncmp(argv[1],"CUTNEU",6)==0) {
       g3->Gccuts()->CUTNEU = val;
     }
     else if(strncmp(argv[1],"CUTHAD",6)==0) {
       g3->Gccuts()->CUTHAD = val;
     }
     else if(strncmp(argv[1],"CUTMUO",6)==0) {
       g3->Gccuts()->CUTMUO = val;
     }
     else if(strncmp(argv[1],"BCUTE",5)==0) {
       g3->Gccuts()->BCUTE = val;
     }
     else if(strncmp(argv[1],"BCUTM",5)==0) {
       g3->Gccuts()->BCUTM = val;
     }
     else if(strncmp(argv[1],"DCUTE",5)==0) {
       g3->Gccuts()->DCUTE = val;
     }
     else if(strncmp(argv[1],"DCUTM",5)==0) {
       g3->Gccuts()->DCUTM = val;
     }
     else if(strncmp(argv[1],"PPCUTM",6)==0) {
       g3->Gccuts()->PPCUTM = val;
     }
     else if(strncmp(argv[1],"TOFMAX",6)==0) {
       g3->Gccuts()->TOFMAX = val;
     }
     else {
       std::cout << " Error: see description of /GCCUTS/  " << argv[1] << std::endl;
       return AbsInterp::ERROR;
     }

		return AbsInterp::OK;
	}

 std::string GccutsCmd::description() const 
 {  std::string retval;
    retval += "Specify tracking thresholds for different particles \n";
    retval += "\t \t Syntax is: gccuts KEYWORD CONTROL_NUMBER\n";
    retval += "\t \t Keywords: CUTGAM CUTHAD CUTMUO BCUTE BCUTM \n";
    retval += "\t \t \t DCUTE DCUTM PPCUTM TOFMAX    \n";
    retval += "\t \t Control_number = float\n";
    retval += "\t \t See Geant3 ZZZZ010 writeup for details\n";
    return retval;
 }

 void GccutsCmd::show() const 
 {
		TGeant3 *g3 = TGeant3::Instance();

    std::cout << "Values of /GCCUTS/: " << std::endl;
    std::cout << "\t \t  CUTGAM =  " << g3->Gccuts()->CUTGAM << std::endl;
    std::cout << "\t \t  CUTELE =  " << g3->Gccuts()->CUTELE << std::endl;
    std::cout << "\t \t  CUTNEU =  " << g3->Gccuts()->CUTNEU << std::endl;
    std::cout << "\t \t  CUTHAD =  " << g3->Gccuts()->CUTHAD << std::endl;
    std::cout << "\t \t  CUTMUO =  " << g3->Gccuts()->CUTMUO << std::endl;
    std::cout << "\t \t  BCUTE  =  " << g3->Gccuts()->BCUTE << std::endl;
    std::cout << "\t \t  BCUTM  =  " << g3->Gccuts()->BCUTM << std::endl;
    std::cout << "\t \t  DCUTE  =  " << g3->Gccuts()->DCUTE << std::endl;
    std::cout << "\t \t  DCUTM  =  " << g3->Gccuts()->DCUTM << std::endl;
    std::cout << "\t \t  PPCUTM =  " << g3->Gccuts()->PPCUTM << std::endl;
    std::cout << "\t \t  TOFMAX =  " << g3->Gccuts()->TOFMAX << std::endl;
 }	

} // namespace sim
