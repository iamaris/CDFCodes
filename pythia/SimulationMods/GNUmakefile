# SUBDIRS deactivated pending actually having something to compile.
SUBDIRS = src SimInitManager InteractiveGeant test python OverlapDetection

simpletest:
	( cd test; $(MAKE) simpletest; )

overlap:
	$(MAKE) -C test/overlap $(OVERRIDES) tbin

include SoftRelTools/standard.mk
