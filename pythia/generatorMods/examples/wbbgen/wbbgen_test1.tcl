## wbbgen_test.tcl    njets=3  04-Oct-2001
## NOTE: for now only choice for ndns is 101 [which gives pdf set cteq5m];
##       MUST execute script generatorMods/examples/wbbgen/wbbgen_setup (or
##     similar) to get soft link to cteq5m.tbl .
##     But, if you do want another pdf, see wbbgen_i/doc/wbbgen_cdf.txt.
## Following takes ~ 20 mins. to run, produces ~10 (temp) files
#############################
#show
#help
path enable AllPath
#
module input GenInputManager
## Following gets 1 line on screen per 1 event (at hadronization stage)
talk GenInputManager
 report set 1
exit
#
## set herwig random number here; wbbgen random number in talk Wbbgen below
talk RandomGenManager
 RandomNumberMenu
   RandomSeedHERWIG1 set 1111110
   RandomSeedHERWIG2 set 1111119
   show
 exit
exit


mod enable Wbbgen
mod enable QQModule

talk Wbbgen
# help
#  show
#
## Following 5 lines lead to a print (internal to herwig) of maxpr events,
##   1 line per entity (so ~500 lines for ttbar), each line has width
##   ~120 characters. If use   prvtx set 0   then VHEP info is not printed
##    and width is ~80.
##   Default is no event printing (maxpr = 0).
 Prints
   maxpr set 2
   prvtx set 0
 exit
#
## default Clpow leads to no B-Baryons
 Hadronization
   Clpow set 1.3
 exit
#
## Here is the explicitly wbbgen talkto
 Wbbuser
#   help
   ibeam2 set 1
   ebeam set 980.
   bmass set 4.75
   njets set 3
   ndns set 101
   ptjmin set 15
   ptbmin set 15
   drjmin set 0.0
   drbmin set 0.01
   ptlmin set 0.1
   etalmax set 9.9
   drlmin set 0.01
   ptnmin set 0.1
   nopt set 200000
   niter set 1
   maxev set 200000
   seed1 set 1111118
   seed2 set 2222226
   gridr set 0
   gridw set 41
   wbbfile set wbbfile1
 exit
#
 show
exit
###
# note default is to NOT call qq, but BEWARE if you do not use qq
talk QQModule
  Decay_Bc set 1
  Decay_Upsilon set 1
  Decay_B_Baryons set 1
  Decay_prompt_charm set 1
#  show
exit
 
###
path list
#
## To make an output file (wbbgen_test.out), uncomment the following 7 lines:
mod output FileOutput
module talk FileOutput
  output create myout wbbgen_test.out
  output enable myout
  output path myout AllPath
  output keepList myout EVCL_StorableBank LRIH_StorableBank HEPG_StorableBank
  output list
exit
###
## Following gets 10 events; change 10 to 1000 for 1000 events, etc.
## But Note: max. no. of events = maxev*effic, which may be < nev set here
begin -nev  10
show timer
exit
#########################

