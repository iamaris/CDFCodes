#define xxx_cxx
#include "xxx.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include "TMath.h"


TH1F* ph_et = new TH1F("ph_et", "ph_et", 150, 0., 150.);
TH1F* ph_pt = new TH1F("ph_pt", "ph_pt", 150, 0., 150.);

TH1F* eall_pt = new TH1F("eall_pt", "eall_pt", 100, 0., 100.);
TH1F* eall_et = new TH1F("eall_et", "eall_et", 100, 0., 100.);
TH1F* eall_eta = new TH1F("eall_eta", "eall_eta", 80, -2., 2.);
TH1F* eall_phi = new TH1F("eall_phi", "eall_phi", 80, -4., 4.);

TH1F* e_pt = new TH1F("e_pt", "e_pt", 100, 0., 100.);
TH1F* e_et = new TH1F("e_et", "e_et", 100, 0., 100.);
TH1F* e_e = new TH1F("e_e", "e_e", 100, 0., 100.);

TH1F* eball_pt = new TH1F("eball_pt", "eball_pt", 100, 0., 100.);
TH1F* eball_et = new TH1F("eball_et", "eball_et", 100, 0., 100.);
TH1F* eball_eta = new TH1F("eball_eta", "eball_eta", 80, -2., 2.);
TH1F* eball_phi = new TH1F("eball_phi", "eball_phi", 80, -4., 4.);

TH1F* eb_pt = new TH1F("eb_pt", "eb_pt", 100, 0., 100.);
TH1F* eb_et = new TH1F("eb_et", "eb_et", 100, 0., 100.);
TH1F* eb_e = new TH1F("eb_e", "eb_e", 100, 0., 100.);

TH1F* e_pair = new TH1F("e_pair", "e_pair", 5, 0., 5.);


///////////////////////////////////////////////////////////////////
TH1F* ph_et_lep = new TH1F("ph_et_lep", "ph_et_lep", 150, 0, 150);
TH1F* ph_et_met = new TH1F("ph_et_met", "ph_et_met", 150, 0, 150);
TH1F* ph_et_ml = new TH1F("ph_et_ml", "ph_et_ml", 150, 0, 150);

TH1F* mi_met = new TH1F("mi_met", "mi_met", 100, 0, 50);

TH1F* eall_dr = new TH1F("eall_dr", "eall_dr", 70, 0, 7);
TH1F* eall_deta = new TH1F("eall_deta", "eall_deta", 80, -4, 4);
TH1F* eall_dphi = new TH1F("eall_dphi", "eall_dphi", 80, -4, 4);


TH1F* eall_pt_lead = new TH1F("eall_pt_lead", "eall_pt_lead", 100, 0, 100);
TH1F* eall_pt_trail = new TH1F("eall_pt_trail", "eall_pt_trail", 100, 0, 100);


TH1F* m_pt = new TH1F("m_pt", "m_pt", 100, 0, 100);
TH1F* m_e = new TH1F("m_e", "m_e", 100, 0, 100);

TH1F* mb_pt = new TH1F("mb_pt", "mb_pt", 100, 0, 100);
TH1F* mb_e = new TH1F("mb_e", "mb_e", 100, 0, 100);

TH1F* mall_dr = new TH1F("mall_dr", "mall_dr", 70, 0, 7);
TH1F* mall_deta = new TH1F("mall_deta", "mall_deta", 80, -4, 4);
TH1F* mall_dphi = new TH1F("mall_dphi", "mall_dphi", 80, -4, 4);

TH1F* mall_pt = new TH1F("mall_pt", "mall_pt", 100, 0, 100);
TH1F* mall_eta = new TH1F("mall_eta", "mall_eta", 80, -2, 2);
TH1F* mall_phi = new TH1F("mall_phi", "mall_phi", 80, -4, 4);
TH1F* mall_pt_lead = new TH1F("mall_pt_lead", "mall_pt_lead", 100, 0, 100);
TH1F* mall_pt_trail = new TH1F("mall_pt_trail", "mall_pt_trail", 100, 0, 100);

TH1F* dp_mass = new TH1F("dp_mass", "dp_mass", 500, 0, 500);
TH2F* p1p2 = new TH2F("p1p2","p1p2",80,-4.,4.,80,-4.,4.);
TH2F* eta1eta2 = new TH2F("eta1eta2","eta1eta2",80,-4.,4.,80,-4.,4.);

TH1F* m_dp_mass = new TH1F("m_dp_mass", "m_dp_mass", 500, 0, 500);
TH2F* m_p1p2 = new TH2F("m_p1p2","m_p1p2",80,-4.,4.,80,-4.,4.);
TH2F* m_eta1eta2 = new TH2F("m_eta1eta2","m_eta1eta2",80,-4.,4.,80,-4.,4.);

float PI = TMath::Pi();

