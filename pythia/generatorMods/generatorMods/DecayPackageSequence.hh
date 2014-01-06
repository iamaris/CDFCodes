#ifndef DECAYPACKAGESEQUENCE_HH
#define DECAYPACKAGESEQUENCE_HH

#include "Framework/APPSequence.hh"

class DecayPackageSequence: public APPSequence 
{
public:
  DecayPackageSequence();

  virtual ~DecayPackageSequence();
  static const char* name;
};

#endif
