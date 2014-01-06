################################################################################
# Makefile for generatorMods/examples/wbbgen area
#
################################################################################
# file lists (standard names, local contents)

# binary products
BINS =
TBINS = wbbgen_test 
COMPLEXBIN =
BINCCFILES = wbbgen_test.cc

SIMPLEBINS = $(BINS) $(TBINS)

# local library definitions for executable link
override LINK_generatorMods += wbbgen_i
override LINK_FrameMods     += wbbgen_i
override LINK_wbbgen_i      += wbbgen_i
include PackageList/link_all.mk

############################################################
include SoftRelTools/standard.mk
include SoftRelTools/binclean.mk
