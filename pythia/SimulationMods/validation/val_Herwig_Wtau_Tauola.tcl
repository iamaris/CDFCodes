##########################################################################
mod enable herwig
mod enable TauolaModule
# Herwig talk-to fragment
talk herwig
# help
# 1453 is qq' -> W -> tau.nutau
# 2300 is SM Higgs + jet prodn.
# 2100 is W + jet production
# 1500 is QCD 2 -> 2 hard parton scattering
 Process set 1453
#
 Beam
# Default Beam energies are 980. 980.
   Pbeam1 set 980.
   Pbeam2 set 980.
 exit
 Masses
   top set 175
 exit
#
#  Structure functions, default is MRST 1998 LO (v6_4)
# Struc_Function
#   Autpdf set MRS
#   Modpdf set 41
# exit
#
# Following 6 lines lead to a print (internal to herwig) of 1 event,
#   1 line per entity (so ~500 lines for ttbar), each line has width
#   ~120 characters. If use   prvtx set 0   then VHEP info is not printed
#    and width is ~80.
#   Default is no event printing (maxpr = 0).
 Prints
   help
   maxpr set 2
   prvtx set 0
   show
 exit
# default Clpow leads to no B-Baryons
 Hadronization
   Clpow set 1.26
 exit
#
   show
exit
##########################################################################
