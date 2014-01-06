#-----------------------------------------------------------------------
# by default everything is disabled in HardScatGenSequence:
#-----------------------------------------------------------------------
module enable TauolaModule
module talk   TauolaModule
  Display_max_taudecays set 1
  Display_tauola_output set 3
exit
#-----------------------------------------------------------------------
#
# EvtGen setup
#
# default $EVTGEN_DECAY_FILE = EvtGen/DECAY.DEC
#         $EVTGEN_PDT_TABLE  = EvtGen/pdt.table
#
# if you want to use your own EVTGEN_DECAY_FILE,
# define EVTGEN_USER_DECAY_FILE=your_decay_file
#
set EVTGEN_USER_DECAY_FILE [ getenv EVTGEN_USER_DECAY_FILE  "0" ]

module enable EvtGen
module talk   EvtGen
  DecayPromptCharm set true
  DecayBMeson      set true
  DecayBBaryon     set true
  if { ${EVTGEN_USER_DECAY_FILE} != "0"} {
    UseUserDecayFile set true
    UserDecayFile set ${EVTGEN_USER_DECAY_FILE}
  }
  show
exit
#-----------------------------------------------------------------------