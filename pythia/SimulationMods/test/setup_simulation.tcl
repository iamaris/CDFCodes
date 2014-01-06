###########################################################################
### If you want to make changes to the recommended settings, change 
### the parameters below. The regular user should not need to do this.
###########################################################################
# current (5.3.4) defaults for the TCL variables
# see cdf-7258 for COT_HIT_RESOLUTION_SCALE and BEAM_SIGMA_T0
# beam sigma_z and sigma_t0 are used unconditionally, the rest parameters
# of the beam are turned on only if BEAM_SET_BY_HAND != 0
###########################################################################
set BEAM_SET_BY_HAND   [ getenv BEAM_SET_BY_HAND    0       ]
###########################################################################
### Default simulation is set to generate realistic MC with misalignment 
### and beamline from DB after setting a run number in run_cdfSim.tcl. 
### If you don't want this set REALISTIC_MC to 0 and comment the 
### following lines 
set REALISTIC_MC 1

############################################################################
# For realistic and ideal MC talk to calibration manager
source $env(CDFSOFT2_DIR)/Production/setup_calibration.tcl

###### Specify alignment table
source $SIM_TCL_DIR/setup_alignment_table.tcl

###### Turn on misalignment for L00 (is also the default)
set L00_Alignment true
###########################################################################
### Set Silicon Charge Deposition Model by choosing one of the following
set SILICON_CDM PARAMETERIZED; set PARA_CDM_SET 1
#set SILICON_CDM GEOMETRIC; set PARA_CDM_SET 0
#set SILICON_CDM PHYSICAL; set PARA_CDM_SET 0

###########################################################################
### To turn off creation of PhantomLayer used for material tuning
### set PHANTOM_LAYER to 0 (current best guess of tuning)
set PHANTOM_LAYER 1

###########################################################################
### Beamline business
###########################################################################
### We turn on the option of generating the z-dependence of the beamwidth 
### according to beta* function but use a Gaussian z-vertex distribution.
### If you don't want to do that, modify both parameters below 
set BEAM_BetaStarBeamWidth true
set BEAM_BetaStarZVertex   true
###########################################################################
### Beamline and primary vertex parameters can also be set by hand,
### set BEAM_SET_BY_HAND to 1 and uncomment the following settings 
###########################################################################
### Here a few settings that are sometimes used
###########################################################################
### Set variable SI_PASSIVE to false 
### to turn off simulation of Si passive material
set SI_PASSIVE true
### Keep information necessary for COT and SI track MC parentage matching
set COT_Matching [ getenv COT_MATCHING  0 ]
set SI_Matching  [ getenv SI_MATCHING   0 ]
### Set B_FIELD to 0.0 to turn off simulation of magnetic field
set B_FIELD 14.116
### Set to true to enable creation of PropagatedSiParticleColl
set SI_PROP_PART false
############################################################################
# Here the talk-to business starts
# The regular user usually does not need to make modifications below
############################################################################

path enable AllPath
creator set NSIM

#Tof simulation
module enable TofManager
############################################################################
# Beamline parameters are set in GenPrimVert module
module enable GenPrimVert
talk GenPrimVert
  sigma_z  set  [ getenv BEAM_SIGMA_Z       28.0     ]
  sigma_t  set  [ getenv BEAM_SIGMA_T0       1.3     ]

  if {$REALISTIC_MC} then { BeamlineFromDB set true } 