void xxx::Loop()
{
//   In a ROOT session, you can do:
//      Root > .L xxx.C
//      Root > xxx t
//      Root > t.GetEntry(12); // Fill t data members with entry number 12
//      Root > t.Show();       // Show values of entry 12
//      Root > t.Show(16);     // Read and show values of entry 16
//      Root > t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;

   Int_t nentries = Int_t(fChain->GetEntriesFast());

   Int_t nbytes = 0, nb = 0;
   for (Int_t jentry=0; jentry<nentries;jentry++) {
      Int_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;

      //good photon loop
      int gp = 0;

      if (ph_Npho>0) {
        
        for (int i=0;i<ph_Npho;i++) {

           if(ph_Pt[i]>20) { 
           ph_pt->Fill(ph_Pt[i]);
           }
           if(ph_Et[i]>30) { 
           ph_et->Fill(ph_Et[i]);
           gp++;
           }
        }
    
        if (gp > 0 &&  mi_Met > 20.0) {     
           //pair of e- & e+ loop
           int   ge5          = 0;
           int   ge10         = 0;
           int   geb5         = 0;
           int   geb10        = 0;
           int   index5[20]   = {-1};
           int   index10[20]  = {-1};
           int   indexb5[20]  = {-1};
           int   indexb10[20] = {-1};

          //good electron
          for (int i=0;i<e_NElectrons;i++) {
            if (e_Et[i]<10) continue;
            if (e_TrackPt[i]<2) continue;
            if (e_Nasl[i]<=3) continue;
            if (e_Nssl[i]<=2) continue;
            if (e_HadEm[i]>=(0.055+0.00045*e_E[i])) continue;
            //if ((e_Iso[i]/e_Et[i])>=0.1) continue;
            if (e_Lshr[i]>0.2) continue;
            if (e_EP[i]>2.0 && e_TrackPt[i]<50 ) continue;
            if (e_DelXQ[i]>1.5 && e_DelXQ[i]<=-3.0) continue;
            if (fabs(e_DelZ[i])>3.0) continue;
            if (fabs(e_Z0[i])<=60) continue;
            if (e_Chi2Strip[i]>10.0) continue;
          
            eall_pt->Fill(e_TrackPt[i]);
            eall_et->Fill(e_Et[i]);
            eall_eta->Fill(e_TrackEta[i]);
            eall_phi->Fill(e_TrackPhi2[i]);
            if (e_TrackPt[i]>= 5 /*&& e_TrackPt[i]< 500*/) {
              e_pt->Fill(e_TrackPt[i]);
              e_e->Fill(e_E[i]);
              e_et->Fill(e_Et[i]);
              index5[ge5] = i;
              ge5++;
              if (e_TrackPt[i] >= 10) {
                index10[ge10] = i;
                ge10++;
              }
            }
          }
          
          //good positron
          for (int i=0;i<eb_NPositrons;i++) {
            if (eb_Et[i]<10) continue;
            if (eb_TrackPt[i]<2) continue;
            if (eb_Nasl[i]<=3) continue;
            if (eb_Nssl[i]<=2) continue;
            if (eb_HadEm[i]>=(0.055+0.00045*eb_E[i])) continue;
            //if ((eb_Iso[i]/eb_Et[i])>=0.1) continue;
            if (eb_Lshr[i]>0.2) continue;
            if (eb_EP[i]>2.0 && eb_TrackPt[i]<50 ) continue;
            if (eb_DelXQ[i]>1.5 && eb_DelXQ[i]<=-3.0) continue;
            if (fabs(eb_DelZ[i])>3.0) continue;
            if (fabs(eb_Z0[i])<=60) continue;
            if (eb_Chi2Strip[i]>10.0) continue;
          
            eball_pt->Fill(eb_TrackPt[i]);
            eball_et->Fill(eb_Et[i]);
            eball_eta->Fill(eb_TrackEta[i]);
            eball_phi->Fill(eb_TrackPhi2[i]);
            if (eb_TrackPt[i]>= 5 /*&& eb_TrackPt[i]< 500*/) {
              eb_pt->Fill(eb_TrackPt[i]);
              eb_e->Fill(eb_E[i]);
              eb_et->Fill(eb_Et[i]);
              indexb5[geb5] = i;
              geb5++;
              if (eb_TrackPt[i] >= 10) {
                indexb10[geb10] = i;
                geb10++;
              }
            }
          }
      
        if ((geb10>0 && ge5>0) || (ge10>0 && geb5>0)) {
          e_pair->Fill(1.0);
        }

        }//gp&met
      }//ph_Npho
   }//jentry


   TObjArray hlist(0);

   hlist.Add( ph_et );
   hlist.Add( ph_pt );

   hlist.Add( eall_pt );
   hlist.Add( eall_et );
   hlist.Add( eall_eta );
   hlist.Add( eall_phi );

   hlist.Add( e_pt );
   hlist.Add( e_et );
   hlist.Add( e_e );

   hlist.Add( eball_pt );
   hlist.Add( eball_et );
   hlist.Add( eball_eta );
   hlist.Add( eball_phi );

   hlist.Add( eb_pt );
   hlist.Add( eb_et );
   hlist.Add( eb_e );

   hlist.Add( e_pair );

   TFile fout("aris.root", "recreate");
   hlist.Write();
   fout.Close();


}
