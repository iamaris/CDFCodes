# -----------------------------------------------------------------------------
#
# p + pbar -> ~chi20 + ~chi1+ (Pythia subprocess 230)
#
# -----------------------------------------------------------------------------
module enable Pythia
talk Pythia
   PythiaMenu

#     decayFileMode set "read"
#     decayFile set  "susyhit_slha.out"
#     decayFile set  "softsusy.spc"
#     readDecayTableMode set 3

     msel set 0
     cmEnergy set 1960
#     inListLevel set 0
#     evListLevel set 1

# * Main Process selection for the generation {D=1}
# MSEL set 0 : desired subprocesses have to be swithced on in MSUB,
#	       i.e full user control.
# MSEL set 1 : depending on incoming particles, different alternatives are used.
# -----------------------------------------------------------------------------
# Classes of processes accessible through the parameter MSEL
# and the individual processes codes ISUB
# -----------------------------------------------------------------------------
#	39 : All MSSM processes except Higgs production
# !	40 : Squark and gluino producton(ISUB=243,244,258,259,271~280) -> SUGRA
#	41 : Stop pair production(ISUB=261~265)
#	42 : Slepton pair production(ISUB=201-214)
# !!	43 : Squark or gluino with chargino or neutralion(ISUB=237~242,246~256)
# !!!	44 : Chargino-neutralino pair production(ISUB=216~236)
# -----------------------------------------------------------------------------

# ******************** SUBMENU ********************
commonMenu
#set_msub -index=216 -value=1
set_msub -index=230 -value=1
# * Set MSUB parameters
#   If no input, then the others are set to zero as default. -> 'show_msub'
#   Default -> MSUB(ISUB)=0 : the subprocess is excluded [ 1 : included ]
show_msub
exit

# ********** 1. SUSY Parameters
susyMenu
# * For example, SUGRA
# -------------------------------------------------------------------------
# set_imss -index=1 -value=2
# set_imss -index=2 -value=1 (0 if no forced relation between gaugino par.)
# set_imss -index=11 -value=0 (1 if allow NLSP decay to gravitinos )
# -------------------------------------------------------------------------
# RMSS(1)  => M1/2 :		- common gaugino mass		(MHALF or m1/2)
# RMSS(4)  => MU   : sgn(mu)<0	- higgsino mass par. 		(mu)
# RMSS(5)  => TANB : 		- ratio of Higgs expectation value (tanBeta)
# RMSS(8)  => M0   :		- common scalar mass		(m0) 
# RMSS(16) => AT   : => A0??	- top trilinear coupling
# RMSS(19) => MA   :		- pseudoscalar Higgs mass par.
# PMSS(6)  => MTOP :		- top quark mass		(m_top)
# -------------------------------------------------------------------------
# CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
#  USING APPROXIMATE SUGRA RELATIONS 
#  M0 =   800.
#  M1/2=  80.
#  TANB=  2.5
#  MU =   ??	<- decided by AT
#  AT =   ?	(AT=MU/TANB, AB=MU*TANB)
#  MA =   ??	<- by AT
#  MTOP=  175.
# CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
#  ALLOWING DECAYS TO GRAVITINOS

set_imss -index=1 -value=11
set_imss -index=21 -value=37
set_imss -index=22 -value=37
#set_imss -index=1 -value=2
	# * level of MSSM simulation
	### D=0 : no MSSM sim
	### 1   : general MSSM sim
	### 2   : approximation SUGRA sim	
set_imss -index=2 -value=0
	# * treatment of U(1), SU(2), and SU(3) gaugino mass parameters ;
	### D=0 : M1, M2 and M3 are set by RMSS(1), RMSS(2), and RMSS(3),
	###       i.e there is no forced relation between them
	### 1	: 3/5(M1/alpha1)=(M2/alpha2)=(M3/alpha3)=X and RMSS(1)
	###	  if IMSS(1)=2 and RMSS(20)=alphaGUT, then X=MHALF/alphaGUT
	### 2	: M1 by RMSS(1), M2 by RMSS(2) and M3=(alpha3/alpha2)M2
	### 3	: X like as 1 and RMSS(2)
set_imss -index=3 -value=0
	# * treatment of gluino mass parameter, M3 ;
	### D=0 : M3 is used to calculate the gluino pole mass with some formulae
	### 1   : M3 is the gluino pole mass
