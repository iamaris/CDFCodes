#------------------------------------------------------------------------------
# this may or may not be necessary depending on whether
# or not -i and -o switches were used on the command line
# however it doesn't hurt to do this here either way...
#------------------------------------------------------------------------------
path disable AllPath
#------------------------------------------------------------------------------
# Define the CDFSIM path:
#------------------------------------------------------------------------------
#set CDFSIM_RUNNING_MODE  [ getenv CDFSIM_RUNNING_MODE "GENERATE_AND_SIMULATE" ]
#if { $CDFSIM_RUNNING_MODE == "GENERATE_ONLY" } then {

set RUNMC_FLAG [ getenv RUNMC_FLAG "GENERATE_AND_SIMULATE" ]

if { $RUNMC_FLAG == "GENERATE_ONLY" } then {
path create cdfSimPath  ManagerSequence      \
                        RandomGenManager     \
                        HardScatGenSequence  \
			DecayPackageSequence \
			GenOutputManager     \
                        GenTrigSequence
} else {
#-----------------------------------------------------------------------
#  default mode: both generation and simulation are required
#-----------------------------------------------------------------------
path create cdfSimPath \
			ManagerSequence      \
                        RandomGenManager     \
                        HardScatGenSequence  \
                        DecayPackageSequence \
                        GenOutputManager     \
                                             \
                        TofManager           \
                        HepRootManager       \
				             \
                        MinBiasSequence      \
                        GenPrimVert          \
                        GenTrigSequence      \
                        PythiaMinBias        \
                                             \
                        SimInitManager       \
			SimulationControlMod \
                                             \
                        SimValModule         \
			SimpleCotTrkReco     \
			SimpleSiTrkReco      \
                        SiClusteringModule   \
                                             \
                        CotqModule           \
                        SvxqModule
}
path enable cdfSimPath
#-----------------------------------------------------------------------
