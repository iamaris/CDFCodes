path enable AllPath
creator set NSIM

module enable HepRootManager
mod talk HepRootManager
  verbose set true
  histfile set run_sim_test.hist
  createHistoFile set  f
exit

module input GenInputManager

mod enable Isajet
set env(CDFPVLI) $env(GENERATORMODS_DIR)/examples/isajet/top175.val

talk GeometryManager
# Use the defaults but substitute the detailed for the simple Si detector
# The following set up is to be added to the default GeometryManager set up.
  DetectorMenu
     enableTof set true
     enablePassive set true
                                # central part of the beampipe
     enableBeampipeC set true   
     show
  exit
# TOF geometry model
  TofGeometryMenu
                                # options: Nominal, Naive
    GeometryModel set Survey   
  exit
  SiliconGeometryMenu
    BuildPassive set true
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
  add CSXCounter   MuonDigiCSX MuonCSXdata
  add Tof3Pack TofDigi3Pack TofGroup
  add TofBar TofDigiBar   TofGroup
  add CalorDetectorElement CalorDigiGeneric CalorGroup 
  add BFCoil CalorDigiBFCoil CalorGroup
  add NoBFCoil CalorDigiNoBFCoil CalorGroup
  add BMUGas  ImuDigiBMUGas    ImuGroup
  add BSUPaddle  ImuDigiBSUPaddle ImuGroup
  add TSUPaddle  ImuDigiTSUPaddle ImuGroup
  show add
  ConfigMenus
    CotGroup_CotSuperLayer
      DriftModel set Garfield
    exit
  exit
exit

mod output FileOutput
mod disable FileOutput
talk FileOutput
  output create main_stream isajet_10Klong.root
  output path main_stream AllPath
  output keepList main_stream \
                    LRIH_StorableBank EVCL_StorableBank HEPG_StorableBank \
                    TL2D_StorableBank OBSP_StorableBank OBSV_StorableBank \
                   COTD_StorableBank CMPD_StorableBank CMUD_StorableBank \
                   CSPD_StorableBank CMXD_StorableBank\
                    SIXD_StorableBank MSVX_StorableBank ISLD_StorableBank \
                    MISL_StorableBank MCOT_StorableBank TOFD_StorableBank \
                    COTQ COTM_StorableObject \
                    BMUD_StorableBank BSUD_StorableBank TSUD_StorableBank\
                    PESD_StorableBank CESD_StorableBank CPRD_StorableBank \
                    CEMD_StorableBank PEMD_StorableBank CHAD_StorableBank \
                    WHAD_StorableBank PHAD_StorableBank PPRD_StorableBank \
		    CSXD_StorableBank\
                    MuonXHitColl MuonTHitColl MuonXZHitColl \
		    TOFD_StorableBank TofPulse TofPulseColl TofMatch TofMatchColl
exit
module enable HepRootManager
mod talk HepRootManager
  verbose set true
  histfile set run_sim_test.hist
  createHistoFile set  f
exit

begin -nev 10000
show timer

exit




