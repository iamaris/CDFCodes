##########################################################################
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
##########################################################################
