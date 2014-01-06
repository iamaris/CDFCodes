###########################################################################
#                                                                         #
#  cdfSim validation script to use default setup's                        #
#                                                                         #
###########################################################################

###########################################################################
### Use official location to source tcl files for chosen release.
set SIM_TCL_DIR $env(PROJECT_DIR)/SimulationMods/test
set output_dir  $env(VAL_DATA_DIR)

###########################################################################
###### Set number of events to generate, report frequency and
###### offset of the 1st event
set NEVENTS            10
set REPORT_FREQUENCY    1
set FIRST_EVENT_OFFSET  0

###########################################################################
### Random number business 
###### Read initial random seeds for random number streams from file
set RANDOM_SEED_INPUT $TCL_LOCATION/setup_random_seed.dat
###### Specify file name to store random seeds at end job
set RANDOM_SEED_OUTPUT $output_dir/CdfRnStat.dat

###########################################################################
### Use default simulation settings for validation
###### Specify run number here
set RUN_NUMBER 154653

###########################################################################
### Specify name of your output file
set OUTPUT_FILE $output_dir/isajet_qq_today.root

###########################################################################
### Setup default simulation  
source $SIM_TCL_DIR/setup_simulation.tcl
source $SIM_TCL_DIR/setup_input.tcl
source $SIM_TCL_DIR/setup_output.tcl
###########################################################################

###########################################################################
### Use Pythia tt= for validation
mod enable Pythia
source $env(CDFSOFT2_DIR)/pythia_i/test/Pythia_top2.tcl

begin -nev $NEVENTS 
show timer
exit
