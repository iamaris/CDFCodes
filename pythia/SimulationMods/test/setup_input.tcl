##########################################################################
# Setup GenInputManager used to set Run number for realistic MC
# GenOutputManager enabled by default
# supported input modes : 1: mc generator and 2: hepg file 
# default is the first one
##########################################################################
set CDFSIM_INPUT          [ getenv CDFSIM_INPUT       "MC_GENERATOR" ]
set INPUT_FILE            [ getenv INPUT_FILE         "undefined"    ]
set EXCL_EVT_DIR          [ getenv RUNMC_TCL_DIR      "."            ]
set FIRST_EVENT_OFFSET    [ expr $BEGIN_EVENT_NUMBER-1               ]
#-----------------------------------------------------------------------
# Setup input module for generators
#-----------------------------------------------------------------------

proc setup_mc_generator_input {} {

    echo "setup_input.tcl:setup_mc_generator_input"

    global RUN_NUMBER    RUN_SECTION   FIRST_EVENT_OFFSET
    global RANDOM_SEED_INPUT RANDOM_SEED_OUTPUT
    global EXCL_EVT_DIR

module input GenInputManager
talk GenInputManager
  report             set [ getenv REPORT_FREQUENCY    1 ]
  first_event        set $FIRST_EVENT_OFFSET
  run_number         set $RUN_NUMBER
  RunSectionOffset   set [ expr $RUN_SECTION-1 ]

    echo "\[setup_input.tcl:setup_mc_generator_input\]" FIRST_EVENT_OFFSET= $FIRST_EVENT_OFFSET
    echo "\[setup_input.tcl:setup_mc_generator_input\]" RUN_NUMBER        = $RUN_NUMBER
    echo "\[setup_input.tcl:setup_mc_generator_input\]" RUN_SECTION       = $RUN_SECTION

    #-- Exclude selected Run/Event pairs
    if [file exists $EXCL_EVT_DIR/input_exclude.tcl] {
        puts "\[setup_input.tcl:setup_mc_generator_input\] Sourcing exclude event tcl..."
	source $EXCL_EVT_DIR/input_exclude.tcl
    }

  RunSectionLength   set 1000000
#  LumiWtRunsFromDB  set true
#  runNumberFile set ./runlist
  show
exit

module enable GenOutputManager
}

#-----------------------------------------------------------------------
# setup HEPG input mode
#-----------------------------------------------------------------------
proc setup_hepg_input {} {
  global INPUT_FILE
  global EXCL_EVT_DIR
#-----------------------------------------------------------------------
# by default everything in HardScatGenSequence is disabled
# Get input file. Variable INPUT_FILE has to be defined in one of the
# template*.tcl files used
#-----------------------------------------------------------------------
  module input  FileInput
  module enable FileInput
  alias include input
  talk FileInput
    include file $INPUT_FILE

    #-- Exclude selected Run/Event pairs
    if [file exists $EXCL_EVT_DIR/input_exclude.tcl] {
        puts "\[setup_input.tcl:setup_hepg_input\] Sourcing exclude event tcl..."
	source $EXCL_EVT_DIR/input_exclude.tcl
    }

    show
  exit
}
#------------------------------------------------------------------------------
#
#------------------------------------------------------------------------------
# Setup random seeds
talk RandomGenManager
   RestoreRandomStreams set true
   SaveRandomStreams set true
   InputFileName  set $RANDOM_SEED_INPUT
   OutputFileName set $RANDOM_SEED_OUTPUT
   show
exit

if { $CDFSIM_INPUT == "MC_GENERATOR" } then { setup_mc_generator_input } 
if { $CDFSIM_INPUT == "HEPG_FILE"    } then { setup_hepg_input         } 
#------------------------------------------------------------------------------
