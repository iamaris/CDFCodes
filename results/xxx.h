//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Sep 22 18:18:01 2011 by ROOT version 4.00/08
// from TTree Photon/My Dark Photon Ntuples
// found on file: TDarkPhoton_Ntuple_1.root
//////////////////////////////////////////////////////////

#ifndef xxx_h
#define xxx_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

class xxx {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leave types
   Int_t           ph_Npho;
   Float_t         ph_E[3];   //[ph_Npho]
   Float_t         ph_Et[3];   //[ph_Npho]
   Float_t         ph_Etc[3];   //[ph_Npho]
   Float_t         ph_Cesx[3];   //[ph_Npho]
   Float_t         ph_Cesz[3];   //[ph_Npho]
   Float_t         ph_Deta[3];   //[ph_Npho]
   Float_t         ph_Eta[3];   //[ph_Npho]
   Float_t         ph_Phi[3];   //[ph_Npho]
   Float_t         ph_SinTheta[3];   //[ph_Npho]
   Int_t           ph_N3d[3];   //[ph_Npho]
   Float_t         ph_Chi2[3];   //[ph_Npho]
   Float_t         ph_NIsoTrk[3];   //[ph_Npho]
   Float_t         ph_Pt[3];   //[ph_Npho]
   Float_t         ph_Tiso[3];   //[ph_Npho]
   Int_t           e_NElectrons;
   Float_t         e_TotalEt[8];   //[e_NElectrons]
   Float_t         e_Et[8];   //[e_NElectrons]
   Float_t         e_E[8];   //[e_NElectrons]
   Float_t         e_TrackPt[8];   //[e_NElectrons]
   Float_t         e_Nasl[8];   //[e_NElectrons]
   Float_t         e_Nssl[8];   //[e_NElectrons]
   Float_t         e_HadEm[8];   //[e_NElectrons]
   Float_t         e_Iso[8];   //[e_NElectrons]
   Float_t         e_Lshr[8];   //[e_NElectrons]
   Float_t         e_EP[8];   //[e_NElectrons]
   Float_t         e_DelXQ[8];   //[e_NElectrons]
   Float_t         e_DelZ[8];   //[e_NElectrons]
   Float_t         e_Z0[8];   //[e_NElectrons]
   Float_t         e_Chi2Strip[8];   //[e_NElectrons]
   Int_t           e_DetectorCode;
   Int_t           e_TrackNumber;
   Float_t         e_ClusterEmEt[8];   //[e_NElectrons]
   Float_t         e_ClusterHadEt[8];   //[e_NElectrons]
   Float_t         e_Charge[8];   //[e_NElectrons]
   Float_t         e_TrackEta[8];   //[e_NElectrons]
   Float_t         e_TrackPhi[8];   //[e_NElectrons]
   Float_t         e_TrackPhi2[8];   //[e_NElectrons]
   Float_t         e_Px[8];   //[e_NElectrons]
   Float_t         e_Py[8];   //[e_NElectrons]
   Float_t         e_Pz[8];   //[e_NElectrons]
   Float_t         e_P0[8];   //[e_NElectrons]
   Int_t           eb_NPositrons;
   Float_t         eb_TotalEt[12];   //[eb_NPositrons]
   Float_t         eb_Et[12];   //[eb_NPositrons]
   Float_t         eb_E[12];   //[eb_NPositrons]
   Float_t         eb_TrackPt[12];   //[eb_NPositrons]
   Float_t         eb_Nasl[12];   //[eb_NPositrons]
   Float_t         eb_Nssl[12];   //[eb_NPositrons]
   Float_t         eb_HadEm[12];   //[eb_NPositrons]
   Float_t         eb_Iso[12];   //[eb_NPositrons]
   Float_t         eb_Lshr[12];   //[eb_NPositrons]
   Float_t         eb_EP[12];   //[eb_NPositrons]
   Float_t         eb_DelXQ[12];   //[eb_NPositrons]
   Float_t         eb_DelZ[12];   //[eb_NPositrons]
   Float_t         eb_Z0[12];   //[eb_NPositrons]
   Float_t         eb_Chi2Strip[12];   //[eb_NPositrons]
   Int_t           eb_DetectorCode;
   Int_t           eb_TrackNumber;
   Float_t         eb_ClusterEmEt[12];   //[eb_NPositrons]
   Float_t         eb_ClusterHadEt[12];   //[eb_NPositrons]
   Float_t         eb_Charge[12];   //[eb_NPositrons]
   Float_t         eb_TrackEta[12];   //[eb_NPositrons]
   Float_t         eb_TrackPhi[12];   //[eb_NPositrons]
   Float_t         eb_TrackPhi2[12];   //[eb_NPositrons]
   Float_t         eb_Px[12];   //[eb_NPositrons]
   Float_t         eb_Py[12];   //[eb_NPositrons]
   Float_t         eb_Pz[12];   //[eb_NPositrons]
   Float_t         eb_P0[12];   //[eb_NPositrons]
   Int_t           m_NMuons;
   Int_t           m_TrackNumber[12];   //[m_NMuons]
   Int_t           m_NCmuHits[12];   //[m_NMuons]
   Int_t           m_NCmpHits[12];   //[m_NMuons]
   Int_t           m_NCmxHits[12];   //[m_NMuons]
   Int_t           m_NBmuHits[12];   //[m_NMuons]
   Float_t         m_TrackPt[12];   //[m_NMuons]
   Float_t         m_Iso[12];   //[m_NMuons]
   Float_t         m_CmuDelX[12];   //[m_NMuons]
   Float_t         m_CmpDelX[12];   //[m_NMuons]
   Float_t         m_CmxDelX[12];   //[m_NMuons]
   Float_t         m_BmuDelX[12];   //[m_NMuons]
   Float_t         m_HasCmuStub[12];   //[m_NMuons]
   Float_t         m_HasCmpStub[12];   //[m_NMuons]
   Float_t         m_HasCmxStub[12];   //[m_NMuons]
   Float_t         m_HasBmuStub[12];   //[m_NMuons]
   Float_t         m_TrackBcPt[12];   //[m_NMuons]
   Float_t         m_EmEnergy[12];   //[m_NMuons]
   Float_t         m_HadEnergy[12];   //[m_NMuons]
   Float_t         m_Z0[12];   //[m_NMuons]
   Float_t         m_D0[12];   //[m_NMuons]
   Float_t         m_mupt[12];   //[m_NMuons]
   Int_t           m_NSvxHits[12];   //[m_NMuons]
   Float_t         m_d0Corr[12];   //[m_NMuons]
   Float_t         m_Charge[12];   //[m_NMuons]
   Float_t         m_TrackEta[12];   //[m_NMuons]
   Float_t         m_TrackPhi[12];   //[m_NMuons]
   Float_t         m_TrackPhi2[12];   //[m_NMuons]
   Float_t         m_Px[12];   //[m_NMuons]
   Float_t         m_Py[12];   //[m_NMuons]
   Float_t         m_Pz[12];   //[m_NMuons]
   Float_t         m_P0[12];   //[m_NMuons]
   Int_t           mb_NAMuons;
   Int_t           mb_TrackNumber[13];   //[mb_NAMuons]
   Int_t           mb_NCmuHits[13];   //[mb_NAMuons]
   Int_t           mb_NCmpHits[13];   //[mb_NAMuons]
   Int_t           mb_NCmxHits[13];   //[mb_NAMuons]
   Int_t           mb_NBmuHits[13];   //[mb_NAMuons]
   Float_t         mb_TrackPt[13];   //[mb_NAMuons]
   Float_t         mb_Iso[13];   //[mb_NAMuons]
   Float_t         mb_CmuDelX[13];   //[mb_NAMuons]
   Float_t         mb_CmpDelX[13];   //[mb_NAMuons]
   Float_t         mb_CmxDelX[13];   //[mb_NAMuons]
   Float_t         mb_BmuDelX[13];   //[mb_NAMuons]
   Float_t         mb_HasCmuStub[13];   //[mb_NAMuons]
   Float_t         mb_HasCmpStub[13];   //[mb_NAMuons]
   Float_t         mb_HasCmxStub[13];   //[mb_NAMuons]
   Float_t         mb_HasBmuStub[13];   //[mb_NAMuons]
   Float_t         mb_TrackBcPt[13];   //[mb_NAMuons]
   Float_t         mb_EmEnergy[13];   //[mb_NAMuons]
   Float_t         mb_HadEnergy[13];   //[mb_NAMuons]
   Float_t         mb_Z0[13];   //[mb_NAMuons]
   Float_t         mb_D0[13];   //[mb_NAMuons]
   Float_t         mb_mupt[12];   //[m_NMuons]
   Int_t           mb_NSvxHits[12];   //[m_NMuons]
   Float_t         mb_d0Corr[12];   //[m_NMuons]
   Float_t         mb_Charge[13];   //[mb_NAMuons]
   Float_t         mb_TrackEta[13];   //[mb_NAMuons]
   Float_t         mb_TrackPhi[13];   //[mb_NAMuons]
   Float_t         mb_TrackPhi2[13];   //[mb_NAMuons]
   Float_t         mb_Px[13];   //[mb_NAMuons]
   Float_t         mb_Py[13];   //[mb_NAMuons]
   Float_t         mb_Pz[13];   //[mb_NAMuons]
   Float_t         mb_P0[13];   //[mb_NAMuons]
   Float_t         mi_Met;
   Float_t         mi_MetPhi;
   Float_t         mi_Ht;
   Float_t         mi_HtCorr;