set_imss -index=4 -value=1
	# * treatment of Higgs sector
	### 0	: Higgs sector is determined by the approximate formulae and
	###	  pseudoscalar mass MA set by RMSS(19)
	### D=1	: determined by the exact formulae and ... MA ...
	###	 [the pole mass for MA is not the same as the input parameter]
	### 2   : fixed by the mixing angle alpha set by RMSS(18)
	###	  and the mass value PMAS(I,1) where I=25,35,36,and 37
set_imss -index=5 -value=0
	# * treatment of sbottom and stop sqark masses {D=0}
set_imss -index=7 -value=0
	# * treatment of the scalar masses in extension of SUGRA models.
	#   The presence of additional U(1) symmetries at high energy scales
	#   can modify the boundary conditions for the scalar masses at the
	#   unification scale.
	### D=0	: no additional D-terms are included. In SUGRA models,
	###	  all scalars have the mass m0 at the unification scale.
set_imss -index=8 -value=1
	# * treatment of ~tau mass eigenstates, ~tau_1, ~tau_2
	### D=1	: ~tau mass eigenstates are identical to interaction eigenstates,
	###	  so they are treated identically to ~e and ~mu
#set_imss -index=9 -value=0
	# * treatment of squark_R mass eigenstates for the first two generations
#set_imss -index=10 -value=1
	# * allowed decays for ~N2
	### D=0 : decay with BR calculated from MSSM
	### 1   : forced decay only to ~N1 + gamma
set_imss -index=11 -value=1
	# * choice of the lightest superpartner(LSP)
	### D=0 : ~N1 is LSP
	### 1   : ~N1 is NLSP

#set_rmss -index=1 -value=124.75
	# * {D=80.} [GeV] : 100. taken from the ATRAS TDR page 818
	# ; if imss(1)=1, M1, the U(1) gaugino mass
	# ; if imss(1)=2, the common gaugino mass, m1/2
#set_rmss -index=2 -value=245.01
	# * M2, the SU(2) gaugino mass {D=160.}
#set_rmss -index=3 -value=697.41
	# * M3, the SU(3) (gluino) mass parmeter {D=500.}	
#set_rmss -index=4 -value=514.88
	# * mu, the higgsino mass parameter {D=800.}
	# ; if imss(1)=2, only the sign is considered. -> sgn(mu)
	#   -> But, MU=614.297328 and 616.867405
set_rmss -index=5 -value=2.
	# * tanBeta, ratio of Higgs expectation values {D=2.00}
#set_rmss -index=6 -value=250.
	# * mass(slepton_L) {D=250.}
	#   [m(sneutrion) is fixed by a sum rule]
#set_rmss -index=7 -value=200.
	# * m(slepton_R) {D=200.}
#set_rmss -index=8 -value=200.
	# * m(squrak_L) {D=800.} 
	# ; if imss(1)=2, the common scalar mass m0
#set_rmss -index=9 -value=700.
	# * m(squark_R) {D=700.}
#set_rmss -index=10 -value=800.
	# * m(squark_L) for 3rd gen {D=800.}
#set_rmss -index=11 -value=700.
	# * m(sbottom_R) {D=700.}
#set_rmss -index=12 -value=500.
	# * m(stop_R) {D=500.}
	# ; when imss(5)=1, this is the lightest stop mass.	
#set_rmss -index=13 -value=250.
	# * m(stau_L) {D=250.} 
	# ; if imss(7)=1, m(stau) is same as m(selectron) & m(smuon) 
	#   -> m(slepton)?
#set_rmss -index=14 -value=200.
	# * m(stau_R) {D=200}
#set_rmss -index=15 -value=800.
	# * bottom trilinear coupling AB {D=800.}
#set_rmss -index=16 -value=0.
	# * top trilinear coupling AT {D=400.}
	# ; if imss(1)=2, the common trilinear coupling A0
	#   -> But, AT= -177.708081 in RMSS(16)=400 and = -222.253365 in 0]
#set_rmss -index=17 -value=0.
	# * tau trilinear coupling Atau {D=0.} -> no exist. why?
#set_rmss -index=18 -value=0.100
	# * Higgs mixing angle {D=0.100}
	#   [This is only used when all of the Higgs parameters are
	#    set by user, i.e imss(4)=2]	
#set_rmss -index=19 -value=652.19
	# * pseudoscalar Higgs mass parameter MA {D=850.}
	#   CDF Preliminary : SUGRA MA=500 [GeV/c^2]
	#   -> But, MA=1088.03955 and 1089.71409
