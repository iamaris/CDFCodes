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
# QQ setup
#
# default $QQ_DECAY_FILE = $QQ_DIR/lib/decay.dbt
#
# if you want to use your own QQ_DECAY_FILE,
# define $QQ_USER_FILE=your_decay_file
#
module enable QQModule
module talk   QQModule
# note qq default is to decay only Bu Bd Bs (as set in CDF interface to qq)
# Herwig default (set in HerwigInterface.F) is to NOT decay hadrons that
# contain 1 or more b or c quarks - let qq do ALL such decays. So for
# most running the next 4 lines are necessary.
  Decay_Bc           set 1
  Decay_Upsilon      set 1
  Decay_B_Baryons    set 1
  Decay_prompt_charm set 1
  show
exit
#-----------------------------------------------------------------------