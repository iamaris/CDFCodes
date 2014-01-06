SUBDIRS = 

TBINS = g3minesweeper
BINS  = 

SIMPLEBINS = $(TBINS) $(BINS)

override LOADLIBES += -lSimulationMods
override LOADLIBES += -lOverlapDetection
override LOADLIBES += -lExtrapolator -lG3XExtrapolator
override LOADLIBES += -lgeneratorMods

override LINK_Framework         += SimulationMods/test
override LINK_FrameMods         += SimulationMods/test
override LINK_FrameMods_root    += SimulationMods/test
override LINK_SimInitManager 	+= SimulationMods/test
override LINK_SvxSim 		+= SimulationMods/test
override LINK_CotSim 	        += SimulationMods/test
override LINK_CotGeometry       += SimulationMods/test
override LINK_TofSim 	        += SimulationMods/test
override LINK_TofObjects        += SimulationMods/test
override LINK_TofCalibDBTables  += SimulationMods/test
override LINK_MuonSim 	        += SimulationMods/test
override LINK_CalorSim  	+= SimulationMods/test
override LINK_ClcSim            += SimulationMods/test
override LINK_EvtGen            += SimulationMods/test
override LINK_Hepevt            += SimulationMods/test
override LINK_SimulationUtils   += SimulationMods/test
override LINK_GflashSim         += SimulationMods/test
override LINK_CalorGeometry     += SimulationMods/test
override LINK_Alignment         += SimulationMods/test
override LINK_generatorMods     += SimulationMods/test
override LINK_Extrapolator      += SimulationMods/test
override LINK_SimulationMods 	+= SimulationMods/test

include PackageList/link_all.mk
include SoftRelTools/standard.mk
include SoftRelTools/binclean.mk
include SoftRelTools/component_test.mk

override BINLIBS:= $(filter-out $(LUND_DIR)/lib/pyr.o,  $(BINLIBS))
