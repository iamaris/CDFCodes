# to be used with cdfSim;
# this is an example how to stop tracking outsite COT volume,
# only COT and SVX simulation is used
# ( usefull for tracking group )

path enable AllPath
creator set NSIM

# input module for generators
module input GenInputManager

mod enable FAKE_EVENT
talk FAKE_EVENT
  use PT
  use THETA
  use PHI
  generate PT  10. 0. 10. 10. 0. 1.
  generate PHI  180. 0. 360. 270. 0. 2
  generate THETA 90. 10. 45. 135. 1.
# Generate mu-
  generate CDFCODE 207
  generate NPARTICLES 1
#  show
exit

talk GeometryManager
  DetectorMenu
     enableTof set true
     enablePassive set true
     enableBeampipeC set true   # central part of the beampipe
  exit
  TofGeometryMenu
    GeometryModel set Survey   # options: Nominal, Naive
  exit
exit

talk SimInitManager
  DetectorMenu
     declareSvx set t 
     declareCot set t
     declareCPR set f
  exit
exit

talk SimulationControlMod
  DebugMenu
# uncomment to print out where the tracking stopped
#   DebugLevel set 21
  exit
  DetectorMenu
     simulateSvx set t
     simulateCot set t
  exit
  dumpFactory
  add CdfHalfLadder SvxDigitizer SvxGroup
  add CotSuperLayer CotDigitizer CotGroup
  show add
  ConfigMenus
    CotGroup_CotSuperLayer
      DriftModel set Garfield
    exit
  exit
  fastTrack set true  ####  this is the switch!
exit

mod output FileOutput
talk FileOutput
  output create main_stream cdfSim.root
  output path main_stream AllPath
  output keepList main_stream \
                    LRIH_StorableBank EVCL_StorableBank HEPG_StorableBank \
                    TL2D_StorableBank OBSP_StorableBank OBSV_StorableBank \
                    COTD_StorableBank CMPD_StorableBank CMUD_StorableBank \
                    CSPD_StorableBank CMXD_StorableBank IMUE_StorableBank \
                    SIXD_StorableBank MSVX_StorableBank ISLD_StorableBank \
                    MISL_StorableBank MCOT_StorableBank                   \
                    COTQ COTM_StorableBank 
exit

begin -nev 10
show timer

exit




