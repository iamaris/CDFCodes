#ifndef SIM_SETUP_INCLUDED
#define SIM_SETUP_INCLUDED 1

class SimSetup 
{
public:
  
  static SimSetup* Instance(); 
  
  bool getFastTrack() { return _fastTrack; }
  void setFastTrack( bool param );
  bool getTraceSecForClc() { return _traceSecForClc; }
  void setTraceSecForClc( bool param );

  long getAltRandomSeed1() { return _altRandomSeed1; }
  void setAltRandomSeed1(long seed) {  _altRandomSeed1 = seed; }
  long getAltRandomSeed2() { return _altRandomSeed2; }
  void setAltRandomSeed2(long seed) {  _altRandomSeed2 = seed; }
  long getRandomSeed() { return _randomSeed; }
  void setRandomSeed(long seed) {  _randomSeed = seed; }

  // default geant seeds
  static long DefaultAltRandomSeed1;
  static long DefaultAltRandomSeed2;

  // default simulation seed ( used by CLHEP engine )
  static long DefaultRandomSeed;

protected:
  
  static SimSetup* _instance;
  struct Cleaner { ~Cleaner(); };

  friend struct Cleaner;
  
  bool _fastTrack;
  bool _traceSecForClc;
  long _altRandomSeed1;
  long _altRandomSeed2;
  long _randomSeed;
  
  SimSetup();
  ~SimSetup();
  
}; 

#endif // SIM_SETUP_INCLUDED

