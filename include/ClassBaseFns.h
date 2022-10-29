#include "fns.h"
//ClassBaseFns.h -- use this file for all auton functions
#ifndef NO_MAKE
list<function<void()>> threadFns;
#else
extern list<function<void()>> threadFns;
#endif

#define toStr(a)  #a


//All the functions that are based on classes
namespace ClassFns {
  #ifdef NO_MAKE
  void spinRoller();
  #else 
  void spinRoller() {
    bool targetRed = wc.isRed();
    bool isRed = rachetColor.hue() > 300 && rachetColor.hue() < 60;
    bool lastRed = isRed;
    while(1){
      if(isRed != lastRed && isRed == targetRed){
        break;
      }
      intake.spin(fwd, 100);
      auto hue = rachetColor.hue();
      lastRed = isRed;
      isRed = hue > 300 && hue < 60;
      if(!isRed){
        if(abs(hue - 240) < 60){
          isRed = false;
        }
        else {
          //Ambiguous colors, set isRed to lastRed to prevent exit
          isRed = lastRed;
        }
      }

      s(50);
    }
    intake.stop(hold);
    //Give the system some time to clear momentum
    s(300);
  }
  #endif
}
