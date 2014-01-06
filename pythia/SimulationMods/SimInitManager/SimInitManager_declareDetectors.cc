#include "SimulationMods/SimInitManager.hh"
#include "geant_i/Geant3GeometryInterface.hh"
#include "ErrorLogger_i/gERRLOG.hh"
#include "GeometryBase/CdfDetector.hh"
#include "GeometryBase/Menu/AbsDetectorNodeSelector.hh"

#include <string>
using std::string;

void SimInitManager::_declareDetectors() {

  APPCommand** command;
  APPListIterator<APPCommand*> i(*_detectorMenu->commands());
  sim::AbsDetectorNodeSelector* selector;
  while (command=i() ) {
    selector=dynamic_cast<sim::AbsDetectorNodeSelector*>(*command);
    if (selector && selector->isUserEnabled() && selector->value()) {
      _geometryInterface->declareDetectorTree(selector->node(),
                                              selector->subsystemName()+
                                              string(" simulation"));
    }
  }
}

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
