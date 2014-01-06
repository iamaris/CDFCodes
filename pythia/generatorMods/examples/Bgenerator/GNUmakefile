################################################################################
# Makefile for generatorMods/examples/Bgenerator area
#
# uses SoftRelTools/standard.mk
#
#  revision history:
#  -----------------
# *0001 Mar 06 2001 Ch.Paus: created
################################################################################
# file lists (standard names, local contents)

COMPLEXBIN = Bgenerator.exe

TBINS = $(COMPLEXBIN)

override LOADLIBES += -lgeneratorMods -lBgenerator
override LINK_DHMods            += generatorMods/Bgenerator
override LINK_FrameMods         += generatorMods/Bgenerator
override LINK_FrameMods_root    += generatorMods/Bgenerator
override LINK_Framework         += generatorMods/Bgenerator
override LINK_Banks             += generatorMods/Bgenerator
override LINK_SimulationObjects += generatorMods/Bgenerator
override LINK_StorableBanks     += generatorMods/Bgenerator
override LINK_gntMods           += generatorMods/Bgenerator
override LINK_generatorMods     += generatorMods/Bgenerator

include PackageList/link_all.mk

BINCCFILES = Bgenerator_build.cc

include SoftRelTools/standard.mk
include SoftRelTools/binclean.mk
include SoftRelTools/arch_spec_root.mk

###############################################################################
