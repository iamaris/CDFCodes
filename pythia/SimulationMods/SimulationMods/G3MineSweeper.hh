#include "Framework/AbsParmGeneral.hh"
#include "Framework/AbsParmBool.hh"
#include "Framework/APPFramework.hh"
#include "FrameMods/HepHistModule.hh"
#include "FrameUtil/APPCommand.hh"
#include "FrameUtil/AbsInterp.hh"
#include "Extrapolator/G3XExtrapolator.hh"

class HepPoint3D;
class Hep3Vector;

class G3MineSweeper : public HepHistModule {

public:

  class Grid {
  public:
    Grid() {};
    ~Grid() {};
    void setPosition(const Hep3Vector& pos);
    void setSize(double rMin, double rMax, double length);
    void setGranularity(double dR, double dPhi, double dZ);
    void initialize();
    bool nextNode();
    const Hep3Vector& nodePosition();
  protected:
    double _rMin;
    double _rMax;
    double _length;
    double _dR;
    double _dPhi; // radians
    double _dZ;
    int _nodeRIndex;
    int _nodePhiIndex;
    int _nodeZIndex;
    int _nodeRNumber;
    int _nodePhiNumber;
    int _nodeZNumber;
    HepTransform3D _gridTransform; // global coordinates
    Hep3Vector     _localNodePosition;
    Hep3Vector     _globalNodePosition;
    int _nodeIndex;
    int _numberOfNodes;
  };

  class Probe {
  public:
    Probe() {};
    ~Probe() {};
    void alignOnGrid(const Hep3Vector& pos);
    void setRadius(double r) { _radius=r; }
    void setPhiStep(double step) { _dPhi=step; }
    void setCosThetaStep(double step) { _dCosTheta=step; }
    void initialize();
    bool nextRay();
    const Hep3Vector& rayPosition();
  protected:
    double _radius;
    double _dPhi; // radians
    double _dCosTheta;
    int _rayPhiIndex;
    int _rayPhiNumber;
    int _rayCosThetaIndex;
    int _rayCosThetaNumber;
    HepTransform3D _probeTransform; // global coordinates
    Hep3Vector     _localRay;
    Hep3Vector     _globalRay;
    int _rayIndex;
    int _numberOfRays;
  };

  class Step {
  public:
    Step() {};
    Step(std::string name, int lvl, HepPoint3D entry, HepPoint3D exit) :
      _volumeName(name), _volumeLevel(lvl), _entryPoint(entry), _exitPoint(exit) {};
    ~Step() {};
    std::string volumeName() const { return _volumeName; }
    bool matchVolumeName(std::string vol) const { return (_volumeName==vol); }
    int volumeLevel() const { return _volumeLevel; }
    const HepPoint3D& entryPoint() const { return _entryPoint; }
    const HepPoint3D& exitPoint() const { return _exitPoint; }
    void entryPoint(const HepPoint3D& pos) { _entryPoint=pos; }
    void exitPoint(const HepPoint3D& pos) { _exitPoint=pos; }
    int operator==(const Step& stp) const { return ( stp.volumeName()==_volumeName &&
                                                     stp.volumeLevel()==_volumeLevel &&
                                                     stp.entryPoint()==_entryPoint &&
                                                     stp.exitPoint()==_exitPoint ); }
  protected:
    std::string _volumeName;
    int _volumeLevel;
    HepPoint3D _entryPoint;
    HepPoint3D _exitPoint;
  };

  class Scan {
  public:
    Scan() {};
    ~Scan() {};
    void setReport(bool flag) { _report=flag; }
    void setMerging(bool flag) { _mergeAdjacentSteps=flag; }
    void setPointTolerance(double tol) { _tolerance=tol; }
    void storeFirstPass(std::vector<G3XExtrapolator::stepinfo>* slist);
    void storeSecondPass(std::vector<G3XExtrapolator::stepinfo>* slist);
    int detectOverlaps();
    void dump();
    void dump(int istep);
    void initialize(const Hep3Vector& pt1, const Hep3Vector& pt2);
    void clearEdge();
  protected:
    bool _report;
    bool _mergeAdjacentSteps;
    double _tolerance;
    Hep3Vector _pointA;
    Hep3Vector _pointB;
    std::vector<Step> _firstPassSteps;
    std::vector<Step> _secondPassSteps;
  };

  class Overlap {
  public:
    Overlap() {};
    ~Overlap() {};
  protected:
  };
  
  // Constructors
  G3MineSweeper(const char* theName = "G3MineSweeper", 
	   const char* theDescription = "Check for overlaps");
  
  // Destructor
  virtual ~G3MineSweeper();
  
  // Operations
  virtual AppResult      beginJob( AbsEvent* aJob );
  virtual AppResult      endJob( AbsEvent* aJob );
  virtual AppResult      event( AbsEvent* event );
  virtual AppModule*     clone( const char* cloneName );
  
protected:
  AbsParmGeneral<bool>   _verbose;

  APPMenu                _GridMenu;
  AbsParmGeneral<double> _gridPosX;
  AbsParmGeneral<double> _gridPosY;
  AbsParmGeneral<double> _gridPosZ;
  AbsParmGeneral<double> _gridRMin;
  AbsParmGeneral<double> _gridRMax;
  AbsParmGeneral<double> _gridLength;
  AbsParmGeneral<double> _gridRStep;
  AbsParmGeneral<double> _gridPhiStep; // degrees
  AbsParmGeneral<double> _gridZStep;

  APPMenu                _ProbeMenu;
  AbsParmGeneral<double> _probeRadius;
  AbsParmGeneral<double> _probePhiStep; // degrees
  AbsParmGeneral<double> _probeCosThetaStep;

  APPMenu                _ScanMenu;
  AbsParmGeneral<bool>   _scanReportOverlaps;
  AbsParmGeneral<bool>   _scanCheckOnlyHierarchy;
  AbsParmGeneral<bool>   _scanMergeAdjacentSteps;
  AbsParmGeneral<bool>   _scanClearEdge;
  AbsParmGeneral<double> _scanPointTolerance;
  AbsParmGeneral<int>    _scanMaxNumberOfOverlaps;

  G3XExtrapolator*       _extrap;
  Grid*                  _grid;
  Probe*                 _probe;
  Scan*                  _currentScan;

  int _numberOfOverlaps;

};
