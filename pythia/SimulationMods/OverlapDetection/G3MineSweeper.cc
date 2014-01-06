
#include "AbsEnv/AbsEnv.hh"

#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Random/RanluxEngine.h"
#include "CLHEP/Random/RandFlat.h"

#include "HepTuple/HepRootFileManager.h"
#include "HepTuple/HepHBookFileManager.h"
#include "HepTuple/HepNtuple.h"
#include "HepTuple/Column.h"
#include "HepTuple/Block.h"

#include "Edm/ConstHandle.hh"
#include "Edm/EventRecord.hh"
#include "SimulationObjects/HEPG_StorableBank.hh"
#include "SimulationObjects/MVTX_StorableBank.hh"

#include "SimulationMods/G3MineSweeper.hh"
#include "Extrapolator/G3XExtrapolator.hh"

#include <algorithm>
#include <functional>

static inline int sign(int x) { return (x>0 ? 1: -1);}

G3MineSweeper::G3MineSweeper(const char* theName, const char* theDescription )
                  :HepHistModule(theName, theDescription),
		   _gridPosX("gridPosX", this, 0., -1000., 1000.),
		   _gridPosY("gridPosY", this, 0., -1000., 1000.),
		   _gridPosZ("gridPosZ", this, 0., -1000., 1000.),
		   _gridRMin("gridRMin", this, 1., 0., 1000.),
		   _gridRMax("gridRMax", this, 5., 0., 1000.),
		   _gridLength("gridLength", this, 5., 0., 1000.),
		   _gridRStep("gridRStep", this, 1., 0., 100.),
		   _gridPhiStep("gridPhiStep", this, 360., 0., 360.),
		   _gridZStep("gridZStep", this, 1.0, 0., 500.),
		   _probeRadius("probeRadius", this, 5., 0., 100.),
		   _probePhiStep("probePhiStep", this, 360., 0., 360.),
		   _probeCosThetaStep("probeCosThetaStep", this, 1.0, 0., 3.),
		   _scanReportOverlaps("scanReportOverlaps", this, true),
		   _scanCheckOnlyHierarchy("scanCheckOnlyHierarchy", this, false),
		   _scanMergeAdjacentSteps("scanMergeAdjacentSteps", this, true),
		   _scanClearEdge("scanClearEdge", this, true),
		   _scanPointTolerance("scanPointTolerance", this, 0.0025, 0., 0.1),
		   _scanMaxNumberOfOverlaps("scanMaxNumberOfOverlaps", this, 5, 0, 100),
		   _verbose("verbose", this, false),
		   _extrap(new G3XExtrapolator(0)),
		   _grid(new G3MineSweeper::Grid()),
		   _probe(new G3MineSweeper::Probe()),
		   _currentScan(new G3MineSweeper::Scan()) {

  commands()->append(&_verbose);

  _GridMenu.initialize("GridMenu", this);
  _GridMenu.initTitle("Grid Specifications");
  commands()->append(&_GridMenu);
  _gridPosX.addDescription("\t X position of the center of the grid [cm]");
  _gridPosY.addDescription("\t Y position of the center of the grid [cm]");
  _gridPosZ.addDescription("\t Z position of the center of the grid [cm]");
  _gridRMin.addDescription("\t Inner radius of the grid [cm]");
  _gridRMax.addDescription("\t Outer radius of the grid [cm]");
  _gridLength.addDescription("\t Length of the grid [cm]");
  _gridRStep.addDescription("\t Granularity in radius of the grid [cm]");
  _gridPhiStep.addDescription("\t Granularity in azimuthal angle of the grid [deg]");
  _gridZStep.addDescription("\t Granularity in Z of the grid [cm]");
  _GridMenu.commands()->append(&_gridPosX);
  _GridMenu.commands()->append(&_gridPosY);
  _GridMenu.commands()->append(&_gridPosZ);
  _GridMenu.commands()->append(&_gridRMin);
  _GridMenu.commands()->append(&_gridRMax);
  _GridMenu.commands()->append(&_gridLength);
  _GridMenu.commands()->append(&_gridRStep);
  _GridMenu.commands()->append(&_gridPhiStep);
  _GridMenu.commands()->append(&_gridZStep);

  _ProbeMenu.initialize("ProbeMenu", this);
  _ProbeMenu.initTitle("Probe Specifications");
  commands()->append(&_ProbeMenu);
  _probeRadius.addDescription("\t Radius of the sphere [cm]");
  _probePhiStep.addDescription("\t Step in azimuth for ray tracing in the sphere [deg]");
  _probeCosThetaStep.addDescription("\t Step in cosine of polar angle for ray tracing in the sphere");
  _ProbeMenu.commands()->append(&_probeRadius);
  _ProbeMenu.commands()->append(&_probePhiStep);
  _ProbeMenu.commands()->append(&_probeCosThetaStep);

  _ScanMenu.initialize("ScanMenu", this);
  _ScanMenu.initTitle("Scanning Specifications and Tolerances");
  commands()->append(&_ScanMenu);
  _scanReportOverlaps.addDescription("\t Report overlaps while scanning");
  _scanCheckOnlyHierarchy.addDescription("\t Check only the consistency of the volume hierarchy");
  _scanMergeAdjacentSteps.addDescription("\t Merge adjacent steps in the same volume");
  _scanClearEdge.addDescription("\t Avoid stopping on boundary for the last step");
  _scanPointTolerance.addDescription("\t Tolerance for comparison of entry/exit points [cm]");
  _scanMaxNumberOfOverlaps.addDescription("\t Stop the scan if more than N overlaps are found");
  _ScanMenu.commands()->append(&_scanReportOverlaps);
  _ScanMenu.commands()->append(&_scanCheckOnlyHierarchy);
  _ScanMenu.commands()->append(&_scanMergeAdjacentSteps);
  _ScanMenu.commands()->append(&_scanPointTolerance);
  _ScanMenu.commands()->append(&_scanMaxNumberOfOverlaps);

}


