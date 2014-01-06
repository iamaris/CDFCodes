##########################################################################
module enable  HeavyQuarkGen
module talk    HeavyQuarkGen
  HeavyQuarkGen
    Verbose   set  1
    # Define Generic b hadron production
    Define      -idhep=5  -name=b    -mass=4.75 -life=0.0           \
                -spec_file=val_HeavyQuarkGen_mrst_2000_4750_mu_total.root \
                -spec_name=dsigma_dydpt
#                -spec_file=$env(CDFDAB_DIR)/dab/mrst_2000_4750_mu_total.root \
    # Setup the mixture
    SetBMixture -mixu=0.389 -mixd=0.389 -mixs=0.107 -mixc=0.007 -mixl=0.116
    # Setup the fragmentation parameter
    SetBEps     -epsu=0.006 -epsd=0.006 -epss=0.006 -epsc=0.006 -epsl=0.006
    # Specify the particles to generate
    Generate    -idhep=5  -rapmin=-2.0 -rapmax=2.0 -ptmin=0.2
  exit
  show
exit

#set env(QQ_USER_FILE) "decayuser.CDF"
mod enable QQModule
mod talk QQModule
  Decay_B_Baryons set 1
  Decay_Bc set 1
  show
exit

##########################################################################
