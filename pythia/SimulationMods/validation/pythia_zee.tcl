# to be used with cdfSim or cdfGen
#
# Pythia: Z->ee
#
# output is in /cdf/data02/s5/mc/lena/validation

path disable AllPath
path create gen_path

module disable Bgenerator

mod input Pythia
mod output DHOutput

mod talk Pythia
  call_qq set t
  output set HEPG
  MSEL set 0
  commonMenu 
    for {set i 1} {$i<100} {incr i} {
      set_msub -index=$i -value=0
    }
    # Produce Zs only
    set_msub -index=15 -value=1
    set_msub -index=19 -value=0
    set_msub -index=30 -value=0
    show_msub
    # Z->ee only
    set_mstp -index=43 -value=2
    show_mstp
    for {set i 174} {$i<190} {incr i} {
      set_mdme -channelIndex=$i -decayType=0
    }
    set_mdme -channelIndex=182 -decayType=1
  exit
exit

module talk DHOutput
  output create gen_stream pythia_zee_3_17_0int2.gen
  output path gen_stream gen_path 
  output keepList gen_stream  LRIH_StorableBank EVCL_StorableBank HEPG_StorableBank
exit
     
begin -nev 5000
  
exit