G3MineSweeper::~G3MineSweeper(){
  delete _extrap;
  delete _grid;
  delete _probe;
  delete _currentScan;
}

AppResult G3MineSweeper::beginJob( AbsEvent* aJob ){

  // setup extrapolator: geantino
  _extrap->setParticleType(9999904);

  // setup grid
  _grid->setSize(_gridRMin.value(),_gridRMax.value(),_gridLength.value());
  _grid->setPosition(Hep3Vector(_gridPosX.value(),_gridPosY.value(),_gridPosZ.value()));
  _grid->setGranularity(_gridRStep.value(),_gridPhiStep.value()*M_PI/180.,_gridZStep.value());
  _grid->initialize();

  // setup probe
  _probe->setRadius(_probeRadius.value());
  _probe->setPhiStep(_probePhiStep.value()*M_PI/180.);
  _probe->setCosThetaStep(_probeCosThetaStep.value());
  _probe->initialize();

  // setup scan
  _currentScan->setReport(_scanReportOverlaps.value());
  _currentScan->setMerging(_scanMergeAdjacentSteps.value());
  _currentScan->setPointTolerance(_scanPointTolerance.value());

  _numberOfOverlaps=0;

  return AppResult::OK;
}


AppResult G3MineSweeper::endJob( AbsEvent* aJob ){
  std::cout<<">>>>>>>>>>>>>>>>>>>>"<<std::endl;
  std::cout<<">>>>>>>>>>>>>>>>>>>> G3MineSweeper: "<<_numberOfOverlaps<<" paths were found with at least one overlap"<<std::endl;
  std::cout<<">>>>>>>>>>>>>>>>>>>>"<<std::endl;
  return AppResult::OK;
}


