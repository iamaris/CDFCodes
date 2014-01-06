#ifndef __FILLRUN1BANKS_HH
#define __FILLRUN1BANKS_HH
//-----------------------------------------------------------------------------
// Description:
// Since we are not supporting generators in the Run 1 environment any more, 
// we need to be able to take Run 2 Edm files and run them through the binary 
// Edm_Ybos2SeqRoot and make files that the Run 1 framework can read.
//
// This module is used to create the LRID and EVCL banks that are required
// by the Run 1 Framework (Analysis_Control)
//
//-----------------------------------------------------------------------------
#include "Framework/APPModule.hh"

class FillRun1BanksModule : public AppModule {

public:
  
  FillRun1BanksModule(const char* theName        = "FillRun1Banks", 
		 const char* theDescription = "Fill LRID and EVCL" );
  
  virtual AppResult      beginJob( AbsEvent* aJob );
  virtual AppResult      beginRun( AbsEvent* aRun );
  virtual AppResult      event( AbsEvent* anEvent );
  virtual AppResult      endRun( AbsEvent* aRun );
  virtual AppResult      endJob( AbsEvent* aJob );
  virtual AppResult      abortJob( AbsEvent* aJob );

  virtual AppModule*     clone(const char* cloneName);

  virtual ~FillRun1BanksModule( );
    
private:
};

#endif 
