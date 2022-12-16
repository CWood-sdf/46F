#include "BrainOS/BosFn.h"
namespace bos
{
#ifndef NO_MAKE
  LinkedList<BosFn> bosFns;
  void setScreen(BosFn fn)
  {
    // auto &oldCurrent = bosFns.getCurrent();
    // while (1)
    // {
    //   if (bosFns.getCurrent().val == fn)
    //   {
    //     return;
    //   }
    //   bosFns.moveCurrentNext();
    //   if (bosFns.getCurrent() == bosFns.getEnd())
    //   {
    //     break;
    //   }
    // }
    // bosFns.setCurrent(oldCurrent);
  }
#else
  extern LinkedList<BosFn> bosFns;
  void setScreen(BosFn fn);
#endif
}