AppResult G3MineSweeper::event( AbsEvent* anEvent ){

  // iterate over the positions on the grid:
  while(_grid->nextNode()&&_numberOfOverlaps<_scanMaxNumberOfOverlaps.value()) {

    if(_verbose.value()) std::cout<<"* Node position: R,Phi,Z= " 
                                  << _grid->nodePosition().rho() 
                                  << " " << _grid->nodePosition().phi() 
                                  << " " << _grid->nodePosition().z() 
                                  <<std::endl;

    _probe->alignOnGrid(_grid->nodePosition());

    // iterate over the ray positions in the sphere:
    while(_probe->nextRay()&&_numberOfOverlaps<_scanMaxNumberOfOverlaps.value()) {

      Hep3Vector pointA = _grid->nodePosition();
      Hep3Vector pointB = _probe->rayPosition();
      Hep3Vector momentum1 = pointB - pointA;
      momentum1.setMag(5000.); // stiff track
      Hep3Vector momentum2 = -momentum1;

      _currentScan->initialize(pointA, pointB);

      if(_verbose.value()) std::cout<<"  * Extrap to "<<pointB
                                    <<" (R="<<pointB.rho()<<")"
				    <<std::endl;

      // do inner to outer tracking first:
      if(_verbose.value()) std::cout<<"    --> Pass 1: Inner to outer tracking:"<<std::endl;
      _extrap->setInitialPosition(pointA);
      if(pointA.rho()<=pointB.rho()) {
        _extrap->setInitialMomentum(momentum1);
        if(_verbose.value()) {
	  std::cout<<"    Forward extrapolation,   ";
          std::cout<<"    Momentum= "<<momentum1<<std::endl;
	}
        _extrap->extrapolateToRadius(pointB.rho());
      } else {
        _extrap->setInitialMomentum(momentum2);
        if(_verbose.value()) {
	  std::cout<<"    Backward extrapolation,   ";
          std::cout<<"    Momentum= "<<momentum2<<std::endl;
	}
        _extrap->extrapolateDownToRadius(pointB.rho());
      }

      std::vector<G3XExtrapolator::stepinfo>* matList1 = _extrap->getMaterialList();
      _currentScan->storeFirstPass(matList1);

      // do outer to inner tracking:
      if(_verbose.value()) std::cout<<"    --> Pass 2: Outer to inner tracking:"<<std::endl;
      _extrap->setInitialPosition(pointB);
      if(pointA.rho()>pointB.rho()) {
        _extrap->setInitialMomentum(momentum1);
        if(_verbose.value()) {
	  std::cout<<"    Forward extrapolation,   ";
          std::cout<<"    Momentum= "<<momentum1<<std::endl;
	}
        _extrap->extrapolateToRadius(pointA.rho());
      } else {
        _extrap->setInitialMomentum(momentum2);
        if(_verbose.value()) {
	  std::cout<<"    Backward extrapolation,   ";
          std::cout<<"    Momentum= "<<momentum2<<std::endl;
	}
        _extrap->extrapolateDownToRadius(pointA.rho());
      }

      std::vector<G3XExtrapolator::stepinfo>* matList2 = _extrap->getMaterialList();
      _currentScan->storeSecondPass(matList2);

      // remove the one or two last step(s) if we might be stopping on a volume boundary:
      if(_scanClearEdge.value()) _currentScan->clearEdge();

      // compare the two paths to find overlaps:
      if(_verbose.value()) _currentScan->dump();
      if(_verbose.value()) std::cout<<"    --> Overlap detection:"<<std::endl;
      int severity = _currentScan->detectOverlaps();
      if(severity) {
	if(_verbose.value()) std::cout<<"       comparison failed. Severity= "<<severity<<std::endl;
	_numberOfOverlaps++;
      } else {
	if(_verbose.value()) std::cout<<"       OK."<<std::endl;
      }

    }

  }

 return AppResult::OK;
}


AppModule* G3MineSweeper::clone(const char* cloneName){
  return new G3MineSweeper(cloneName,"this module is a clone");
}

void G3MineSweeper::Grid::setPosition(const Hep3Vector& pos) {
  _gridTransform = HepTranslate3D(pos);
  std::cout<<"Grid origin="<<_gridTransform*HepPoint3D(0,0,0)<<std::endl;
}

