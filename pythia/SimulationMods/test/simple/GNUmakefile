#
# This sets up the tests in run, in order of simplicity. Uncomment only one
# of the following lines.
#
#TBINS = testVolumeNamePrinter
#TBINS = testVolumeNamePrinter testIntegrationData 
#TBINS = testVolumeNamePrinter testIntegrationData testIntegrationLeg
TBINS = testVolumeNamePrinter testIntegrationData testIntegrationLeg testPathIntegral

BINS  = 
SIMPLEBINS = $(TBINS) $(BINS)

override LOADLIBES += -lSimulationMods

override LINK_geant_i        += SimulationMods/test/simple

simpletest: $(foreach i, $(TBINS), TEST_$(i))

include PackageList/link_all.mk
include SoftRelTools/standard.mk
include SoftRelTools/binclean.mk
include SoftRelTools/component_test.mk

