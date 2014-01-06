ifdef LINK_SHARED
  SHAREDLIB = libSimInitManager.so

else
  LIB = libSimInitManager.a
endif

skip_files =
LIBCCFILES = $(filter-out $(skip_files),$(wildcard *.cc))

include SoftRelTools/standard.mk
include SoftRelTools/refresh.mk