void G3MineSweeper::Grid::setSize(double rMin, double rMax, double l) {
  _rMin=rMin;
  _rMax=max(rMin,rMax);
  _length=l;
  std::cout<<"Grid size:"<<std::endl;
  std::cout<<" rMin= "<<_rMin<<std::endl;
  std::cout<<" rMax= "<<_rMax<<std::endl;
  std::cout<<" length= "<<_length<<std::endl;
}

void G3MineSweeper::Grid::setGranularity(double dR, double dPhi, double dZ) {
  if(dR<=0.) dR=max(1.,_rMax-_rMin);
  if(dPhi<=0.) dPhi=2.*M_PI;
  if(dZ<=0.) dZ=max(1.,_length);
  _dR=dR;
  _dPhi=dPhi;
  _dZ=dZ;
  std::cout<<"Grid granularity:"<<std::endl;
  std::cout<<" dR= "<<_dR<<std::endl;
  std::cout<<" dPhi= "<<_dPhi<<std::endl;
  std::cout<<" dZ= "<<_dZ<<std::endl;
}

void G3MineSweeper::Grid::initialize() {
  _nodeIndex=0;
  _nodePhiIndex=0;
  _nodeRIndex=0;
  _nodeZIndex=0;
  _nodeRNumber = static_cast<int>((_rMax-_rMin)/_dR)+1;
  _nodePhiNumber = static_cast<int>(2.*M_PI/_dPhi);
  _nodeZNumber = static_cast<int>(_length/_dZ)+1;
  _numberOfNodes = _nodeRNumber*_nodePhiNumber*_nodeZNumber;
  std::cout<<"Grid initialized with "<<_numberOfNodes<<" nodes:"<<std::endl;
  std::cout<<" "<<_nodeRNumber<<" steps in R"<<std::endl;
  std::cout<<" "<<_nodePhiNumber<<" steps in Phi"<<std::endl;
  std::cout<<" "<<_nodeZNumber<<" steps in Z"<<std::endl;
  std::cout<<" 1st position: local= "<<_localNodePosition
           <<" global= "<< this->nodePosition() << std::endl;
}

bool G3MineSweeper::Grid::nextNode() {
  bool nextOK = true;
  _nodeIndex++;
  if(_nodeIndex>1) {
    if(_nodePhiIndex<_nodePhiNumber-1) {
      _nodePhiIndex++;
    } else {
      _nodePhiIndex=0;
      if(_nodeRIndex<_nodeRNumber-1) {
	_nodeRIndex++;
      } else {
	_nodeRIndex=0;
	if(_nodeZIndex<_nodeZNumber-1) {
	  _nodeZIndex++;
	} else {
	  nextOK = false;
	}
      }
    }
  }
  double currentPhi = -M_PI + _nodePhiIndex*_dPhi;
  double currentR = _rMin + _nodeRIndex*_dR;
  double currentZ = -_length/2. + _nodeZIndex*_dZ;
  _localNodePosition.setRhoPhiZ(currentR,currentPhi,currentZ);
  return nextOK;
}

const Hep3Vector& G3MineSweeper::Grid::nodePosition() {
  _globalNodePosition = _gridTransform*HepVector3D(_localNodePosition);
  return _globalNodePosition;
}

void G3MineSweeper::Probe::initialize() {
  _rayPhiNumber = static_cast<int>(2.*M_PI/_dPhi);
  _rayCosThetaNumber = static_cast<int>(2./_dCosTheta)+1;
  _numberOfRays = _rayPhiNumber*_rayCosThetaNumber;
  std::cout<<"Probe initialized for "<<_numberOfRays<<" rays:"<<std::endl;
  std::cout<<" "<<_rayPhiNumber<<" steps in Phi"<<std::endl;
  std::cout<<" "<<_rayCosThetaNumber<<" steps in CosTheta"<<std::endl;
}

void G3MineSweeper::Probe::alignOnGrid(const Hep3Vector& pos) {
  _rayIndex=0;
  _rayPhiIndex=0;
  _rayCosThetaIndex=0;
  _probeTransform = HepTranslate3D(pos);
}

