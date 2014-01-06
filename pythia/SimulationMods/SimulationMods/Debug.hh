#ifndef SIM_DEBUG_INCLUDED
#define SIM_DEBUG_INCLUDED 1

#include <iostream>

namespace sim {

extern int sim_debug_level;

inline void setDebugLevel(int level) { sim_debug_level=level; }
inline int  getDebugLevel() { return sim_debug_level; }

#ifndef SIM_NO_DEBUG
#define debug_out(LEVEL) if(LEVEL<=getDebugLevel()) std::cout
#define debug_endl std::endl
#else
#define debug_out(LEVEL) /*
#define debug_std::endl */
#endif

}

#endif

