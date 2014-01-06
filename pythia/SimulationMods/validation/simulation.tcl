# to be used with cdfSim or cdfGen
#
# input is in /cdf/data02/s5/mc/lena/validation
# output is in /cdf/data02/s5/mc/lena/validation

path enable AllPath

module disable Bgenerator

mod input FileInput
mod output DHOutput

module talk FileInput
  input file input.gen
exit

talk GeometryManager
# Use the defaults but substitute the detailed for the simple Si detector
# The following set up is to be added to the default GeometryManager set up.
  DetectorMenu
     enableTof set true
     enablePassive set true
     enableBeampipeC set true   # central part of the beampipe
     show
  exit
# TOF geometry model
    TofGeometryMenu
      GeometryModel set Survey   # options: Nominal, Naive
    exit
exit     

talk SimInitManager
  DetectorMenu
     declareSvx set t 
     declareCot set t
     declareMuon set t
     declareCalor set t
     declareTof  set t
     declarePassive set t
     declareCPR set f
  exit
exit

talk SimulationControlMod
  DebugMenu
    showActiveVolumes set t
  exit
  DetectorMenu
     simulateSvx set t
     simulateCot set t
     simulateMuon set t
     simulateTof set t
     simulateCalor set t
     simulatePassive set t
  exit
  dumpFactory
  add CdfHalfLadder SvxDigitizer SvxGroup
  add CotSuperLayer CotDigitizer CotGroup
  add CMUExtrusion MuonDigiCMU MuonCMUdata
  add CMPPart      MuonDigiCMP MuonCMPdata
  add CMXChamber   MuonDigiCMX MuonCMXdata
  add Tof3Pack TofDigi3Pack TofGroup
  add TofBar TofDigiBar   TofGroup
  add CalorDetectorElement CalorDigiGeneric CalorGroup 
  add BFCoil CalorDigiBFCoil CalorGroup
  add NoBFCoil CalorDigiNoBFCoil CalorGroup
  show add
  ConfigMenus
    CotGroup_CotSuperLayer
      DriftModel set Garfield
    exit
  exit
exit

talk DHOutput
  output create main_stream cdfSim.root
  output path main_stream AllPath
  output keepList main_stream \
                    LRIH_StorableBank EVCL_StorableBank HEPG_StorableBank \
                    TL2D_StorableBank OBSP_StorableBank OBSV_StorableBank \
                    COTD_StorableBank CMPD_StorableBank CMUD_StorableBank \
                    CSPD_StorableBank CMXD_StorableBank IMUE_StorableBank \
                    SIXD_StorableBank MSVX_StorableBank ISLD_StorableBank \
                    MISL_StorableBank MCOT_StorableBank TOFD_StorableBank \
                    COTQ COTM_StorableObject \
                    PESD_StorableBank CESD_StorableBank CPRD_StorableBank \
                    CEMD_StorableBank PEMD_StorableBank CHAD_StorableBank \
                    WHAD_StorableBank PHAD_StorableBank PPRD_StorableBank \
                    MuonXHitColl MuonTHitColl MuonXZHitColl \
		    TOFD_StorableBank \
		    TofPulse TofPulseColl TofMatch TofMatchColl
exit

begin -nev 5000
  
exit
