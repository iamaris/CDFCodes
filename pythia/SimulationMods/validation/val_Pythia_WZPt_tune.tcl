#
# Willis Sakumoto Tune for Z Pt from Run I
# must use CTEQ5L
#
talk Pythia
  PythiaMenu
   commonMenu
   #Gaussian Kt:
     set_mstp -index=91 -value=1
   #kt sigma:
     set_parp -index=91 -value=2.10
   #kt max:
     set_parp -index=93 -value=15.0
   #Q^{2}_{min}:
     set_parp -index=62 -value=1.25
   # K factor of evolution scale, Kt^2 for alphas and PDFs
     set_parp -index=64 -value=0.2
   exit
  exit
exit