   // List of branches
   TBranch        *b_ph_Npho;   //!
   TBranch        *b_ph_E;   //!
   TBranch        *b_ph_Et;   //!
   TBranch        *b_ph_Etc;   //!
   TBranch        *b_ph_Cesx;   //!
   TBranch        *b_ph_Cesz;   //!
   TBranch        *b_ph_Deta;   //!
   TBranch        *b_ph_Eta;   //!
   TBranch        *b_ph_Phi;   //!
   TBranch        *b_ph_SinTheta;   //!
   TBranch        *b_ph_N3d;   //!
   TBranch        *b_ph_Chi2;   //!
   TBranch        *b_ph_NIsoTrk;   //!
   TBranch        *b_ph_Pt;   //!
   TBranch        *b_ph_Tiso;   //!
   TBranch        *b_e_NElectrons;   //!
   TBranch        *b_e_TotalEt;   //!
   TBranch        *b_e_Et;   //!
   TBranch        *b_e_E;   //!
   TBranch        *b_e_TrackPt;   //!
   TBranch        *b_e_Nasl;   //!
   TBranch        *b_e_Nssl;   //!
   TBranch        *b_e_HadEm;   //!
   TBranch        *b_e_Iso;   //!
   TBranch        *b_e_Lshr;   //!
   TBranch        *b_e_EP;   //!
   TBranch        *b_e_DelXQ;   //!
   TBranch        *b_e_DelZ;   //!
   TBranch        *b_e_Z0;   //!
   TBranch        *b_e_Chi2Strip;   //!
   TBranch        *b_e_DetectorCode;   //!
   TBranch        *b_e_TrackNumber;   //!
   TBranch        *b_e_ClusterEmEt;   //!
   TBranch        *b_e_ClusterHadEt;   //!
   TBranch        *b_e_Charge;   //!
   TBranch        *b_e_TrackEta;   //!
   TBranch        *b_e_TrackPhi;   //!
   TBranch        *b_e_TrackPhi2;   //!
   TBranch        *b_e_Px;   //!
   TBranch        *b_e_Py;   //!
   TBranch        *b_e_Pz;   //!
   TBranch        *b_e_P0;   //!
   TBranch        *b_eb_NPositrons;   //!
   TBranch        *b_eb_TotalEt;   //!
   TBranch        *b_eb_Et;   //!
   TBranch        *b_eb_E;   //!
   TBranch        *b_eb_TrackPt;   //!
   TBranch        *b_eb_Nasl;   //!
   TBranch        *b_eb_Nssl;   //!
   TBranch        *b_eb_HadEm;   //!
   TBranch        *b_eb_Iso;   //!
   TBranch        *b_eb_Lshr;   //!
   TBranch        *b_eb_EP;   //!
   TBranch        *b_eb_DelXQ;   //!
   TBranch        *b_eb_DelZ;   //!
   TBranch        *b_eb_Z0;   //!
   TBranch        *b_eb_Chi2Strip;   //!
   TBranch        *b_eb_DetectorCode;   //!
   TBranch        *b_eb_TrackNumber;   //!
   TBranch        *b_eb_ClusterEmEt;   //!
   TBranch        *b_eb_ClusterHadEt;   //!
   TBranch        *b_eb_Charge;   //!
   TBranch        *b_eb_TrackEta;   //!
   TBranch        *b_eb_TrackPhi;   //!
   TBranch        *b_eb_TrackPhi2;   //!
   TBranch        *b_eb_Px;   //!
   TBranch        *b_eb_Py;   //!
   TBranch        *b_eb_Pz;   //!
   TBranch        *b_eb_P0;   //!
   TBranch        *b_m_NMuons;   //!
   TBranch        *b_m_TrackNumber;   //!
   TBranch        *b_m_NCmuHits;   //!
   TBranch        *b_m_NCmpHits;   //!
   TBranch        *b_m_NCmxHits;   //!
   TBranch        *b_m_NBmuHits;   //!
   TBranch        *b_m_TrackPt;   //!
   TBranch        *b_m_Iso;   //!
   TBranch        *b_m_CmuDelX;   //!
   TBranch        *b_m_CmpDelX;   //!
   TBranch        *b_m_CmxDelX;   //!
   TBranch        *b_m_BmuDelX;   //!
   TBranch        *b_m_HasCmuStub;   //!
   TBranch        *b_m_HasCmpStub;   //!
   TBranch        *b_m_HasCmxStub;   //!
   TBranch        *b_m_HasBmuStub;   //!
   TBranch        *b_m_TrackBcPt;   //!
   TBranch        *b_m_EmEnergy;   //!
   TBranch        *b_m_HadEnergy;   //!
   TBranch        *b_m_Z0;   //!
   TBranch        *b_m_D0;   //!
   TBranch        *b_m_mupt;   //!
   TBranch        *b_m_NSvxHits;   //!
   TBranch        *b_m_d0Corr;   //!
   TBranch        *b_m_Charge;   //!
   TBranch        *b_m_TrackEta;   //!
   TBranch        *b_m_TrackPhi;   //!
   TBranch        *b_m_TrackPhi2;   //!
   TBranch        *b_m_Px;   //!
   TBranch        *b_m_Py;   //!
   TBranch        *b_m_Pz;   //!
   TBranch        *b_m_P0;   //!
   TBranch        *b_mb_NAMuons;   //!
   TBranch        *b_mb_TrackNumber;   //!
   TBranch        *b_mb_NCmuHits;   //!
   TBranch        *b_mb_NCmpHits;   //!
   TBranch        *b_mb_NCmxHits;   //!
   TBranch        *b_mb_NBmuHits;   //!
   TBranch        *b_mb_TrackPt;   //!
   TBranch        *b_mb_Iso;   //!
   TBranch        *b_mb_CmuDelX;   //!
   TBranch        *b_mb_CmpDelX;   //!
   TBranch        *b_mb_CmxDelX;   //!
   TBranch        *b_mb_BmuDelX;   //!
   TBranch        *b_mb_HasCmuStub;   //!
   TBranch        *b_mb_HasCmpStub;   //!
   TBranch        *b_mb_HasCmxStub;   //!
   TBranch        *b_mb_HasBmuStub;   //!
   TBranch        *b_mb_TrackBcPt;   //!
   TBranch        *b_mb_EmEnergy;   //!
   TBranch        *b_mb_HadEnergy;   //!
   TBranch        *b_mb_Z0;   //!
   TBranch        *b_mb_D0;   //!
   TBranch        *b_mb_mupt;   //!
   TBranch        *b_mb_NSvxHits;   //!
   TBranch        *b_mb_d0Corr;   //!
   TBranch        *b_mb_Charge;   //!
   TBranch        *b_mb_TrackEta;   //!
   TBranch        *b_mb_TrackPhi;   //!
   TBranch        *b_mb_TrackPhi2;   //!
   TBranch        *b_mb_Px;   //!
   TBranch        *b_mb_Py;   //!
   TBranch        *b_mb_Pz;   //!
   TBranch        *b_mb_P0;   //!
   TBranch        *b_mi_Met;   //!
   TBranch        *b_mi_MetPhi;   //!
   TBranch        *b_mi_Ht;   //!
   TBranch        *b_mi_HtCorr;   //!

