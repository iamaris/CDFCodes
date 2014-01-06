SUBDIRS = 

BINS  = cdfGen
COMPLEXBIN = cdfGen

override LOADLIBES += -lGenTrig
override LOADLIBES += -lgeneratorMods
override LOADLIBES += $(JIMMY_DIR)/lib/libjimmy.a

BINCCFILES =  cdfGen.cc

override LINK_r_n               += generatorMods/test
override LINK_Framework         += generatorMods/test
override LINK_FrameMods         += generatorMods/test
override LINK_FrameMods_root    += generatorMods/test
override LINK_Bgenerator    	+= generatorMods/test
override LINK_HeavyQuarkGen 	+= generatorMods/test
override LINK_ExoticMods	+= generatorMods/test
override LINK_GenTrig	 	+= generatorMods/test
override LINK_HepgMods	 	+= generatorMods/test
override LINK_generatorMods 	+= generatorMods/test
override LINK_L2GlobalTrigger   += generatorMods/test 
override LINK_CotGeometry       += generatorMods/test
override LINK_Hepevt            += generatorMods/test
override LINK_SemiLeptonicB     += generatorMods/test
override LINK_SimulationUtils   += generatorMods/test
override LINK_CalorGeometry     += generatorMods/test
override LINK_tauola_i          += generatorMods/test
override LINK_vecbos_i          +=generatorMods/test
override LINK_LesHouches        +=generatorMods/test
override LINK_grappa            +=generatorMods/test
override LINK_Fmcfio            +=generatorMods/test

include PackageList/link_all.mk
include SoftRelTools/standard.mk
include SoftRelTools/binclean.mk
include SoftRelTools/component_test.mk

override BINLIBS:= $(filter-out -lpythia_services,  $(BINLIBS))
override BINLIBS:= $(filter-out $(LUND_DIR)/lib/pyr.o,  $(BINLIBS))
override BINLIBS:= $(filter-out -lgrappa,  $(BINLIBS))