#set_rmss -index=20 -value=0.410
	# * GUT scale coupling constant alphaGUT {D=0.410}
#set_rmss -index=21 -value=1.00
	# * gravitino mass {D=1.00} [eV]
#set_rmss -index=29 -value=2.4e+18
	# * Plank mass used for calculating decays to light gravitinos
	### {D=2.4*10^18 i.e 2.4e+18} [GeV]
show_imss
show_rmss
exit

# ********** 2. Switches for Event Type and Kinematics Selection
commonMenu
# Set mass cuts
# ckin(1)-(2) : this is the range of allowed parton center mass E;
# the first is the minimum value we want to have for the second quark
# x(=bjorken scaling variable=fraction of proton mom carried by the quark),
# if the first has the max value(in case of Tevatron sqrt(s)=2TeV,so x1=1;
# so we want to produce gluinos with mass 300 GeV,then we want the
# minimum to be right below 300 to allow gluinos but not all the other
# processes at too low energies; if ckin(2) is negative then there is no
# upper limit
	set_ckin -index=1 -value=65.
	set_ckin -index=2 -value=-1.
	# {D=2.,-1. [GeV]} range of allowed parton center of mass E;
	# ; if CKIN(2)<0, the upper limit is inactive.
# Set pt range
	set_ckin -index=3  -value=5.
	set_ckin -index=4  -value=750.
	# {D=0.,-1. [GeV]} range of allowed Pt values for hard 2->2 processes,
	# with transverse momentum Pt defined in the rest frame of the hard
	# interaction ; if CKIN(4)<0, the upperlimit is inactive.
# this below set the rapidity(y) range of center of mass of interacting partons
# which for massless particles(as quarks/gluons) is equally to eta 
	set_ckin -index=7  -value=-7.
	set_ckin -index=8  -value=7.
# this below set the rapidity(y) range of the  interacting partons
# which for massless particles(as quarks/gluons) is equally to eta 
	set_ckin -index=7  -value=-4.
	set_ckin -index=8  -value=4.
# this below set the rapidity range of the largest rapidity final state range
	set_ckin -index=9  -value=-10.
	set_ckin -index=10 -value=10.
# this below set the rapidity range of the smallest rapidity final state particle
	set_ckin -index=11  -value=-10.
	set_ckin -index=12 -value=10.
# Set eta range
	set_ckin -index=13 -value=-5.
	set_ckin -index=14 -value=5.
	set_ckin -index=15 -value=-5.
	set_ckin -index=16 -value=5.

# ********** 3. The General Swithes and Parameters
# below is the max number of fermion generations
	set_mstp -index=1 -value=1
	# {D=3} maximum number of generations. Automatically set=<4.
# below is coupling constants stuff
	set_mstp -index=2 -value=2
	# {D=1} calculation of alpha_s at hard interaction, in the routine ULALPS
	# 0 : alpha_s is fixed at value PARU(111)
	# 1 : first-order running alpha_s
	# 2 : second-order running alpha_s
#
	##set_mstp -index=3 -value=.920E+05
	# {D=2} selection of LAMBDA value in alpha_s for MSTP(2)>=1.
#
	##set_mstp -index=4 -value=0
	# {D=0} treatment of the Higgs sector, predominantly the neutral one.
	# 0 : H^0 is given SM Higgs couplings,
	#     while H^'0 and A^0 couplings shoud be set by user in
	#     PARU(171~175) and PARU(181~185), respectively.
	# 1 : user should set couplings for all three Higgses,
	#     for H^0 in PARU(161~165), and for H^'0 and A^0 as above.
	# 2 : mass of H^0 in PMAS(25,1) and tanB value in PARU(141) are used
	#     to derive H^'0, A^0 and H+- couplings, using relations fo MSSM
	#     extension of SM at Born level. Existing masses and couplings are
	#     overwritten by derived values.
	# 3 : as =2, but using relations at one-loop level.
	#     This option is not yet implemented.

# Decay stuff for top, also ISR/FSR
# decay t->bW+, and then W fragments
	set_mstp -index=48 -value=2
# structure functions {D=9,1}
	set_mstp -index=51 -value=10
	set_mstp -index=52 -value=1
# Initial/final state radiation {D=1,1}
	set_mstp -index=61 -value=1
	set_mstp -index=71 -value=1
# Tuned underlying event parameters {D=0,1}
	set_mstp -index=33 -value=2
	set_mstp -index=82 -value=2

