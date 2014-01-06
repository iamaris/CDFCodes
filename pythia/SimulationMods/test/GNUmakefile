SUBDIRS = 

simpletest:
	( cd simple; $(MAKE) simpletest; )

TBINS      = cdfIG
BINS       = cdfSim
COMPLEXBIN = cdfSim

#TBINS =
#BINS  = cdfIntegrate

SIMPLEBINS = $(TBINS)

#
#  kludge
#
BINCCFILES = cdfSim.cc
BINFFILES  = 

override LOADLIBES += -lGenTrig
override LOADLIBES += -lgeneratorMods
override LOADLIBES += $(JIMMY_DIR)/lib/libjimmy.a

override LINK_Alignment               += SimulationMods/test
override LINK_Bgenerator    	      += SimulationMods/test
override LINK_CalorGeometry           += SimulationMods/test
override LINK_CalorSim  	      += SimulationMods/test
override LINK_ClcSim                  += SimulationMods/test
override LINK_CotGeometry             += SimulationMods/test
override LINK_CotSim 	              += SimulationMods/test
override LINK_EvtGen                  += SimulationMods/test
override LINK_ExoticMods              += SimulationMods/test
override LINK_Fmcfio                  += SimulationMods/test
override LINK_FrameMods               += SimulationMods/test
override LINK_FrameMods_root          += SimulationMods/test
override LINK_Framework               += SimulationMods/test
override LINK_GenTrig	 	      += SimulationMods/test
override LINK_GflashSim               += SimulationMods/test
override LINK_HeavyQuarkGen           += SimulationMods/test
override LINK_Hepevt                  += SimulationMods/test
override LINK_InteractiveGeant 	      += SimulationMods/test
override LINK_L2GlobalTrigger         += SimulationMods/test 
override LINK_LesHouches              += SimulationMods/test
override LINK_Level3Mods              += SimulationMods/test
override LINK_MuonSim 	              += SimulationMods/test
override LINK_PADSMods                += SimulationMods/test
override LINK_QuickSim_Reconstruction += SimulationMods/test
override LINK_QuickSim_Simulation     += SimulationMods/test
override LINK_QuickSim_Objects        += SimulationMods/test
override LINK_QuickSim_Geometry       += SimulationMods/test
override LINK_SemiLeptonicB           += SimulationMods/test
override LINK_SimInitManager 	      += SimulationMods/test
override LINK_SimulationExample       += SimulationMods/test
override LINK_SimulationMods          += SimulationMods/test
override LINK_SimulationUtils         += SimulationMods/test
override LINK_SvxSim 		      += SimulationMods/test
override LINK_TofAlgs	              += SimulationMods/test
override LINK_TofCalibDBTables        += SimulationMods/test
override LINK_TofObjects              += SimulationMods/test
override LINK_TofSim 	              += SimulationMods/test
override LINK_TrackingMods	      += SimulationMods/test
override LINK_XTRPSim                 += SimulationMods/test 
override LINK_generatorMods           += SimulationMods/test
override LINK_svtsim                  += SimulationMods/test 
override LINK_tauola_i                += SimulationMods/test
override LINK_vecbos_i                += SimulationMods/test
override LINK_wwbgen_i                += SimulationMods/test
				   
test: $(foreach i, $(TBINS), TEST_$(i))

include PackageList/link_all.mk
override LOADLIBES += -lHepgObjects

include SoftRelTools/standard.mk
include SoftRelTools/binclean.mk
include SoftRelTools/component_test.mk

override BINLIBS:= $(filter-out -lpythia_services,  $(BINLIBS))
override BINLIBS:= $(filter-out $(LUND_DIR)/lib/pyr.o,  $(BINLIBS))
override BINLIBS:= $(filter-out -lgrappa,  $(BINLIBS))
