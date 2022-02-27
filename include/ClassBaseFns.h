#include "fns.h"
//ClassBaseFns.h -- use this file for all auton functions
list<function<void()>> threadFns;

//Sleeps the current thread
void ts (int time){
  this_thread::sleep_for(time);
}
#define toStr(a)  #a

//This macro does a lot
//First it makes a prototype function with the passed name and accepting 2 arguments:
//        an int and a boolean that defaults to true
//It then defines the function with a beginning body that writes to the APA 
//      if the 'write' arg is true
string parseInt(vision::signature& c){
  return "";
}
#define FN_WITH_APA_SIG(name, tp1) \
  void name (tp1, bool = true);       \
  void name (tp1 amnt, bool write) {       \
    string msg = #name;       \
    msg += "(";                 \
    if(write)                             \
    cout << msg << parseInt(amnt) << string(");\n");

#define FN_WITH_APA_SIG_ARG(name, tp1, arg, str) \
  void name (tp1, arg, bool = true);       \
  void name (tp1 amnt, arg, bool write) {       \
    string msg = #name;       \
    msg += "(";                 \
    string msg2 = parseInt(amnt) + ", ";             \
    msg2 += str;                  \
    if(write)                             \
    cout << msg << msg2 << string(");\n");
#define FN_WITH_APA_SIG_NO_ARG(name) \
  void name (bool = true);       \
  void name (bool write) {       \
    string msg = #name;       \
    if(write)                             \
    cout << msg << string("();\n");
//


//All the functions that are based on classes
namespace ClassFns {
  
  FN_WITH_APA_SIG_NO_ARG(allowVisionAlign)
    wc.setExitDist(16.0);
  }
  FN_WITH_APA_SIG_NO_ARG(clipGoal)
    goalHolder->close();
    //wc.addGoal();
  }
  FN_WITH_APA_SIG_NO_ARG(unclipGoal)
    goalHolder->open();
    //wc.removeGoal();
  }
  FN_WITH_APA_SIG_NO_ARG(clipLiftGoal)
    liftGoalHolder.close();
  }
  FN_WITH_APA_SIG_NO_ARG(unclipLiftGoal)
    liftGoalHolder.open();
  }
  FN_WITH_APA_SIG(visionAlign, vision::signature&)
    //Highest speed desired == 30
    //Highest err ~= 12
    PID align = PID(2.5, 0, 1.666);
    PID driveFwd = wc.getCtrl();
    align.setTarget(0);
    driveFwd.setTarget(0);
    while(!true){
      
      double fwd = driveFwd.getVal(wc.botPos().dist2D(wc.getLastTarget()));
      PVector l = basicAlign.closest(amnt);
      double alignVal = align.getVal(l.x);
      if(frontCounter.pressing()){
        clipLiftGoal();
        s(200);
        break;
      }
      wc.moveLeft(fwd + alignVal, directionType::fwd);
      wc.moveRight(fwd - alignVal, directionType::fwd);
    }
    wc.hardBrake();
  }
  

  FN_WITH_APA_SIG_NO_ARG(raiseLiftByOne)
    liftCtrllr.done = false;
    liftCtrllr.freePress();
    liftCtrllr.addIndex();
  }
  FN_WITH_APA_SIG_NO_ARG(lowerLiftByOne)
    liftCtrllr.done = false;
    liftCtrllr.freePress();
    liftCtrllr.subIndex();
  }
  FN_WITH_APA_SIG_NO_ARG(waitForLiftFinish)
    //cout << liftCtrllr.isDone() << endl;
    while(!liftCtrllr.isDone()){
      //cout << liftCtrllr.isDone() << endl;
      s(10);
    }
  }
  FN_WITH_APA_SIG_NO_ARG(raiseLiftByOneWait)
    liftCtrllr.done = false;
    liftCtrllr.freePress();
    liftCtrllr.addIndex();
    waitForLiftFinish(false);
  }
  FN_WITH_APA_SIG_NO_ARG(lowerLiftByOneWait)
    liftCtrllr.done = false;
    liftCtrllr.freePress();
    liftCtrllr.subIndex();
    waitForLiftFinish(false);
  }
  
  
  FN_WITH_APA_SIG_NO_ARG(useLineGoalDetect)
    threadFns.push_back([](){
      while(1){
        if(frontCounter.pressing()){
          clipLiftGoal();
          s(300);
          if(wc.isMoving()){
            wc.forceEarlyExit();
          }
          break;
        }
        s(10);
      }
    });
  }
  FN_WITH_APA_SIG_NO_ARG(useLineGoalDetectNoExit)
    threadFns.push_back([](){
      while(1){
        if(frontCounter.pressing()){
          clipLiftGoal();
          break;
        }
        s(10);
      }
    });
  }
  //Needed for prog skills (maybe)
  FN_WITH_APA_SIG_NO_ARG(balanceBot)
    PID speedGetter = PID(1.5, 0.001, 0.1);
    bool exit = false;
    bool angleInRange = false;
    int t = 0;
    
    while(!exit){
      double angle = tiltAngle;
      angleInRange = abs(angle) < 3.0;
      if(!angleInRange){
        t = 0;
      } else {
        if(t++ >= 100){
          exit = true;
        }
      }
      double speed = speedGetter.getVal(angle);
      double angleTravel;// = wc.posNeg180(glblBotAngle - angle);
      if(speed < 0){
        angleTravel = posNeg180(-90 - glblBotAngle);
      } else {
        angleTravel = posNeg180(90 - glblBotAngle);
      }
      
      s(1);
      
    }
    

  }

}
