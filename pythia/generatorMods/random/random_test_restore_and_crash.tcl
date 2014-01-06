# test setup for cdfGen

path enable AllPath

mod talk RandomGenManager
  RestoreRandomStreams set true
  InputFileName set EvilInput.dat
  OutputFileName set SomeOutput.dat
exit  

begin -nev 10
show timer

exit






