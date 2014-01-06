# run with cdfSim
# Example of setup  FAKE_EVENT(1 event)  
# + mbr min bias( poisson distributed with mean=1 )
# Substitute FAKE_EVENT with any other generator. 
# Simulation here is disabled.  
#
# When verbose set true for generator module,
# output histogram file contains the distribution of the number of 
# generated primary vertices 

path enable AllPath
creator set NSIM

module input GenInputManager
module enable GenPrimVert

module enable FAKE_EVENT
talk FAKE_EVENT
    use PT
    use ETA
    use PHI
# SET PARAMETER_NAME MEAN SIGMA PMIN PMAX POWER MODE(1=gauss,2=flat)
    generate PT      0. 0.   4.    50.  -2.  2
    generate ETA     0. 0.  -1.0   1.0   0.  2
    generate PHI     0. 0.   0.0   360.  0.  2
    generate CDFCODE  11
    generate NPARTICLES 1
   show
exit

mod enable mbr
talk mbr
  verbose set true
  MEAN set 1
  MODE set POISSON
exit

module enable HepRootManager
mod talk HepRootManager
  verbose set true
  histfile set test.root
  createHistoFile set  true
exit

mod disable SimInitManager
mod disable SimulationControlMod

mod output FileOutput
talk FileOutput
  output create main_stream isajet_qq_today.root
  output path main_stream AllPath
  output keepList main_stream \
                    LRIH_StorableBank EVCL_StorableBank HEPG_StorableBank \
                    MVTX_StorableBank \
                    TL2D_StorableBank OBSP_StorableBank OBSV_StorableBank 
exit

begin -nev 100
show timer

exit




