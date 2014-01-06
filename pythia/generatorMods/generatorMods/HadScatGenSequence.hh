#ifndef HADSCATGENSEQUENCE_HH
#define HADSCATGENSEQUENCE_HH

#include "Framework/APPSequence.hh"

class HardScatGenSequence: public APPSequence 
{
public:
  HardScatGenSequence();

  virtual ~HardScatGenSequence();

  static const char* name;
};

#endif
