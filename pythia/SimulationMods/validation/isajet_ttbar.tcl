# to be used with cdfSim or cdfGen
#
# Isajet: p pbar --> ttbar
#
# output is in /cdf/data02/s5/mc/lena/validation

path disable AllPath
path create gen_path

module disable Bgenerator

mod input Isajet
mod output DHOutput

set env(CDFPVLI) $env(CDFSOFT2_DIR)/generatorMods/examples/isajet/top175.val

mod talk Isajet 
  call_qq set t
exit

module talk DHOutput
  output create gen_stream isajet_ttbar_3_17_0int2.gen
  output path gen_stream gen_path 
  output keepList gen_stream  LRIH_StorableBank EVCL_StorableBank HEPG_StorableBank
exit
     
begin -nev 5000
  
exit
