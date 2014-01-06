path enable AllPath

# input module for generators
module input GenInputManager

# enable generator ( no generators are enabled bu default )
mod enable StdhepInput
mod talk StdhepInput
  Listfirst set 1
  Listlast set 2
StdhepFile set mad_e+e-jj.stdhep
#  show
exit

talk DHOutput
  output create main_stream test.hepg
  output path main_stream AllPath
  output keepList main_stream \
                    LRIH_StorableBank EVCL_StorableBank HEPG_StorableBank 
exit

begin -nev 100
show timer

exit