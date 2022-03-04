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
  bool goalClipUse = false;

  FN_WITH_APA_SIG_NO_ARG(allowVisionAlign)
    wc.setExitDist(16.0);
  }
  FN_WITH_APA_SIG_NO_ARG(clipGoal)
    goalHolder->close();
    if(goalClipUse){
      conveyer.ready = true;
      wc.setGoalBack();
    }
    //wc.addGoal();
  }
  FN_WITH_APA_SIG_NO_ARG(unclipGoal)
    goalHolder->open();
    if(goalClipUse){
      conveyer.ready = false;
      wc.removeGoalBack();
    }
    //wc.removeGoal();
  }
  FN_WITH_APA_SIG_NO_ARG(clipLiftGoal)
    liftGoalHolder.open();
    if(goalClipUse){
      wc.setGoalFront();
    }
  }
  FN_WITH_APA_SIG_NO_ARG(unclipLiftGoal)
    liftGoalHolder.close();
    if(goalClipUse){
      wc.removeGoalFront();
    }
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
  FN_WITH_APA_SIG(setLiftIndex, int)
    liftCtrllr.setIndex(amnt);
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
    PID straight = wc.getSlave();
    bool exit = false;
    bool angleInRange = false;
    int t = 0;
    double lastTilt = tiltAngle;
    double tiltDeriv = 0;
    int timeIn = 0;
    straight.setTarget(0);
    while(1){
      tiltDeriv = tiltAngle - lastTilt;
      double normAngle = 0;
      if(abs(wc.botAngle()) < 90){
        normAngle = wc.botAngle();
      }
      else {
        normAngle = posNeg180(wc.botAngle() + 180);
      }
      double extra = -straight.getVal(normAngle) / 4.0;
      if(abs(tiltAngle) > 5 && abs(tiltDeriv) < 0.3 && abs(wc.botPos().y) > 7){
        if(tiltAngle > 0){
          wc.moveLeft(70 + extra, fwd);
          wc.moveRight(70 - extra, fwd);
        }
        else {
          wc.moveLeft(70 - extra, reverse);
          wc.moveRight(70 + extra, reverse);
        }
        timeIn = 0;
      }
      else {
        wc.hardBrake();
        timeIn += 10;
      }
      if(timeIn > 1000){
        break;
      }
      lastTilt = tiltAngle;
      s(10);
    }
    

  }

}
