# Makefile for generatorMods/examples/isajet area
#
# uses SoftRelTools/standard.mk
#
#  revision history:
#  -----------------
# *0001 Jan 22 1998 P.Murat: add arch_spec_Python.mk
# *0002 Sep 30 1998 M.Shapiro:  add Mixing example to TBINS
# *0003 Jun 04 1999 P.Murat: general cleanup (tbin build is broken in many
#                            places)
###############################################################################
# file lists (standard names, local contents)

# include file products
INC = 

# library product
LIB = 

# subdirectories
SUBDIRS =

# binary products
BINS  = isajet_test.exe

override LINK_generatorMods += generatorMods
override LINK_FrameMods     += generatorMods
override LINK_isajet_i      += generatorMods
override LINK_qq_i          += generatorMods
override LINK_cern_i        += generatorMods

include PackageList/link_all.mk
############################################################
ifdef debug
  DEBUG := $(shell echo LIBDEPS = $(LIBDEPS) >& 2)
endif

#wdir     = $(SRT_TOP)/tmp/$(BFARCH)/generatorMods/isajet/
#dirlist += $(wdir)

COMPLEXBIN = isajet_test.exe
BINCCFILES = isajet_test.cc
BINFFILES  =

# include RootUtils/link.mk
include SoftRelTools/standard.mk
############################################################
