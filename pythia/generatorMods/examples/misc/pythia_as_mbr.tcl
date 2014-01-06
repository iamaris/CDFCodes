# run with cdfSim
# Example of setup  FAKE_EVENT(1 event)  + pythia min bias( 1 event)
#  as tuned by Rick Field
# Substitute FAKE_EVENT with any other generator except PythiaModule.
# Simulation here is disabled.  

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

mod enable PythiaMinBias
# Pythia talk-to fragment
mod talk PythiaMinBias
  PythiaMenu
     msel set 0
      inListLevel set 0
      listLast set 5
      commonMenu
 # minimum bias processes
 #             set_msub -index=91 -value=1  elastic scattering
              set_msub -index=92 -value=1
              set_msub -index=93 -value=1
              set_msub -index=94 -value=1
              set_msub -index=95 -value=1
 # Parameters due to Rick Field (Tune B)
              set_mstp -index=81 -value=1
              set_mstp -index=82 -value=4
              set_parp -index=82 -value=1.6
 #             set_parp -index=82 -value=2.0 tune A
              set_parp -index=83 -value=0.5
              set_parp -index=85 -value=1.0
 #             set_parp -index=85 -value=0.9 tune A
              set_parp -index=86 -value=1.0
 #             set_parp -index=86 -value=0.95 tune A
              set_parp -index=89 -value=1.8
              set_parp -index=90 -value=0.25
              set_parp -index=67 -value=1.0
 #             set_parp -index=67 -value=4.0 tune A
 # CTEQ5L pdf, this is the default in pythia6.203
              set_mstp -index=51 -value=7
      exit
    exit
   exit
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




