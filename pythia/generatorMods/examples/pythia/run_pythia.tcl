#####################################################################
# Example Tcl File                                                  #
#                                                                   #
# tcl file for generating and simulating top events using           #
# PYTHIA in cdfGen or cdfSim                                        #         
#####################################################################
path enable AllPath
creator set NSIM

module input GenInputManager
module talk GenInputManager
# default run number
  run_number set 151435
exit 

exec rm -f run_pythia_test.root
module enable HepRootManager
mod talk HepRootManager
  verbose set true
  histfile set run_pythia_test.root
  createHistoFile set  t
exit

talk RandomGenManager
  SaveRandomStreams set true
  RandomNumberMenu
    RandomSeedPYTHIA1 set 12345
    RandomSeedPYTHIA2 set 98765
  exit
exit 

mod enable Pythia 
# Pythia talk-to fragment
# use this tcl fragment first to set pythia masses as per EvtGen 
# version of Oct2004
source $env(CDFSOFT2_DIR)/pythia_i/test/EvtGen_mass.tcl 
# Pythia talk-to fragment to generate top events
source $env(CDFSOFT2_DIR)/pythia_i/test/Pythia_top1.tcl

talk GeometryManager
# Use the defaults but substitute the detailed for the simple Si detector
# The following set up is to be added to the default GeometryManager set up.
  DetectorMenu
     enableTof set true
     enablePassive set true
     enableBeampipeC set true   # central part of the beampipe
     show
  exit
# TOF geometry model
  TofGeometryMenu
    GeometryModel set Survey   # options: Nominal, Naive
  exit
exit

mod enable GenOutputManager
talk FileOutput
  output create main_stream pythia_wwee_test.root
  output path main_stream AllPath
  output keepList main_stream \
                    LRIH_StorableBank EVCL_StorableBank HEPG_StorableBank \
                    TL2D_StorableBank OBSP_StorableBank OBSV_StorableBank \
                    COTD_StorableBank CMPD_StorableBank CMUD_StorableBank \
                    CSPD_StorableBank CMXD_StorableBank IMUE_StorableBank \
                    SIXD_StorableBank MSVX_StorableBank ISLD_StorableBank \
                    MISL_StorableBank MCOT_StorableBank TOFD_StorableBank \
                    COTQ COTM_StorableObject \
                    PESD_StorableBank CESD_StorableBank CPRD_StorableBank \
                    CEMD_StorableBank PEMD_StorableBank CHAD_StorableBank \
                    WHAD_StorableBank PHAD_StorableBank PPRD_StorableBank \
                    MuonXHitColl MuonTHitColl MuonXZHitColl \
		    TOFD_StorableBank \
		    TofPulse TofPulseColl TofMatch TofMatchColl
exit

begin -nev 10
show timer

exit




