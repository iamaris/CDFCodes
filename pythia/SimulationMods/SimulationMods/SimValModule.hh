#ifndef SIMVALMODULE_INCLUDED
#define SIMVALMODULE_INCLUDED 1

// Purpose: plots validation plots for simulation
// $Id: SimValModule.hh,v 1.1 2002/05/14 00:51:42 lena Exp $

#include "FrameMods/HepHistModule.hh"
#include <vector>
#include <string>

class SimValModule : public HepHistModule {
public :

	SimValModule(  const char* const theName = "SimValModule",
								const char* const theDescription = "Simulation Validation Module");
	virtual ~SimValModule();
  virtual AppResult  beginJob( AbsEvent* aJob );
  virtual AppResult  beginRun( AbsEvent* aRun );
  virtual AppResult  event( AbsEvent* anEvent );
  virtual AppResult  other( AbsEvent* anEvent );
  virtual AppResult  endRun( AbsEvent* aRun );
  virtual AppResult  endJob( AbsEvent* aJob );
  virtual AppResult  abortJob( AbsEvent* aJob );
  const char* rcsId( void ) const;

  // one dimensional histogram plotting & booking
  void Plot(const std::string&, 
	    const float& var, 
	    const int& nbins, 
	    const float& low, 
	    const float& high, float weight=1);

  // two dimensional histogram plotting & booking
  void Plot(const std::string&, 
	    const float& var1, 
	    const int& nbins1,
	    const float& low1, 
	    const float& high1, 
	    const float& var2, 
	    const int& nbins2,
	    const float& low2, 
	    const float& high2, float weight=1);

private:

	  std::vector<HepObj*> _histList;

};
#endif 
