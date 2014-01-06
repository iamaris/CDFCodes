###########################################################################
#                                                                         #
#  cdfSim master configuration file                                       #
#                                                                         #
###########################################################################
#
# Use official location to source tcl files for chosen release.
# Unless you are using a test release
#
if [ file exists $env(SRT_PRIVATE_CONTEXT)/SimulationMods/test ] { 
 set SIM_TCL_DIR $env(SRT_PRIVATE_CONTEXT)/SimulationMods/test
} else { 
 set SIM_TCL_DIR $env(SRT_PUBLIC_CONTEXT)/SimulationMods/test
}
#
# Use TclUtils package to change settings through environment variables  
#
if [ file exists $env(SRT_PRIVATE_CONTEXT)/TclUtils ] { 
  source $env(SRT_PRIVATE_CONTEXT)/TclUtils/scripts/getenv.tcl
} else { 
  source $env(SRT_PUBLIC_CONTEXT)/TclUtils/scripts/getenv.tcl
}
#
# Set a run number to simulate (default: run 151435 <Lumi>=1.20e+31) 
# number of events to generate, report frequency, first event number 
# 
set DFC_BOOK           [ getenv DFC_BOOK       "wewk9t"]
set NEVENTS            [ getenv NEVENTS           "10" ]
set RUN_NUMBER         [ getenv RUN_NUMBER     "151435"]
set BEGIN_EVENT_NUMBER [ getenv BEGIN_EVENT_NUMBER  1  ]
set RUN_SECTION        [ getenv RUN_SECTION         1  ]
#
# Specify the tcl for generator, output file name and
# initial/final random seeds for random number streams
#
set GENERATOR_TCL      [ getenv GENERATOR_TCL "$SIM_TCL_DIR/setup_fakeev.tcl" ]
set OUTPUT_FILE        [ getenv OUTPUT_FILE                       "test.root" ]
set RANDOM_SEED_INPUT  [ getenv RANDOM_SEED_INPUT "$SIM_TCL_DIR/setup_random_seed.dat" ]
set RANDOM_SEED_OUTPUT [ getenv RANDOM_SEED_OUTPUT            "CdfRnStat.dat" ]
#
# switch for UserInfoColl
#
set STORE_USERINFO     [ getenv STORE_USERINFO "0" ]
###########################################################################
### Setup default simulation for MC_PROCESS_TCL
###########################################################################
set    outputPath   cdfSimPath
source $SIM_TCL_DIR/setup_simulation.tcl
source $SIM_TCL_DIR/setup_input.tcl
source $SIM_TCL_DIR/setup_path.tcl
source $SIM_TCL_DIR/setup_output.tcl
source $GENERATOR_TCL
###########################################################################
# Uncomment following lines to overlay additional min bias events with 
# MEAN=[0,1], MODE=[POISSON,FIXED], TCL=[PYTHIA_TUNE_A,MBR]
# or append them your $GENERATOR_TCL
###########################################################################
#set ADD_MINBIAS_MEAN [ getenv ADD_MINBIAS_MEAN       "0" ]
#set ADD_MINBIAS_MODE [ getenv ADD_MINBIAS_MODE "POISSON" ]
#set ADD_MINBIAS_TCL  [ getenv ADD_MINBIAS_TCL "$SIM_TCL_DIR/setup_MinBias_Pythia_Tune_A.tcl" ]
#source $ADD_MINBIAS_TCL

begin -nev $NEVENTS 
show timer
exit
