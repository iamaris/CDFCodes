// SimValModule - source file 
// Original author: Elena Gerchtein, CMU


// $Id: SimValModule.cc,v 1.1 2002/05/14 00:51:44 lena Exp $

#include "SimulationMods/SimValModule.hh"

#ifndef HEPHIST1D_H
#include "HepTuple/HepHist1D.h"
#endif
#ifndef HEPHIST2D_H
#include "HepTuple/HepHist2D.h"
#endif
#include "AbsEnv/AbsEnv.hh"
#include "Edm/EventRecord.hh"
#include "Edm/ConstHandle.hh"
#include "SimulationObjects/OBSP_StorableBank.hh"
#include "SimulationObjects/OBSV_StorableBank.hh"
#include "SimulationObjects/HEPG_StorableBank.hh"
#include "ParticleDB/ParticleDb.hh"
#include "TrackingObjects/Storable/StorableRun2SiStripSet.hh"
#include "SimulationObjects/MVTX_StorableBank.hh"
#include "RawDataBanks/TOFD_StorableBank.hh"

using namespace std;

class NameEqual {
public:
  NameEqual(const string& name) : hist(name) {}
  bool operator()(HepObj* ptr) const { return hist==ptr->title(); }
  string hist;
};

SimValModule::SimValModule( const char* const theName, 
			    const char* const theDescription) 
  :  HepHistModule( theName, theDescription )
{}

SimValModule::~SimValModule() {}

AppResult SimValModule::beginJob( AbsEvent* aJob ) {
  return AppResult::OK;
}

AppResult SimValModule::beginRun( AbsEvent* aRun ) {
  return AppResult::OK;
}

AppResult SimValModule::event( AbsEvent* anEvent ) {

  // **** HEPG bank
  EventRecord::ConstIterator ihepg(anEvent, "HEPG_StorableBank");
  if (ihepg.is_valid()) {
    ConstHandle<HEPG_StorableBank> hepg(ihepg);
    Plot("HEPG - n_particles", hepg->n_particles(),100,0., 1.);      
  }

  // **** MVTX bank 
  EventRecord::ConstIterator imvtx(anEvent, "MVTX_StorableBank");  
  if (imvtx.is_valid()) {
    ConstHandle<MVTX_StorableBank> mvtx(imvtx);
    for ( int i=0; i<mvtx->NVertices(); i++ ) { 
      Plot("MVTX - z position of PV", mvtx->Vz(i),100,-100., 100.);      
    }
  }  

  // **** OBSP, OBSV banks
  EventRecord::ConstIterator iobsp(anEvent, "OBSP_StorableBank");
  EventRecord::ConstIterator iobsv(anEvent, "OBSV_StorableBank");
  if (iobsp.is_valid() && iobsv.is_valid()) {
    ConstHandle<OBSP_StorableBank> obsp(iobsp);
    ConstHandle<OBSV_StorableBank> obsv(iobsv);
    int np = obsp->n_particles();
    int nv = obsv->n_vertices();
    Plot("OBSP - n_particles", np, 200, 0., 200. );
    Plot("OBSV - n_vertices", np, 50, 0., 50. );
    for (int i = 0; i<np; i++) {
      OBSP_StorableBank::particleIter p(*obsp, i);
      assert(p.is_valid());
      int q = (int) ParticleDb::Instance()->Charge(obsp->CdfCode(p));
      Plot("OBSP - part.charge", q, 10, -5., 5. );
      double px = obsp->Px(p), py = obsp->Py(p), pz = obsp->Pz(p);
      Plot("OBSP - p",sqrt(px*px + py*py + pz*pz) , 100, 0., 200. );
      Plot("OBSP - p",sqrt(px*px + py*py + pz*pz) , 100, 0., 10. );
      Plot("OBSP - pt",sqrt(px*px + py*py + pz*pz) , 100, 0., 200. );
      Plot("OBSP - pt",sqrt(px*px + py*py + pz*pz) , 100, 0., 10. );
    }
    for (int i = 0; i<nv; i++) {
      OBSV_StorableBank::vertexIter v(*obsv, i);
      double vx = obsv->X(v), vy = obsv->Y(v), vz = obsv->Z(v);
      Plot("OBSV - vz",vz, 100, -400.,400.);      
      Plot("OBSV - vr",sqrt(vx*vx + vy*vy), 100, -200.,200.);      
    }
  }

  // **** S V X

  StorableRun2SiStripSet *stripSet = new StorableRun2SiStripSet();
  stripSet->loadFromBanks(anEvent);
  
  int s_0=0, s_1=0, s_2=0, s_3=0, s_4=0, s_5=0, s_6=0, s_7=0;
  
  for (SiStripSet::digi_iterator digiIter = 
	 stripSet->contents().begin();
       digiIter != stripSet->contents().end(); 
       ++digiIter) {
    int layer =  (*digiIter).first.getLayer();
    if ( layer == 0 ) { s_0 =+ (*digiIter).second.size(); }
    if ( layer == 1 ) { s_1 =+ (*digiIter).second.size(); }
    if ( layer == 2 ) { s_2 =+ (*digiIter).second.size(); }
    if ( layer == 3 ) { s_3 =+ (*digiIter).second.size(); }
    if ( layer == 4 ) { s_4 =+ (*digiIter).second.size(); }
    if ( layer == 5 ) { s_5 =+ (*digiIter).second.size(); }
    if ( layer == 6 ) { s_6 =+ (*digiIter).second.size(); }
    if ( layer == 7 ) { s_7 =+ (*digiIter).second.size(); }
  }
  Plot("SiStripSet - nstrips", stripSet->nStrips(), 100, 0., 5000. );
  Plot("SiStripSet - nstrips layer0", s_0 , 100, 0., 700. );
  Plot("SiStripSet - nstrips layer1", s_1 , 100, 0., 700. );
  Plot("SiStripSet - nstrips layer2", s_2 , 100, 0., 700. );
  Plot("SiStripSet - nstrips layer3", s_3 , 100, 0., 700. );
  Plot("SiStripSet - nstrips layer4", s_4 , 100, 0., 700. );
  Plot("SiStripSet - nstrips layer5", s_5 , 100, 0., 700. );
  Plot("SiStripSet - nstrips layer6", s_6 , 100, 0., 700. );
  Plot("SiStripSet - nstrips layer7", s_7 , 100, 0., 700. );  

  // ****  T O F 

  int _chargeThreshold = 100;
  int _timeThreshold   = 100;
  int NHits = 0;
  int charge, time;
  EventRecord::ConstIterator iTOFD( anEvent, "TOFD_StorableBank");
  if ( iTOFD.is_valid( ) ) { 
    ConstHandle<TOFD_StorableBank> TOFD( iTOFD );
    for (TOFD_StorableBank::ConstGrandBankIter hit(TOFD); hit.is_valid(); ++hit) {
      charge = TOFD->get_charge(hit);
      time     = TOFD->get_time(hit);
      if ((charge >= _chargeThreshold)&&(time >= _timeThreshold)) {
	NHits++;
      }
    }
  }
  Plot("TOF number of hits per event", NHits, 100, 0.0, 433.0 );
  //  Plot("TOF - number of bars with hits", NHits, 217, -0.5, 216.5 );
  //Plot("TOF - Energy deposited(MeV)", NHits, 200, 0., 20. );
  //Plot("TOF - Hit time(ns)", NHits, 200, 0., 20. );
  //Plot("TOF - dE/dx (MeV/cm) vs momentum (GeV/c)", NHits, 200, 0., 20. );
  

  // **** C A L O R I M E T E R - G F L A S H

  float CEM_eme=0., CEM_had=0., CEN_tot=0.;
  float CHA_eme=0., CHA_had=0., CHA_tot=0.;
  float PEM_eme=0., PEM_had=0., PEM_tot=0.;
  float PHA_eme=0., PHA_had=0., PHA_tot=0.;
  float WHA_eme=0., WHA_had=0., WHA_tot=0.;
  


  return AppResult::OK;
}

