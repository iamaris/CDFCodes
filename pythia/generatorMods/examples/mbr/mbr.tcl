// how to run mbr with cdfGen### MBR for use with cdfGen executable
# example tcl file for generating minimum-bias (elastic, diffractive 
# and non-diffractive) events using cdfGen
# for details on talk-to's, etc, see CDF 5371

path enable AllPath
module input GenInputManager

mod enable mbr
mod talk mbr
  pbeam set 980
### process: combination of Hard Core, Double-, Single-Diffractive and
### Elastic: 1*HC+10*DD+100*SD+1000*EL, eg 101=HC+SD
### Do not add leading Zeros
### Default: generate hard core (inelastic, non-diffractive) events only (1)#  
### hard-core+double-diffractive (events that likely pass 
### CLC min-bias trigger)
  process set 11
#
###single-diffractive
#  process set 100
#  ximax set 0.10
#  ximin set 0.001
#  sd_p_only set 1
###
  show
exit

module talk FileOutput
  output create myout mbr_test.out
  output path myout AllPath
  output keepList myout EVCL_StorableBank LRIH_StorableBank HEPG_StorableBank
  output list
exit

begin -nev  10

exit
