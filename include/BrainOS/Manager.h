#include "BrainOS/BosFn.h"
namespace bos {
#ifndef NO_MAKE
  LinkedList<BosFn> bosFns;
  void setScreen(BosFn fn){
    auto& oldCurrent = bosFns.getCurrent();
    for(auto f : bosFns){
      if(f == fn){
        //Don't need to set current here because the loop will do it
        return;
      }
    }
    //reset current
    bosFns.setCurrent(oldCurrent);
  }
#else
  extern LinkedList<BosFn> bosFns;
  void setScreen(BosFn fn);
#endif
}