AppResult SimValModule::other( AbsEvent* anEvent ) {
  return AppResult::OK;
}

AppResult SimValModule::endRun( AbsEvent* aRun ) {
  return AppResult::OK;
}

AppResult SimValModule::endJob( AbsEvent* aJob ) {
  return AppResult::OK;
}

AppResult SimValModule::abortJob( AbsEvent* aJob ) {
  return AppResult::OK;
}

// graphics 

void SimValModule::Plot(const std::string& title, 
			const float& var, 
			const int& nbins, 
			const float& low, 
			const float& high, float weight) {
  vector<HepObj*>::iterator result = find_if(_histList.begin(), _histList.end(),
					     NameEqual(title));
  HepHist1D* h;
  if (result == _histList.end()) {
    h = &fileManager()->hist1D(title,nbins,low,high);
    _histList.push_back(h);
  }
  else { 
    h = &fileManager()->retrieveHist1D(title);
  }
  h->accumulate(var,weight);
}

void SimValModule::Plot(const string& title, 
			const float& var1, 
			const int& nb1,
			const float& low1, 
			const float& high1, 
			const float& var2, 
			const int& nb2,
			const float& low2, 
			const float& high2, float weight) { 
  vector<HepObj*>::iterator result = find_if(_histList.begin(), _histList.end(),
					     NameEqual(title));
  HepHist2D* h;
  if (result == _histList.end()) {
    h = &fileManager()->hist2D(title,nb1,low1,high1,nb2,low1,high2);
    _histList.push_back(h);
  }
  else { 
    h = &fileManager()->retrieveHist2D(title);
  }
  h->accumulate(var1,var2,weight);
}




