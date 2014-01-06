SUBDIRS = 

TBINS = $(COMPLEXBIN)

COMPLEXBIN = random_test

override LOADLIBES += -lgeneratorMods
# while it is not in PackageList
override LOADLIBES += -lgeneratorMods -lwbbgen_i

BINCCFILES = RandomTestModule.cc random_test.cc 

override LINK_r_n               += generatorMods/test
override LINK_Framework         += generatorMods/test
override LINK_FrameMods         += generatorMods/test
override LINK_FrameMods_root    += generatorMods/test
override LINK_Bgenerator    	+= generatorMods/test
override LINK_GenTrig	 	+= generatorMods/test
override LINK_generatorMods 	+= generatorMods/test
override LINK_CotGeometry       += generatorMods/test
override LINK_EvtGen            += generatorMods/test
override LINK_Hepevt            += generatorMods/test
override LINK_SimulationUtils   += generatorMods/test
override LINK_CalorGeometry     += generatorMods/test
override LINK_tauola_i          += generatorMods/test
#override LINK_wbbgen_i          +=generatorMods/test
override LINK_vecbos_i          +=generatorMods/test
override LINK_pythia_services   +=generatorMods/test

include PackageList/link_all.mk
include SoftRelTools/standard.mk
include SoftRelTools/binclean.mk
include SoftRelTools/component_test.mk


override BINLIBS:= $(filter-out -lEvtGen,   $(BINLIBS))