# Makefile for herwig_i package
#
# Author Elena Gerchtein ( CMU )
# created Jan 17 2002
#
#############################################################

# include file products
INC = 

COMPLEXBIN = hepevt2hepg

# binary products
TBINS = $(COMPLEXBIN)

BINCCFILES = Hepevt2HepgModule.cc  hepevt2hepg.cc 
BINFFILES  = $(wildcard *.F)

override LOADLIBES += -lgeneratorMods
override LOADLIBES += $(JIMMY_DIR)/lib/libjimmy.a

override  LINK_Framework          += generatorMods/hepevt2hepg
override  LINK_FrameMods          += generatorMods/hepevt2hepg
override  LINK_FrameMods_root     += generatorMods/hepevt2hepg   
override  LINK_generatorMods      += generatorMods/hepevt2hepg
override  LINK_Hepevt             += generatorMods/hepevt2hepg	
override  LINK_Fmcfio             += generatorMods/hepevt2hepg
override  LINK_GenTrig            += generatorMods/hepevt2hepg

override LINK_MinBias           += generatorMods/hepevt2hepg
override LINK_Bgenerator        += generatorMods/hepevt2hepg
override LINK_EvtGen            += generatorMods/hepevt2hepg
override LINK_ParticleDB        += generatorMods/hepevt2hepg
override LINK_cern_i            += generatorMods/hepevt2hepg
override LINK_evt               += generatorMods/hepevt2hepg
override LINK_herwig_i          += generatorMods/hepevt2hepg
override LINK_isajet_i          += generatorMods/hepevt2hepg
override LINK_qq_i              += generatorMods/hepevt2hepg
override LINK_pythia_i          += generatorMods/hepevt2hepg
#override LINK_grappa            += generatorMods/hepevt2hepg
override LINK_pythia_services   += generatorMods/hepevt2hepg
override LINK_vecbos_i          += generatorMods/hepevt2hepg
override LINK_stdhep_i          += generatorMods/hepevt2hepg
override LINK_stdhep            += generatorMods/hepevt2hepg
override LINK_wgrad_i           += generatorMods/hepevt2hepg
override LINK_wbbgen_i          += generatorMods/hepevt2hepg
override LINK_Edm               += generatorMods/hepevt2hepg
override LINK_SimulationObjects += generatorMods/hepevt2hepg
override LINK_BankTools         += generatorMods/hepevt2hepg
override LINK_Hepevt            += generatorMods/hepevt2hepg
override LINK_r_n               += generatorMods/hepevt2hepg
override LINK_tauola_i          += generatorMods/hepevt2hepg
override LINK_HeavyQuarkGen     += generatorMods/hepevt2hepg
override LINK_pythia_services   += generatorMods/hepevt2hepg
override LINK_Alignment         += generatorMods/hepevt2hepg
override LINK_mcfm              += generatorMods/hepevt2hepg


###############################################################################
include PackageList/link_all.mk
include SoftRelTools/standard.mk
include SoftRelTools/binclean.mk
include SoftRelTools/arch_spec_root.mk
include SoftRelTools/arch_spec_stdhep.mk
include SoftRelTools/refresh.mk


override BINLIBS:= $(filter-out $(LUND_DIR)/lib/pyr.o,  $(BINLIBS))
override BINLIBS:= $(filter-out -lgrappa,  $(BINLIBS))
