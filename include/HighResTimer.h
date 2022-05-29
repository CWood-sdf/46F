#ifndef HIGH_RES_TIMER_H
#define HIGH_RES_TIMER_H
#include "vex.h"
class TimerHighRes {
  uint64_t lastTime;
public:
  TimerHighRes(){
    lastTime = timer::systemHighResolution();
  }
  //In microsec
  uint64_t time(){
    return timer::systemHighResolution() - lastTime;
  }
  
  uint64_t time(timeUnits u){
    switch(u){
      case vex::timeUnits::msec:
        return time() / 1000;
      case vex::timeUnits::sec:
        return time() / 1000000;
    }
  }

  //In seconds
  uint64_t value(){
    return (time()) / 1000000;
  }

  operator uint64_t(){
    return value();
  }

  void clear(){
    lastTime = timer::systemHighResolution();
  }

  void reset(){
    clear();
  }
};
#endif