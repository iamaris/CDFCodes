##########################################################################
# enable generator ( no generators are enabled by default )
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
##########################################################################