bool G3MineSweeper::Probe::nextRay() {
  bool nextOK = true;
  _rayIndex++;
  if(_rayIndex>1) {
    if(_rayPhiIndex<_rayPhiNumber-1) {
      _rayPhiIndex++;
    } else {
      _rayPhiIndex=0;
      if(_rayCosThetaIndex<_rayCosThetaNumber-1) {
        _rayCosThetaIndex++;
      } else {
        nextOK = false;
      }
    }
  }
  double currentPhi = -M_PI + _rayPhiIndex*_dPhi;
  double currentCosTheta = -1. + _rayCosThetaIndex*_dCosTheta;
  _localRay.setRThetaPhi(_radius,acos(currentCosTheta),currentPhi);
  return nextOK;
}

const Hep3Vector& G3MineSweeper::Probe::rayPosition() {
  _globalRay = _probeTransform*HepVector3D(_localRay);
  return _globalRay;
}

void G3MineSweeper::Scan::initialize(const Hep3Vector& pt1, const Hep3Vector& pt2) {
  _pointA = pt1;
  _pointB = pt2;
  _firstPassSteps.clear();
  _secondPassSteps.clear();
}

void G3MineSweeper::Scan::storeFirstPass(std::vector<G3XExtrapolator::stepinfo>* slist) {
  for(std::vector<G3XExtrapolator::stepinfo>::const_iterator 
      istep = slist->begin(); istep != slist->end(); ++istep) {
    Step currentStep = Step((*istep).name,(*istep).level,(*istep).entrpoint,(*istep).exitpoint);
    _firstPassSteps.push_back(currentStep);
  }
  if(_mergeAdjacentSteps) {
  for(int iref=0; iref<_firstPassSteps.size(); iref++) {
    std::string refName = _firstPassSteps[iref].volumeName();
    int refLevel = _firstPassSteps[iref].volumeLevel();
    int irefs = iref;
    bool adjacent = true;
    for(int inxt=iref+1; inxt<_firstPassSteps.size(); inxt++) {
      std::string nxtName = _firstPassSteps[inxt].volumeName();
      int nxtLevel = _firstPassSteps[inxt].volumeLevel();
      if(nxtName==refName&&nxtLevel==refLevel&&adjacent) {
	_firstPassSteps[inxt].entryPoint(_firstPassSteps[irefs].entryPoint());
	for(int ibck=irefs; ibck<inxt; ibck++)
	_firstPassSteps[ibck].exitPoint(_firstPassSteps[inxt].exitPoint());
	iref=inxt;
      } else {
	adjacent = false;
      }
    }
  }
  std::vector<Step>::iterator pos = std::unique( _firstPassSteps.begin(), _firstPassSteps.end());
  _firstPassSteps.erase(pos, _firstPassSteps.end());
  }
}

void G3MineSweeper::Scan::storeSecondPass(std::vector<G3XExtrapolator::stepinfo>* slist) {
  for(std::vector<G3XExtrapolator::stepinfo>::const_iterator 
      istep = slist->begin(); istep != slist->end(); ++istep) {
    Step currentStep = Step((*istep).name,(*istep).level,(*istep).entrpoint,(*istep).exitpoint);
    _secondPassSteps.push_back(currentStep);
  }
  if(_mergeAdjacentSteps) {
  for(int iref=0; iref<_secondPassSteps.size(); iref++) {
    std::string refName = _secondPassSteps[iref].volumeName();
    int refLevel = _secondPassSteps[iref].volumeLevel();
    int irefs = iref;
    bool adjacent = true;
    for(int inxt=iref+1; inxt<_secondPassSteps.size(); inxt++) {
      std::string nxtName = _secondPassSteps[inxt].volumeName();
      int nxtLevel = _secondPassSteps[inxt].volumeLevel();
      if(nxtName==refName&&nxtLevel==refLevel&&adjacent) {
	_secondPassSteps[inxt].entryPoint(_secondPassSteps[irefs].entryPoint());
	for(int ibck=irefs; ibck<inxt; ibck++)
	_secondPassSteps[ibck].exitPoint(_secondPassSteps[inxt].exitPoint());
	iref=inxt;
      } else {
	adjacent = false;
      }
    }
  }
  std::vector<Step>::iterator pos = std::unique( _secondPassSteps.begin(), _secondPassSteps.end());
  _secondPassSteps.erase(pos, _secondPassSteps.end());
  }
}

