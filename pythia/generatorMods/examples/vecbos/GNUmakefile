################################################################################
# Makefile for generatorMods/examples/vecbos area
#
################################################################################
# file lists (standard names, local contents)

COMPLEXBIN = Vecbos

TBINS = $(COMPLEXBIN)

override LOADLIBES += -lgeneratorMods -lvecbos_i  -lvecbos_callbacks -lvecbos
override LINK_DHMods            += generatorMods/vecbos
override LINK_FrameMods         += generatorMods/vecbos
override LINK_FrameMods_root    += generatorMods/vecbos
override LINK_Framework         += generatorMods/vecbos
override LINK_Banks             += generatorMods/vecbos
override LINK_SimulationObjects += generatorMods/vecbos
override LINK_StorableBanks     += generatorMods/vecbos
override LINK_generatorMods     += generatorMods/vecbos

include PackageList/link_all.mk

BINCCFILES = vecbos_build.cc

include SoftRelTools/standard.mk
include SoftRelTools/binclean.mk
include SoftRelTools/arch_spec_root.mk

###############################################################################
