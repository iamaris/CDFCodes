1) class RandomTestModule
   shows how CdfRn interface should be used in the module.

   Each module using CdfRn should have
   TalkTo's ( RandomMenu, 2 random seed setters in this case ) and
   default random seeds, two seeds for each stream.

   For user convenience this structure is kept the same for all modules
   that need to set up seeds for random number generator.
   Since RandomMenu belongs to the module the developmer has a freedom to 
   reserve as many random streams as needed within RandomMenu.    

  
2) This is a test of how the job can be restarted
   after abortion at the same event.
   This job is aborted when some condition depending on
   random number generated is applied. 

To build random_test:

addpkg -h generatorMods
gmake generatorMods.tbin

To run random_test ( tested on fcdflnx1 ):

random_test generatorMods/random/random_test_crash.tcl

this should crash at some point before 10th event
and CdfRnStat.dat file be dumped.

Restore this state and crash at the same (1th) event:

cp CdfRnStat.dat EvilInput.dat
random_test generatorMods/random/random_test_restore_and_crash.tcl