int G3MineSweeper::Scan::detectOverlaps() {
  int severity=0;
  int nstep1 = _firstPassSteps.size();
  int nstep2 = _secondPassSteps.size();
  if(nstep1!=nstep2) {
    severity=max(severity,9);
    std::cout<<"    >>>>>>>>>>>>>>>> Overlap detected, severity=9 "<<std::endl;
    std::cout<<"    >>>>>>>>>>>>>>>> different number of steps in pass 1 and pass 2"<<std::endl;
    dump();
    std::cout<<"    <<<<<<<<<<<<<<<<"<<std::endl;
  } else {
    if(nstep1+nstep2>0) {
      for(int istep=0; istep<nstep1; istep++) {
        Step step1 = _firstPassSteps[istep];
        Step step2 = _secondPassSteps[nstep2-istep-1];
	if(step1.volumeName()==step2.volumeName()) {
	  if(step1.volumeLevel()==step2.volumeLevel()) {
	    double d1 = step1.entryPoint().distance(step2.exitPoint());
	    double d2 = step1.exitPoint().distance(step2.entryPoint());
	    if(d1>_tolerance||d2>_tolerance) {
              severity=max(severity,6);
              std::cout<<"    >>>>>>>>>>>>>>>> Overlap detected, severity=6 "<<std::endl;
              std::cout<<"    >>>>>>>>>>>>>>>> collision depth is "<<max(d1,d2)<<" cm: "
	               <<step1.entryPoint()-step2.exitPoint()
	               <<" "
	               <<step1.exitPoint()-step2.entryPoint()
		       <<std::endl;
              dump(istep+1);
	      std::cout<<"    <<<<<<<<<<<<<<<<"<<std::endl;
	    }
	  } else {
            severity=max(severity,7);
            std::cout<<"    >>>>>>>>>>>>>>>> Overlap detected, severity=7 "<<std::endl;
            std::cout<<"    >>>>>>>>>>>>>>>> different volume levels in pass 1 and pass 2"<<std::endl;
            dump(istep+1);
	    std::cout<<"    <<<<<<<<<<<<<<<<"<<std::endl;
	  }
	} else {
          severity=max(severity,8);
          std::cout<<"    >>>>>>>>>>>>>>>> Overlap detected, severity=8 "<<std::endl;
          std::cout<<"    >>>>>>>>>>>>>>>> different volume names in pass 1 and pass 2"<<std::endl;
          dump(istep+1);
	  std::cout<<"    <<<<<<<<<<<<<<<<"<<std::endl;
        }
      }
    }
 }
 return severity;
}

