############################################################################
# Tcl file to overlay additional min bias events using MBR
############################################################################
mod enable mbr
mod talk mbr
   pbeam set 980
   process set 11
   MEAN set $ADD_MINBIAS_MEAN 
   MODE set $ADD_MINBIAS_MODE
   show
exit

############################################################################