# Set beamline parameters by hand
  if {$BEAM_SET_BY_HAND} then {
     BeamlineFromDB set false
# Set beam spread in x-y [cm] (defaults are 0.0):
     sigma_x        set  [ getenv BEAM_SIGMA_X        0.00257 ]
     sigma_y        set  [ getenv BEAM_SIGMA_Y        0.00258 ]
# Set beam position and slope (defaults are 0.0):
     pv_central_x   set  [ getenv BEAM_PV_CENTRAL_X   0.064   ]
     pv_central_y   set  [ getenv BEAM_PV_CENTRAL_Y   0.310   ]
     pv_central_z   set  [ getenv BEAM_PV_CENTRAL_Z   2.5     ]
     pv_slope_dxdz  set  [ getenv BEAM_PV_SLOPE_DXDZ -0.00021 ]
     pv_slope_dydz  set  [ getenv BEAM_PV_SLOPE_DYDZ  0.00031 ]
  }
  UseBetaStarBeamWidth set $BEAM_BetaStarBeamWidth
  UseBetaStarZVertex   set $BEAM_BetaStarZVertex 
  show
exit

############################################################################
# S.Behari 05/20/2010> SiClusteringModule setting specific to B group.
#                      According to SD'Auria this is needed before trigger
#                      simulation to create the "CORRECTED" silicon SIXD bank,
#                      which is used by svtsim.
if { $DFC_BOOK == "cdfpbot" } {

module enable SiClusteringModule

# Configure clustering
#
  talk SiClusteringModule
   DebugMenu
      PrintInput     set f
      PrintOutput    set f
      PrintNClusters set f
   exit
     NewFramework set f
     RegionalMode set t
     MonitorMode set f
     parmSetName set QUIET
     CorrectClusterCentroids set true
# Centroid correction set for the same settings as in simulation
     CentroidCorrectionModel set $SILICON_CDM
     UseNNDataUnpacker set false
#
# This is needed by the realistic MC
     InputFromBanks set t
     OutputDescr set "CORRECTED"
     OutputBanks set t
     WriteStripBanks set t 
#
     StripCorrectorMenu
        FlagNonIntLadders set true
        SubtractPedestal set false
        UseL00PedFit set false
     exit
     ExtL00Param
       onlyGoodStrips    set f
       maxClusterLen     set 32
       maxQtotal         set 9999
       maxQstrip         set 80
       maxNoisePeakStrip set 10.0
       minDistToNextClus set -1
     exit
     show
# to be commented soon
     verbose set t
  exit

}

############################################################################
# For storing UserInfoColl("MCInfo")
if {$STORE_USERINFO} then {
  module enable MCInfoModule
}
############################################################################
talk GeometryManager
# TOF geometry model now set to Aligned (old default was Survey)
  TofGeometryMenu
     # options: Nominal, Naive
     GeometryModel set Aligned
  exit
# Misalignment in MC set in SiliconGeometryMenu
  SiliconGeometryMenu
     if {$REALISTIC_MC} then {
         AlignmentPrint set 3
         AlignmentSource set "$ALIGN_TABLE"
# Turn off L00 misalignment to avoid volume overlaps with beampipe
         L00Alignment set $L00_Alignment
     }
# Simulation of Si passive material
     BuildPassive set $SI_PASSIVE
# Disable SVX inner carbon screen as passive material as it does not exist 
# and SVXII bulkhead taps
     DisabledPassiveElements set svxInnScreen svxBHTap
# Turn on creation of PhantomLayer used for material studies
     if {$PHANTOM_LAYER} then {
         CreatePhantomLayer set true
         PhantomLayerRmin set 14.8 14.8 14.8 14.8 14.8 20.5 20.5 20.8 20.5 20.5
         PhantomLayerZmin set -60 -45 -15 15.1 45.1 -100 -45 -15 15.1 45.1
         PhantomLayerZmax set -45.1 -15.1 15 45 60 -45.1 -15.1 15 45 100
         PhantomLayerThickness set 0.9 0.9 0.2 0.9 0.9 0.4 0.15 0.1 0.15 0.4
         PhantomLayerMaterial set SVX_BIAS_CABLE SVX_BIAS_CABLE SVX_BIAS_CABLE \
                                  SVX_BIAS_CABLE SVX_BIAS_CABLE SVX_BIAS_CABLE \
                                  SVX_BIAS_CABLE SVX_BIAS_CABLE SVX_BIAS_CABLE \
                                  SVX_BIAS_CABLE
         PhantomLayerContainer set SVCC SVCC SVCC SVCC SVCC ISLC ISLC ISLC ISLC ISLC 
     } 
  exit
