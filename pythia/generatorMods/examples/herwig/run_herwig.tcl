### for cdfGen executable
# ( adapted from /herwig_i/examples/herwig_test.tcl for cdfGen )


path enable AllPath

module input GenInputManager
module talk GenInputManager
# default run number
  run_number set 151435
exit 

#-->--> H e r w i g    p a r t 

mod enable herwig
mod talk herwig
# 1453 is qq' -> W -> tau.nutau
# 2300 is SM Higgs + jet prodn.
# 2100 is W + jet production
# 1500 is QCD 2 -> 2 hard parton scattering
 Process set 1453
# Following gets 1 line on screen per 100 events
#  report set 100
# seeds for herwig - note there may be TWO random number schemes ***
 Randseed1 set 2222222
 Randseed2 set 44445444
#
 Beam
# Default Beam energies are 900. 900.
   Pbeam1 set 900.
   Pbeam2 set 900.
 exit
 Masses
   top set 175
 exit
#
#  Structure functions, default is MRS 41 (cdf v6_202)
# Struc_Function
#   Autpdf set MRS
#   Modpdf set 41
# exit
# note default is to NOT call qq, but BEWARE if you do not use qq
# qq is in the separate module QQModule

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
#
# Decays
#   help
# Following line sets pizero as stable, so get no pizero-to-gamma.gamma
#    decays. Default is pizero decays.
#   Pizstable set 1
# See manual (Herwig 6 sect 3.4 p 14) for Modbos info - inoperative on ttbar
#   Modbos1 set 4
#   Modbos2 set 4
#   show
# exit
#
# default Clpow leads to no B-Baryons
  Hadronization
    Clpow set 1.26
 exit
#
 show
exit

#-->--> Decayer (EvtGen is the CDF default) 

module enable EvtGen
module talk   EvtGen
  DecayPromptCharm set true
  DecayBMeson      set true
  DecayBBaryon     set true
  show
exit


exec rm -f run_herwig_test.root
module enable HepRootManager
mod talk HepRootManager
 verbose set true
 histfile set run_herwig_test.root
 createHistoFile set  f
exit 

#-->--> Write     o u t p u t

module talk FileOutput
  output create myout herwig_test.out
  output path myout AllPath
  output keepList myout EVCL_StorableBank LRIH_StorableBank HEPG_StorableBank
  output list
exit

begin -nev  10

exit

