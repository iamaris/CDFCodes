# Makefile for herwig_i package
#
# Author R. Harris, 18-Dec-1996
#
#############################################################
# file lists (standard names, local contents)
PACKAGE = generatorMods

# include file products
INC = 

# library product

ifdef LINK_SHARED
  SHAREDLIB = lib$(PACKAGE).so
  ifdef TEST_LINK
    include $(PACKAGE)/link_$(PACKAGE).mk
    include PackageList/link_all.mk
  endif
else
  LIB       = lib$(PACKAGE).a
endif

#library contents
#
#  Jan 12 1999 P.Murat: Herprt needs to be cleaned up and its dependency 
#  on UIPACK has to be removed
#
#skip_files := HerprtModule.cc 

# Not ready until MCFM is safely in development :
skip_files := GrappaModule.cc GrappaModule_initializeTalkTo.cc 

LIBFFILES  =   $(wildcard *.F)
LIBCFILES  =   $(wildcard *.c)
LIBCCFILES =   $(filter-out $(skip_files), $(wildcard *.cc))

# subdirectories
SUBDIRS =

# binary products
BINS = 

###############################################################################
include SoftRelTools/standard.mk
include SoftRelTools/arch_spec_stdhep.mk
include SoftRelTools/arch_spec_root.mk
include SoftRelTools/refresh.mk
###############################################################################
#isajet_test:
#	$(MAKE) $(OVERRIDES) -C examples/isajet bin

#                                  by default don't put anything on the link
#                                  list when linking the shared libraries
ifndef TEST_LINK
  override LOADLIBES := 
endif
