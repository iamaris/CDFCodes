#define zdarkp_cxx
#include "zdarkp.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include "TMath.h"

TH1F* mi_met = new TH1F("mi_met", "mi_met", 100, 0, 50);

TH1F* ph_n = new TH1F("ph_n", "ph_n", 10, 0., 10.);
TH1F* ph_pt20 = new TH1F("ph_pt20", "ph_pt20", 5, 0., 5.);
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

TH1F* mall_pt = new TH1F("mall_pt", "mall_pt", 100, 0., 100.);
TH1F* mall_eta = new TH1F("mall_eta", "mall_eta", 80, -2., 2.);
TH1F* mall_phi = new TH1F("mall_phi", "mall_phi", 80, -4., 4.);

TH1F* m_pt = new TH1F("m_pt", "m_pt", 100, 0., 100.);

TH1F* mball_pt = new TH1F("mball_pt", "mball_pt", 100, 0., 100.);
TH1F* mball_eta = new TH1F("mball_eta", "mball_eta", 80, -2., 2.);
TH1F* mball_phi = new TH1F("mball_phi", "mball_phi", 80, -4., 4.);

TH1F* mb_pt = new TH1F("mb_pt", "mb_pt", 100, 0., 100.);

TH1F* m_pair = new TH1F("m_pair", "m_pair", 5, 0., 5.);

float PI = TMath::Pi();

void zdarkp::Loop()
{
//   In a ROOT session, you can do:
//      Root > .L zdarkp.C
//      Root > zdarkp t
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

      mi_met->Fill(mi_Met);
      //good photon loop
      int pt20 = 0;
      int gp = 0;
      ph_n->Fill(ph_Npho);

      if (ph_Npho>0) {
        
        for (int i=0;i<ph_Npho;i++) {

           if(ph_Pt[i]>20) { 
           ph_pt->Fill(ph_Pt[i]);
           pt20++;
           }
           if(ph_Et[i]>30) { 
           ph_et->Fill(ph_Et[i]);
           gp++;
           }
        }
        
        if (pt20>0) {
        ph_pt20->Fill(pt20);
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
            if (e_Good[i]==0) continue;
          
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
            if (eb_Good[i]==0) continue;
          
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

          //pair of mu- & mu+ loop
           int   gm5          = 0;
           int   gm10         = 0;
           int   gmb5         = 0;
           int   gmb10        = 0;
           int   indexm5[20]   = {-1};
           int   indexm10[20]  = {-1};
           int   indexmb5[20]  = {-1};
           int   indexmb10[20] = {-1};

          //good muon
          for (int i=0;i<m_NMuons;i++) {
            if (m_Good[i]==0) continue;
          
            mall_pt->Fill(m_TrackPt[i]);
            mall_eta->Fill(m_TrackEta[i]);
            mall_phi->Fill(m_TrackPhi2[i]);
            if (m_TrackPt[i]>= 5 /*&& m_TrackPt[i]< 500*/) {
              m_pt->Fill(m_TrackPt[i]);
              indexm5[gm5] = i;
              gm5++;
              if (m_TrackPt[i] >= 10) {
                indexm10[gm10] = i;
                gm10++;
              }
            }
          }
          
          //good anti-muon
          for (int i=0;i<mb_NAMuons;i++) {
            if (mb_Good[i]==0) continue;
          
            mball_pt->Fill(mb_TrackPt[i]);
            mball_eta->Fill(mb_TrackEta[i]);
            mball_phi->Fill(mb_TrackPhi2[i]);
            if (mb_TrackPt[i]>= 5 /*&& mb_TrackPt[i]< 500*/) {
              mb_pt->Fill(mb_TrackPt[i]);
              indexmb5[gmb5] = i;
              gmb5++;
              if (mb_TrackPt[i] >= 10) {
                indexmb10[gmb10] = i;
                gmb10++;
              }
            }
          }
      
        if ((gmb10>0 && gm5>0) || (gm10>0 && gmb5>0)) {
          m_pair->Fill(1.0);
        }

        }//gp&met
      }//ph_Npho
   }//jentry


   TObjArray hlist(0);

   hlist.Add( ph_n );
   hlist.Add( ph_pt20 );
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

   hlist.Add( mall_pt );
   hlist.Add( mall_eta );
   hlist.Add( mall_phi );

   hlist.Add( m_pt );

   hlist.Add( mball_pt );
   hlist.Add( mball_eta );
   hlist.Add( mball_phi );

   hlist.Add( mb_pt );

   hlist.Add( m_pair );

   TFile fout("aris.root", "recreate");
   hlist.Write();
   fout.Close();


}