# Setting of B field
  Bfield set $B_FIELD 
  show
exit

############################################################################
talk SimInitManager
  DetectorMenu
    declareCPR       set f
    show
  exit
# Misalignment in MC applied
  if {$REALISTIC_MC} then {
     applyAlignment set t
  }
show
exit

############################################################################
talk SimulationControlMod
  fastTrack           set [ getenv CDFSIM_FAST_TRACK   false ]
  DebugMenu
    showActiveVolumes set [ getenv SHOW_ACTIVE_VOLUMES false ]
    DebugLevel        set [ getenv CDFSIM_DEBUG_LEVEL  0     ]
  exit
  DetectorMenu
    simulateSvx     set t
    simulateCot     set t
    simulateMuon    set t
    simulateTof     set t
    simulateCalor   set t
    simulatePassive set t
    show
  exit
  dumpFactory
  add CdfHalfLadder         SvxDigitizer      SvxGroup
  add CotSuperLayer         CotDigitizer      CotGroup
  add CMUExtrusion          MuonDigiCMU       MuonCMUdata
  add CMPPart               MuonDigiCMP       MuonCMPdata
  add CMXChamber            MuonDigiCMX       MuonCMXdata
  add CSXCounter            MuonDigiCSX       MuonCSXdata
  add Tof3Pack              TofDigi3Pack      TofGroup
  add TofBar                TofDigiBar        TofGroup
  add CalorDetectorElement  CalorDigiGeneric  CalorGroup 
  add BFCoil                CalorDigiBFCoil   CalorGroup
  add NoBFCoil              CalorDigiNoBFCoil CalorGroup
  add BMUGas                ImuDigiBMUGas     ImuGroup
  add BSUPaddle             ImuDigiBSUPaddle  ImuGroup
  add TSUPaddle             ImuDigiTSUPaddle  ImuGroup
  show add
  ConfigMenus
    CotGroup_CotSuperLayer
      DriftModel         set [ getenv COT_DRIFT_MODEL           Garfield ]
      HitResolutionScale set [ getenv COT_HIT_RESOLUTION_SCALE  0.64     ]
      show
    exit
    SvxGroup_CdfHalfLadder
        if { $RUN_NUMBER > 1 } then {
          useNoiseDB set t
        }
# Set Silicon Charge Deposition Model 
       pick_svx_cdm set $SILICON_CDM
# Talk-to settings for parametric CDM 
       if {$PARA_CDM_SET} then {     
          svx_cdm_noise set ON
	  svx_cdm_crosstalk_list set 2  0.    0.    0.    0.43  0.2  0.47 0.19  \
                                        0.49  0.51  0.41  0.24  0.46 0.50       \
                                        0.355 0.355 0.355 0.355
#          svx_validation set ON
       }
# Control creation of PropagatedSiParticleColl
       CreatePropagatedSi set $SI_PROP_PART 
# Turns on wafer-level misalignments
  if {$REALISTIC_MC} then {
     AlignmentAlignWafers set t
     svx_cdm_noise set THRESH7 
  }
       show
    exit
# S.Behari 05/20/2010> CMX coverage setting for B group
    if { $DFC_BOOK == "cdfpbot" } {
      MuonCMXdata_CMXChamber
        if  {$RUN_NUMBER < 175008} then {
            disableMiniskirts set true
            disableKeystone set true
        } else {
            disableMiniskirts set false
            disableKeystone set false
        }
      exit
    }
    MuonCMPdata_CMPPart
    if  {$RUN_NUMBER < 154449} then {
	disableBluebeam set true
    } else {
	disableBluebeam set false
    }
      show
    exit
# setting for Tof 
    TofGroup_TofBar
        simModel set DETAILED
    exit
    TofGroup_Tof3Pack
        simModel set DETAILED
    exit
  exit
exit