   xxx(TTree *tree=0);
   ~xxx();
   Int_t  Cut(Int_t entry);
   Int_t  GetEntry(Int_t entry);
   Int_t  LoadTree(Int_t entry);
   void   Init(TTree *tree);
   void   Loop();
   Bool_t Notify();
   void   Show(Int_t entry = -1);
};

#endif

#ifdef xxx_cxx
xxx::xxx(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
#ifdef SINGLE_TREE
      // The following code should be used if you want this class to access
      // a single tree instead of a chain
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("Memory Directory");
      if (!f) {
         f = new TFile("Memory Directory");
         f->cd("Rint:/");
      }
      tree = (TTree*)gDirectory->Get("Photon");

#else // SINGLE_TREE

      // The following code should be used if you want this class to access a chain
      // of trees.
      TChain * chain = new TChain("Photon","");
      chain->Add("./TDarkPhoton_Ntuple_1.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_2.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_3.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_4.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_5.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_6.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_7.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_8.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_9.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_10.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_11.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_12.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_13.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_14.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_15.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_16.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_17.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_18.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_19.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_20.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_21.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_22.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_23.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_24.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_25.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_26.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_27.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_28.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_29.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_30.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_31.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_32.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_33.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_34.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_35.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_36.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_37.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_38.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_39.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_40.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_41.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_42.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_43.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_44.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_45.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_46.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_47.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_48.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_49.root/Photon");
      chain->Add("./TDarkPhoton_Ntuple_50.root/Photon");
      tree = chain;
#endif // SINGLE_TREE

   }
   Init(tree);
}

