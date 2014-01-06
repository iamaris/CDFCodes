#
# QQ setup
#
# default $QQ_DECAY_FILE = $QQ_DIR/lib/decay.dbt
#
# if you want to use your own QQ_DECAY_FILE,
# define $QQ_DECAY_FILE=your_decay_file
#
set QQ_DECAY_FILE [ getenv QQ_DECAY_FILE  "$env(QQ_DIR)/lib/decay.dbt" ]

module enable QQModule
module talk   QQModule
  Decay_Bc           set 1
  Decay_Upsilon      set 1
  Decay_B_Baryons    set 1
  Decay_prompt_charm set 1
  show
exit
#-----------------------------------------------------------------------