## run_isajet.tcl  using executable cdfGen,  Sept 2001 jlys et al.

## NOTE: user-control variables are set in $CDFPVLI. The default
##   CDFPVLI is $CDFSOFT2_DIR/generatorMods/examples/isajet/top175.val
## For a simple MSSM test, do:
#set env(CDFPVLI) $env(GENERATORMODS_DIR)/examples/isajet/mssm.val
## For your own version of the .val file, say /cdf/scratch/me/xxxx.val, do
#set env(CDFPVLI) /cdf/scratch/me/xxxx.val

path enable AllPath

module input GenInputManager
module talk GenInputManager
# default run number
  run_number set 151435
exit 

# enable generator ( no generators are enabled bu default )
mod enable Isajet

exec rm -f run_isajet_test.root
module enable HepRootManager
mod talk HepRootManager
  verbose set true
  histfile set run_isajet_test.root
  createHistoFile set  f
exit

module talk FileOutput
 output create myout isajet_test.out
 output path myout AllPath
 output keepList myout EVCL_StorableBank LRIH_StorableBank HEPG_StorableBank
 output list
exit

#  
begin -nev  10

show timer
exit