# correct underlying event parameters for Pythia 6.129 from Stan Thompson(Nov 5, 1999)
	set_parp -index=81 -value=1.4
	set_parp -index=82 -value=1.55
	set_parp -index=90 -value=0.

# to be asked _this sets the fragmentation value
	set_mstj -index=11 -value=3
# this below has been changed from 1 to 2 because it was inconsistent with mstp(48)
	set_mstj -index=27 -value=2

# we don't know this, so we leave what was here {D=?}
	#set_parj -index=55 -value=-0.006

# ** set_pmas	: Set PMAS parameters. Requires -masscode=KC with at least one keyword
#		  from mass, width and maxdev. These correspond to PMAS(c,1),
#		  PMAS(c,2) and PMAS(c,3) respectively.
# set_pmas command takes just one index ?
	set_pmas -masscode=6 -mass=175.
	set_pmas -masscode=5000039 -mass=1.4 -width=0.01 -maxdev=0.01
# -----------------------------------------------------------------------------
# chi2 decays 
# want leptonic only -- take out taus for now...
# there are no sneutrino/slepton decays here
# (unnecessary due to susy parameters chosen) so
# be careful with arbitrary changes in susy parameters!
# note -- update for Pythia 6.129 -- thanx to Pasha -- MDME numbers change!!!!
#
# two leptons decay only
# #   1000003    304    ~s_L            ~s_Lbar            -1    1    1    500.00000     1.00000    10.00000   0.00000E+00    1
#           1734    1   53    0.000000    ~gravitino      s                                                               
#           1735    1   53    0.000000
# -----------------------------------------------------------------------------

# set_mdme -channelIndex=idc [-decayType={-1,5} (MDME(n,1))] [-matrix=n (MDME(n,2))]
# set_brat -channelIndex=idc -brRatio={0.,1.} 
# : Set branching ratios for decay table entries
	set_mdme -channelIndex=1912 -decayType=1
	set_brat -channelIndex=1912 -brRatio=1.00
	set_mdme -channelIndex=1913 -decayType=1
	set_mdme -channelIndex=1914 -decayType=1
	set_mdme -channelIndex=1915 -decayType=1
	set_mdme -channelIndex=1916 -decayType=1
	set_mdme -channelIndex=1917 -decayType=-1
	set_mdme -channelIndex=1918 -decayType=-1
	##set_mdme -channelIndex=1919 -decayType=1
	##set_mdme -channelIndex=1920 -decayType=1
	##set_mdme -channelIndex=1924 -decayType=1
	##set_mdme -channelIndex=1925 -decayType=1
	##set_mdme -channelIndex=2019 -decayType=1
	##set_mdme -channelIndex=2020 -decayType=1
# -----------------------------------------------------------------------------	
#        KF     KC    particle        antiparticle      chg  col  anti        mass       width       w-cut     lifetime decay
#           IDC on/off ME   Br.rat.    decay products
#
#   1000022    315    ~chi_10                             0    0    0    500.00000     1.00000    10.00000   0.00000E+00    1
#          1912    1   53    0.000000    ~gravitino      gamma                                                           
#          1913    1   53    0.000000    ~gravitino      Z0                                                              
#          1914    1   53    0.000000    ~gravitino      h0                                                              
#          1915    1   53    0.000000    ~gravitino      H0                                                              
#          1916    1   53    0.000000    ~gravitino      A0                                                              
#          1917   -1   53    0.000000    c               dbar            e-                                              
#          1918   -1   53    0.000000    d               sbar            nu_e                          
#                 
#   1000023    316    ~chi_20                             0    0    0    500.00000     1.00000    10.00000   0.00000E+00    1
#          1919    1   53    0.000000    ~gravitino      gamma                                                           
#          1920    1   53    0.000000    ~gravitino      Z0                                                              
#          1921    1   53    0.000000    ~gravitino      h0                                                              
#          1922    1   53    0.000000    ~gravitino      H0                                                              
#          1923    1   53    0.000000    ~gravitino      A0                                                              
#          1924    1   53    0.000000    ~chi_10         gamma                                                           
#          1925    1   53    0.000000    ~chi_10         Z0                                                              
#                                           
#          2019    1   53    0.000000    ~g              u               ubar                                            
#          2020    1   53    0.000000    ~g              c               cbar
# -----------------------------------------------------------------------------

