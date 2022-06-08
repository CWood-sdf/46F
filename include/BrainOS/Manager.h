#include "BrainOS/BosFn.h"
namespace bos {
#ifndef NO_MAKE
  LinkedList<BosFn> bosFns;
#else
  extern LinkedList<BosFn> bosFns;
#endif
}