xxx::~xxx()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t xxx::GetEntry(Int_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Int_t xxx::LoadTree(Int_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Int_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->IsA() != TChain::Class()) return centry;
   TChain *chain = (TChain*)fChain;
   if (chain->GetTreeNumber() != fCurrent) {
      fCurrent = chain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void xxx::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses of the tree
   // will be set. It is normaly not necessary to make changes to the
   // generated code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running with PROOF.

   // Set branch addresses
   if (tree == 0) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("ph_Npho",&ph_Npho);
   fChain->SetBranchAddress("ph_E",ph_E);
   fChain->SetBranchAddress("ph_Et",ph_Et);
   fChain->SetBranchAddress("ph_Etc",ph_Etc);
   fChain->SetBranchAddress("ph_Cesx",ph_Cesx);
   fChain->SetBranchAddress("ph_Cesz",ph_Cesz);
   fChain->SetBranchAddress("ph_Deta",ph_Deta);
   fChain->SetBranchAddress("ph_Eta",ph_Eta);
   fChain->SetBranchAddress("ph_Phi",ph_Phi);
   fChain->SetBranchAddress("ph_SinTheta",ph_SinTheta);
   fChain->SetBranchAddress("ph_N3d",ph_N3d);
   fChain->SetBranchAddress("ph_Chi2",ph_Chi2);
   fChain->SetBranchAddress("ph_NIsoTrk",ph_NIsoTrk);
   fChain->SetBranchAddress("ph_Pt",ph_Pt);
   fChain->SetBranchAddress("ph_Tiso",ph_Tiso);
   fChain->SetBranchAddress("e_NElectrons",&e_NElectrons);
   fChain->SetBranchAddress("e_TotalEt",e_TotalEt);
   fChain->SetBranchAddress("e_Et",e_Et);
   fChain->SetBranchAddress("e_E",e_E);
   fChain->SetBranchAddress("e_TrackPt",e_TrackPt);
   fChain->SetBranchAddress("e_Nasl",e_Nasl);
   fChain->SetBranchAddress("e_Nssl",e_Nssl);
   fChain->SetBranchAddress("e_HadEm",e_HadEm);
   fChain->SetBranchAddress("e_Iso",e_Iso);
   fChain->SetBranchAddress("e_Lshr",e_Lshr);
   fChain->SetBranchAddress("e_EP",e_EP);
   fChain->SetBranchAddress("e_DelXQ",e_DelXQ);
   fChain->SetBranchAddress("e_DelZ",e_DelZ);
   fChain->SetBranchAddress("e_Z0",e_Z0);
   fChain->SetBranchAddress("e_Chi2Strip",e_Chi2Strip);
   fChain->SetBranchAddress("e_DetectorCode",&e_DetectorCode);
   fChain->SetBranchAddress("e_TrackNumber",&e_TrackNumber);
   fChain->SetBranchAddress("e_ClusterEmEt",e_ClusterEmEt);
   fChain->SetBranchAddress("e_ClusterHadEt",e_ClusterHadEt);
   fChain->SetBranchAddress("e_Charge",e_Charge);
   fChain->SetBranchAddress("e_TrackEta",e_TrackEta);
   fChain->SetBranchAddress("e_TrackPhi",e_TrackPhi);
   fChain->SetBranchAddress("e_TrackPhi2",e_TrackPhi2);
   fChain->SetBranchAddress("e_Px",e_Px);
   fChain->SetBranchAddress("e_Py",e_Py);
   fChain->SetBranchAddress("e_Pz",e_Pz);
   fChain->SetBranchAddress("e_P0",e_P0);
   fChain->SetBranchAddress("eb_NPositrons",&eb_NPositrons);
   fChain->SetBranchAddress("eb_TotalEt",eb_TotalEt);
   fChain->SetBranchAddress("eb_Et",eb_Et);
   fChain->SetBranchAddress("eb_E",eb_E);
   fChain->SetBranchAddress("eb_TrackPt",eb_TrackPt);
   fChain->SetBranchAddress("eb_Nasl",eb_Nasl);
   fChain->SetBranchAddress("eb_Nssl",eb_Nssl);
   fChain->SetBranchAddress("eb_HadEm",eb_HadEm);
   fChain->SetBranchAddress("eb_Iso",eb_Iso);
   fChain->SetBranchAddress("eb_Lshr",eb_Lshr);
   fChain->SetBranchAddress("eb_EP",eb_EP);
   fChain->SetBranchAddress("eb_DelXQ",eb_DelXQ);
   fChain->SetBranchAddress("eb_DelZ",eb_DelZ);
   fChain->SetBranchAddress("eb_Z0",eb_Z0);
   fChain->SetBranchAddress("eb_Chi2Strip",eb_Chi2Strip);
   fChain->SetBranchAddress("eb_DetectorCode",&eb_DetectorCode);
   fChain->SetBranchAddress("eb_TrackNumber",&eb_TrackNumber);
   fChain->SetBranchAddress("eb_ClusterEmEt",eb_ClusterEmEt);
   fChain->SetBranchAddress("eb_ClusterHadEt",eb_ClusterHadEt);
   fChain->SetBranchAddress("eb_Charge",eb_Charge);
   fChain->SetBranchAddress("eb_TrackEta",eb_TrackEta);
   fChain->SetBranchAddress("eb_TrackPhi",eb_TrackPhi);
   fChain->SetBranchAddress("eb_TrackPhi2",eb_TrackPhi2);
   fChain->SetBranchAddress("eb_Px",eb_Px);
   fChain->SetBranchAddress("eb_Py",eb_Py);
   fChain->SetBranchAddress("eb_Pz",eb_Pz);
   fChain->SetBranchAddress("eb_P0",eb_P0);
   fChain->SetBranchAddress("m_NMuons",&m_NMuons);
   fChain->SetBranchAddress("m_TrackNumber",m_TrackNumber);
   fChain->SetBranchAddress("m_NCmuHits",m_NCmuHits);
   fChain->SetBranchAddress("m_NCmpHits",m_NCmpHits);
   fChain->SetBranchAddress("m_NCmxHits",m_NCmxHits);
   fChain->SetBranchAddress("m_NBmuHits",m_NBmuHits);
   fChain->SetBranchAddress("m_TrackPt",m_TrackPt);
   fChain->SetBranchAddress("m_Iso",m_Iso);
   fChain->SetBranchAddress("m_CmuDelX",m_CmuDelX);
   fChain->SetBranchAddress("m_CmpDelX",m_CmpDelX);
   fChain->SetBranchAddress("m_CmxDelX",m_CmxDelX);
   fChain->SetBranchAddress("m_BmuDelX",m_BmuDelX);
   fChain->SetBranchAddress("m_HasCmuStub",m_HasCmuStub);
   fChain->SetBranchAddress("m_HasCmpStub",m_HasCmpStub);
   fChain->SetBranchAddress("m_HasCmxStub",m_HasCmxStub);
   fChain->SetBranchAddress("m_HasBmuStub",m_HasBmuStub);
   fChain->SetBranchAddress("m_TrackBcPt",m_TrackBcPt);
   fChain->SetBranchAddress("m_EmEnergy",m_EmEnergy);
   fChain->SetBranchAddress("m_HadEnergy",m_HadEnergy);
   fChain->SetBranchAddress("m_Z0",m_Z0);
   fChain->SetBranchAddress("m_D0",m_D0);
   fChain->SetBranchAddress("m_mupt",m_mupt);
   fChain->SetBranchAddress("m_NSvxHits",m_NSvxHits);
   fChain->SetBranchAddress("m_d0Corr",m_d0Corr);
   fChain->SetBranchAddress("m_Charge",m_Charge);
   fChain->SetBranchAddress("m_TrackEta",m_TrackEta);
   fChain->SetBranchAddress("m_TrackPhi",m_TrackPhi);
   fChain->SetBranchAddress("m_TrackPhi2",m_TrackPhi2);
   fChain->SetBranchAddress("m_Px",m_Px);
   fChain->SetBranchAddress("m_Py",m_Py);
   fChain->SetBranchAddress("m_Pz",m_Pz);
   fChain->SetBranchAddress("m_P0",m_P0);
   fChain->SetBranchAddress("mb_NAMuons",&mb_NAMuons);
   fChain->SetBranchAddress("mb_TrackNumber",mb_TrackNumber);
   fChain->SetBranchAddress("mb_NCmuHits",mb_NCmuHits);
   fChain->SetBranchAddress("mb_NCmpHits",mb_NCmpHits);
   fChain->SetBranchAddress("mb_NCmxHits",mb_NCmxHits);
   fChain->SetBranchAddress("mb_NBmuHits",mb_NBmuHits);
   fChain->SetBranchAddress("mb_TrackPt",mb_TrackPt);
   fChain->SetBranchAddress("mb_Iso",mb_Iso);
   fChain->SetBranchAddress("mb_CmuDelX",mb_CmuDelX);
   fChain->SetBranchAddress("mb_CmpDelX",mb_CmpDelX);
   fChain->SetBranchAddress("mb_CmxDelX",mb_CmxDelX);
   fChain->SetBranchAddress("mb_BmuDelX",mb_BmuDelX);
   fChain->SetBranchAddress("mb_HasCmuStub",mb_HasCmuStub);
   fChain->SetBranchAddress("mb_HasCmpStub",mb_HasCmpStub);
   fChain->SetBranchAddress("mb_HasCmxStub",mb_HasCmxStub);
   fChain->SetBranchAddress("mb_HasBmuStub",mb_HasBmuStub);
   fChain->SetBranchAddress("mb_TrackBcPt",mb_TrackBcPt);
   fChain->SetBranchAddress("mb_EmEnergy",mb_EmEnergy);
   fChain->SetBranchAddress("mb_HadEnergy",mb_HadEnergy);
   fChain->SetBranchAddress("mb_Z0",mb_Z0);
   fChain->SetBranchAddress("mb_D0",mb_D0);
   fChain->SetBranchAddress("mb_mupt",mb_mupt);
   fChain->SetBranchAddress("mb_NSvxHits",mb_NSvxHits);
   fChain->SetBranchAddress("mb_d0Corr",mb_d0Corr);
   fChain->SetBranchAddress("mb_Charge",mb_Charge);
   fChain->SetBranchAddress("mb_TrackEta",mb_TrackEta);
   fChain->SetBranchAddress("mb_TrackPhi",mb_TrackPhi);
   fChain->SetBranchAddress("mb_TrackPhi2",mb_TrackPhi2);
   fChain->SetBranchAddress("mb_Px",mb_Px);
   fChain->SetBranchAddress("mb_Py",mb_Py);
   fChain->SetBranchAddress("mb_Pz",mb_Pz);
   fChain->SetBranchAddress("mb_P0",mb_P0);
   fChain->SetBranchAddress("mi_Met",&mi_Met);
   fChain->SetBranchAddress("mi_MetPhi",&mi_MetPhi);
   fChain->SetBranchAddress("mi_Ht",&mi_Ht);
   fChain->SetBranchAddress("mi_HtCorr",&mi_HtCorr);
   Notify();
}

Bool_t xxx::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. Typically here the branch pointers
   // will be retrieved. It is normaly not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed.

   // Get branch pointers
   b_ph_Npho = fChain->GetBranch("ph_Npho");
   b_ph_E = fChain->GetBranch("ph_E");
   b_ph_Et = fChain->GetBranch("ph_Et");
   b_ph_Etc = fChain->GetBranch("ph_Etc");
   b_ph_Cesx = fChain->GetBranch("ph_Cesx");
   b_ph_Cesz = fChain->GetBranch("ph_Cesz");
   b_ph_Deta = fChain->GetBranch("ph_Deta");
   b_ph_Eta = fChain->GetBranch("ph_Eta");
   b_ph_Phi = fChain->GetBranch("ph_Phi");
   b_ph_SinTheta = fChain->GetBranch("ph_SinTheta");
   b_ph_N3d = fChain->GetBranch("ph_N3d");
   b_ph_Chi2 = fChain->GetBranch("ph_Chi2");
   b_ph_NIsoTrk = fChain->GetBranch("ph_NIsoTrk");
   b_ph_Pt = fChain->GetBranch("ph_Pt");
   b_ph_Tiso = fChain->GetBranch("ph_Tiso");
   b_e_NElectrons = fChain->GetBranch("e_NElectrons");
   b_e_TotalEt = fChain->GetBranch("e_TotalEt");
   b_e_Et = fChain->GetBranch("e_Et");
   b_e_E = fChain->GetBranch("e_E");
   b_e_TrackPt = fChain->GetBranch("e_TrackPt");
   b_e_Nasl = fChain->GetBranch("e_Nasl");
   b_e_Nssl = fChain->GetBranch("e_Nssl");
   b_e_HadEm = fChain->GetBranch("e_HadEm");
   b_e_Iso = fChain->GetBranch("e_Iso");
   b_e_Lshr = fChain->GetBranch("e_Lshr");
   b_e_EP = fChain->GetBranch("e_EP");
   b_e_DelXQ = fChain->GetBranch("e_DelXQ");
   b_e_DelZ = fChain->GetBranch("e_DelZ");
   b_e_Z0 = fChain->GetBranch("e_Z0");
   b_e_Chi2Strip = fChain->GetBranch("e_Chi2Strip");
   b_e_DetectorCode = fChain->GetBranch("e_DetectorCode");
   b_e_TrackNumber = fChain->GetBranch("e_TrackNumber");
   b_e_ClusterEmEt = fChain->GetBranch("e_ClusterEmEt");
   b_e_ClusterHadEt = fChain->GetBranch("e_ClusterHadEt");
   b_e_Charge = fChain->GetBranch("e_Charge");
   b_e_TrackEta = fChain->GetBranch("e_TrackEta");
   b_e_TrackPhi = fChain->GetBranch("e_TrackPhi");
   b_e_TrackPhi2 = fChain->GetBranch("e_TrackPhi2");
   b_e_Px = fChain->GetBranch("e_Px");
   b_e_Py = fChain->GetBranch("e_Py");
   b_e_Pz = fChain->GetBranch("e_Pz");
   b_e_P0 = fChain->GetBranch("e_P0");
   b_eb_NPositrons = fChain->GetBranch("eb_NPositrons");
   b_eb_TotalEt = fChain->GetBranch("eb_TotalEt");
   b_eb_Et = fChain->GetBranch("eb_Et");
   b_eb_E = fChain->GetBranch("eb_E");
   b_eb_TrackPt = fChain->GetBranch("eb_TrackPt");
   b_eb_Nasl = fChain->GetBranch("eb_Nasl");
   b_eb_Nssl = fChain->GetBranch("eb_Nssl");
   b_eb_HadEm = fChain->GetBranch("eb_HadEm");
   b_eb_Iso = fChain->GetBranch("eb_Iso");
   b_eb_Lshr = fChain->GetBranch("eb_Lshr");
   b_eb_EP = fChain->GetBranch("eb_EP");
   b_eb_DelXQ = fChain->GetBranch("eb_DelXQ");
   b_eb_DelZ = fChain->GetBranch("eb_DelZ");
   b_eb_Z0 = fChain->GetBranch("eb_Z0");
   b_eb_Chi2Strip = fChain->GetBranch("eb_Chi2Strip");
   b_eb_DetectorCode = fChain->GetBranch("eb_DetectorCode");
   b_eb_TrackNumber = fChain->GetBranch("eb_TrackNumber");
   b_eb_ClusterEmEt = fChain->GetBranch("eb_ClusterEmEt");
   b_eb_ClusterHadEt = fChain->GetBranch("eb_ClusterHadEt");
   b_eb_Charge = fChain->GetBranch("eb_Charge");
   b_eb_TrackEta = fChain->GetBranch("eb_TrackEta");
   b_eb_TrackPhi = fChain->GetBranch("eb_TrackPhi");
   b_eb_TrackPhi2 = fChain->GetBranch("eb_TrackPhi2");
   b_eb_Px = fChain->GetBranch("eb_Px");
   b_eb_Py = fChain->GetBranch("eb_Py");
   b_eb_Pz = fChain->GetBranch("eb_Pz");
   b_eb_P0 = fChain->GetBranch("eb_P0");
   b_m_NMuons = fChain->GetBranch("m_NMuons");
   b_m_TrackNumber = fChain->GetBranch("m_TrackNumber");
   b_m_NCmuHits = fChain->GetBranch("m_NCmuHits");
   b_m_NCmpHits = fChain->GetBranch("m_NCmpHits");
   b_m_NCmxHits = fChain->GetBranch("m_NCmxHits");
   b_m_NBmuHits = fChain->GetBranch("m_NBmuHits");
   b_m_TrackPt = fChain->GetBranch("m_TrackPt");
   b_m_Iso = fChain->GetBranch("m_Iso");
   b_m_CmuDelX = fChain->GetBranch("m_CmuDelX");
   b_m_CmpDelX = fChain->GetBranch("m_CmpDelX");
   b_m_CmxDelX = fChain->GetBranch("m_CmxDelX");
   b_m_BmuDelX = fChain->GetBranch("m_BmuDelX");
   b_m_HasCmuStub = fChain->GetBranch("m_HasCmuStub");
   b_m_HasCmpStub = fChain->GetBranch("m_HasCmpStub");
   b_m_HasCmxStub = fChain->GetBranch("m_HasCmxStub");
   b_m_HasBmuStub = fChain->GetBranch("m_HasBmuStub");
   b_m_TrackBcPt = fChain->GetBranch("m_TrackBcPt");
   b_m_EmEnergy = fChain->GetBranch("m_EmEnergy");
   b_m_HadEnergy = fChain->GetBranch("m_HadEnergy");
   b_m_Z0 = fChain->GetBranch("m_Z0");
   b_m_D0 = fChain->GetBranch("m_D0");
   b_m_mupt = fChain->GetBranch("m_mupt");
   b_m_NSvxHits = fChain->GetBranch("m_NSvxHits");
   b_m_d0Corr = fChain->GetBranch("m_d0Corr");
   b_m_Charge = fChain->GetBranch("m_Charge");
   b_m_TrackEta = fChain->GetBranch("m_TrackEta");
   b_m_TrackPhi = fChain->GetBranch("m_TrackPhi");
   b_m_TrackPhi2 = fChain->GetBranch("m_TrackPhi2");
   b_m_Px = fChain->GetBranch("m_Px");
   b_m_Py = fChain->GetBranch("m_Py");
   b_m_Pz = fChain->GetBranch("m_Pz");
   b_m_P0 = fChain->GetBranch("m_P0");
   b_mb_NAMuons = fChain->GetBranch("mb_NAMuons");
   b_mb_TrackNumber = fChain->GetBranch("mb_TrackNumber");
   b_mb_NCmuHits = fChain->GetBranch("mb_NCmuHits");
   b_mb_NCmpHits = fChain->GetBranch("mb_NCmpHits");
   b_mb_NCmxHits = fChain->GetBranch("mb_NCmxHits");
   b_mb_NBmuHits = fChain->GetBranch("mb_NBmuHits");
   b_mb_TrackPt = fChain->GetBranch("mb_TrackPt");
   b_mb_Iso = fChain->GetBranch("mb_Iso");
   b_mb_CmuDelX = fChain->GetBranch("mb_CmuDelX");
   b_mb_CmpDelX = fChain->GetBranch("mb_CmpDelX");
   b_mb_CmxDelX = fChain->GetBranch("mb_CmxDelX");
   b_mb_BmuDelX = fChain->GetBranch("mb_BmuDelX");
   b_mb_HasCmuStub = fChain->GetBranch("mb_HasCmuStub");
   b_mb_HasCmpStub = fChain->GetBranch("mb_HasCmpStub");
   b_mb_HasCmxStub = fChain->GetBranch("mb_HasCmxStub");
   b_mb_HasBmuStub = fChain->GetBranch("mb_HasBmuStub");
   b_mb_TrackBcPt = fChain->GetBranch("mb_TrackBcPt");
   b_mb_EmEnergy = fChain->GetBranch("mb_EmEnergy");
   b_mb_HadEnergy = fChain->GetBranch("mb_HadEnergy");
   b_mb_Z0 = fChain->GetBranch("mb_Z0");
   b_mb_D0 = fChain->GetBranch("mb_D0");
   b_mb_mupt = fChain->GetBranch("mb_mupt");
   b_mb_NSvxHits = fChain->GetBranch("mb_NSvxHits");
   b_mb_d0Corr = fChain->GetBranch("mb_d0Corr");
   b_mb_Charge = fChain->GetBranch("mb_Charge");
   b_mb_TrackEta = fChain->GetBranch("mb_TrackEta");
   b_mb_TrackPhi = fChain->GetBranch("mb_TrackPhi");
   b_mb_TrackPhi2 = fChain->GetBranch("mb_TrackPhi2");
   b_mb_Px = fChain->GetBranch("mb_Px");
   b_mb_Py = fChain->GetBranch("mb_Py");
   b_mb_Pz = fChain->GetBranch("mb_Pz");
   b_mb_P0 = fChain->GetBranch("mb_P0");
   b_mi_Met = fChain->GetBranch("mi_Met");
   b_mi_MetPhi = fChain->GetBranch("mi_MetPhi");
   b_mi_Ht = fChain->GetBranch("mi_Ht");
   b_mi_HtCorr = fChain->GetBranch("mi_HtCorr");

   return kTRUE;
}

void xxx::Show(Int_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t xxx::Cut(Int_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef xxx_cxx
