skip_files := 

ifdef LINK_SHARED
  SHAREDLIB = libSimulationMods.so
else
  LIB = libSimulationMods.a
endif

LIBCCFILES = $(filter-out $(skip_files), $(wildcard *.cc))

BINS =

include SoftRelTools/arch_spec_root_minedm.mk
include SoftRelTools/standard.mk
include SoftRelTools/refresh.mk