void G3MineSweeper::Scan::dump() {
  std::cout << " - point A : XYZ= " << _pointA 
            << " RPhiZ= ("<<_pointA.rho()<<","<<_pointA.phi()<<","<<_pointA.z()
	    << ")" <<std::endl;
  std::cout << " - point B : XYZ= " << _pointB 
            << " RPhiZ= ("<<_pointB.rho()<<","<<_pointB.phi()<<","<<_pointB.z()
	    << ")" <<std::endl;
  std::cout<<" - pass 1 (all steps):"<<std::endl;
  for(int iref=0; iref<_firstPassSteps.size(); iref++) {
    Hep3Vector entry = Hep3Vector(_firstPassSteps[iref].entryPoint());
    Hep3Vector exit = Hep3Vector(_firstPassSteps[iref].exitPoint());
    std::cout<<"Step "<<iref+1<<" : vol= "<<_firstPassSteps[iref].volumeName()
                              <<" lvl= "<<_firstPassSteps[iref].volumeLevel()
             <<" entry: XYZ= " << entry
             << " RPhiZ= ("<<entry.rho()<<","<<entry.phi()<<","<<entry.z()<<")"
	     <<" exit: XYZ= "<< exit
             << " RPhiZ= ("<<exit.rho()<<","<<exit.phi()<<","<<exit.z()<<")"
			      <<std::endl;
  }
  std::cout<<" - pass 2 (all steps):"<<std::endl;
  for(int iref=0; iref<_secondPassSteps.size(); iref++) {
    Hep3Vector entry = Hep3Vector(_secondPassSteps[iref].entryPoint());
    Hep3Vector exit = Hep3Vector(_secondPassSteps[iref].exitPoint());
    std::cout<<"Step "<<iref+1<<" : vol= "<<_secondPassSteps[iref].volumeName()
                              <<" lvl= "<<_secondPassSteps[iref].volumeLevel()
             <<" entry: XYZ= " << entry
             << " RPhiZ= ("<<entry.rho()<<","<<entry.phi()<<","<<entry.z()<<")"
	     <<" exit: XYZ= "<< exit
             << " RPhiZ= ("<<exit.rho()<<","<<exit.phi()<<","<<exit.z()<<")"
			      <<std::endl;
  }
}

void G3MineSweeper::Scan::dump(int iref) {
  std::cout << " - point A : XYZ= " << _pointA 
            << " RPhiZ= ("<<_pointA.rho()<<","<<_pointA.phi()<<","<<_pointA.z()
	    << ")" <<std::endl;
  std::cout << " - point B : XYZ= " << _pointB 
            << " RPhiZ= ("<<_pointB.rho()<<","<<_pointB.phi()<<","<<_pointB.z()
	    << ")" <<std::endl;
  if(iref>0&&iref<=_firstPassSteps.size()) {
    Hep3Vector entry = Hep3Vector(_firstPassSteps[iref-1].entryPoint());
    Hep3Vector exit = Hep3Vector(_firstPassSteps[iref-1].exitPoint());
    std::cout<<" - pass 1 (step "<<iref<<"): ";
    std::cout<<" vol= "<<_firstPassSteps[iref-1].volumeName()
             <<" lvl= "<<_firstPassSteps[iref-1].volumeLevel()
             <<" entry: XYZ= " << entry
             << " RPhiZ= ("<<entry.rho()<<","<<entry.phi()<<","<<entry.z()<<")"
	     <<" exit: XYZ= "<< exit
             << " RPhiZ= ("<<exit.rho()<<","<<exit.phi()<<","<<exit.z()<<")"
	     <<std::endl;
  }
  iref = _secondPassSteps.size()-iref+1;
  if(iref>0&&iref<=_secondPassSteps.size()) {
    Hep3Vector entry = Hep3Vector(_secondPassSteps[iref-1].entryPoint());
    Hep3Vector exit = Hep3Vector(_secondPassSteps[iref-1].exitPoint());
    std::cout<<" - pass 2 (step "<<iref<<"): ";
    std::cout<<" vol= "<<_secondPassSteps[iref-1].volumeName()
             <<" lvl= "<<_secondPassSteps[iref-1].volumeLevel()
             <<" entry: XYZ= " << entry
             << " RPhiZ= ("<<entry.rho()<<","<<entry.phi()<<","<<entry.z()<<")"
	     <<" exit: XYZ= "<< exit
             << " RPhiZ= ("<<exit.rho()<<","<<exit.phi()<<","<<exit.z()<<")"
	     <<std::endl;
  }
}

void G3MineSweeper::Scan::clearEdge() {
  if( (_secondPassSteps.size()-_firstPassSteps.size()) == 1 ) {
    _secondPassSteps.erase(_secondPassSteps.begin());
  } else {
    if( (_firstPassSteps.size()-_secondPassSteps.size()) == 1 ) {
      _firstPassSteps.pop_back();
    }
  }
}

