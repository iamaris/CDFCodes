#ifndef Mdc2TriggerBits_hh
#define Mdc2TriggerBits_hh

struct Mdc2TriggerBits {
  struct L2 {
    enum { None        = -1,
	   HptElectron = 0,
	   HptMuon     = 1,
	   DiElectron  = 2,
	   DiMuon      = 3
    };
  };
};
#endif