# set_kfdp -channelIndex=idc -products=kf1,kf2,...
# : Set KF codes of decay products for decay table entries
# -----------------------------------------------------------------------------
# For example, set_kfdp -channelIndex=1912 -products=1000039,22,0,0 ; Default
# Kfdp(1912) = 1000039  22  0  0  0
# : dp(decay products) for "1912 decay table entry[IDC 1912]" : ~gravitino gamma
#  -. Kf of ~gravition is 1000039
#  -. Kf of gamma      is 22 
#        KF     KC    particle        antiparticle      chg  col  anti        mass       width       w-cut     lifetime decay
#           IDC on/off ME   Br.rat.    decay products                                                          
#
#        22     22    gamma                               0    0    0      0.00000     0.00000     0.00000   0.00000E+00    0
#           162    1  102    0.000000    d               dbar                                                            
#           163    1  102    0.000000    u               ubar                                                            
#           164    1  102    0.000000    s               sbar                                                            
#           165    1  102    0.000000    c               cbar                                                            
#           166    1  102    0.000000    b               bbar                                                            
#           167    1  102    0.000000    t               tbar                                                            
#           168   -1  102    0.000000    b'              b'bar                                                           
#           169   -1  102    0.000000    t'              t'bar                                                           
#           170    1  102    0.000000    e-              e+                                                              
#           171    1  102    0.000000    mu-             mu+                                                             
#           172    1  102    0.000000    tau-            tau+                                                            
#           173   -1  102    0.000000    tau'-           tau'+
#
#   1000039    321    ~gravitino                          0    0    0    500.00000     0.00000     0.00001   0.00000E+00    0
# -----------------------------------------------------------------------------
#   5000039    347    Graviton*                           0    0    0   1000.00000     0.14159     1.41588   0.00000E+00    1
#          4158    1   32    0.063273    d               dbar                                                            
#          4159    1   32    0.063273    u               ubar                                                            
#          4160    1   32    0.063273    s               sbar                                                            
#          4161    1   32    0.063272    c               cbar                                                            
#          4162    1   32    0.063268    b               bbar                                                            
#          4163    1   32    0.056681    t               tbar                                                            
#          4164   -1   32    0.000000    b'              b'bar                                                           
#          4165   -1   32    0.000000    t'              t'bar                                                           
#          4166    1    0    0.020486    e-              e+                                                              
#          4167    1    0    0.020486    nu_e            nu_ebar                                                         
#          4168    1    0    0.020486    mu-             mu+                                                             
#          4169    1    0    0.020486    nu_mu           nu_mubar                                                        
#          4170    1    0    0.020486    tau-            tau+                                                            
#          4171    1    0    0.020486    nu_tau          nu_taubar                                                       
#          4172   -1    0    0.000000    tau'-           tau'+                                                           
#          4173   -1    0    0.000000    nu'_tau         nu'_taubar                                                      
#          4174    1    0    0.327780    g               g                                                               
#          4175    1    0    0.040972    gamma           gamma                                                           
#          4176    1    0    0.045217    Z0              Z0                                                              
#          4177    1    0    0.090073    W+              W-             
	set_mdme -channelIndex=4158 -decayType=-1
	set_mdme -channelIndex=4159 -decayType=-1
	set_mdme -channelIndex=4160 -decayType=-1
	set_mdme -channelIndex=4161 -decayType=-1
	set_mdme -channelIndex=4162 -decayType=-1
	set_mdme -channelIndex=4163 -decayType=-1
	set_mdme -channelIndex=4164 -decayType=-1
	set_mdme -channelIndex=4165 -decayType=-1
	set_mdme -channelIndex=4166 -decayType=1
	set_brat -channelIndex=4166 -brRatio=0.50
	set_mdme -channelIndex=4167 -decayType=-1
	set_mdme -channelIndex=4168 -decayType=1
	set_brat -channelIndex=4168 -brRatio=0.50
	set_mdme -channelIndex=4169 -decayType=-1
	set_mdme -channelIndex=4170 -decayType=-1
	set_mdme -channelIndex=4171 -decayType=-1
	set_mdme -channelIndex=4172 -decayType=-1
	set_mdme -channelIndex=4173 -decayType=-1
	set_mdme -channelIndex=4174 -decayType=-1
	set_mdme -channelIndex=4175 -decayType=-1
	set_mdme -channelIndex=4176 -decayType=-1
	set_mdme -channelIndex=4177 -decayType=-1
show_msub
exit
exit

exit

