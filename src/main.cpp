/******************************
TODO:
  - Lift indices
  
*******************************/
#include "updaters.h"
using namespace ClassFns;
using namespace vex;
competition Competition;
//Returns true if a button is pressing at the start, but doesn't return until button releaseed
bool isPressing(controller::button& btn){
  if(btn.pressing()){
    while(btn.pressing()){
      s(10);
    }
    return true;
  }
  
  return false;
}
//Similar to isPressing(controller::button&), but only does it if a joystick axis is going in a certain direction
bool isPressing(controller::axis& joystick, int mult){
  if(abs(joystick.value()) > 50 && joystick.value() * mult > 1){
    while(abs(joystick.value()) > 50 && joystick.value() * mult > 1){
      s(10);
    }
    //cout << "  ";
    return true;
  }
  
  return false;
}

//Moves bot backwards for the given amount of time
void backwards(int t){
  Left.spin(vex::reverse, 60, pct);
  Right.spin(vex::reverse, 60, pct);
  s(t);
  Left.stop();
  Right.stop();
}
//Moves bot backwards at the given speed for the given amount of time 
void backwardsSlow(int t, int sp = 40){
  Left.spin(vex::reverse, sp, pct);
  Right.spin(vex::reverse, sp, pct);
  s(t);
  Left.stop();
  Right.stop();
}
void fwds(int t){
  Left.spin(fwd, 60, pct);
  Right.spin(fwd, 60, pct);
  s(t);
  Left.stop(hold);
  Right.stop(hold);
  
}
void fwdsSlow(int t, double sp = 30){
  Left.spin(fwd, sp, pct);
  Right.spin(fwd, sp, pct);
  s(t);
  Left.stop(hold);
  Right.stop(hold);
  
}
void turnRight(int t){
  Left.spin(fwd, 60, pct);
  Right.spin(vex::reverse, 60, pct);
  s(t);
  Left.stop(hold);
  Right.stop(hold);
}
void turnLeft(int t){
  Left.spin(vex::reverse, 60, pct);
  Right.spin(fwd, 60, pct);
  s(t);
  Left.stop(hold);
  Right.stop(hold);
}
//Autonomous Stuff {

//Map order skillsOrSide - 3, centerGoals - 4, autonGoals - 2

void awp(){
  wc.estimateStartPos(PVector(57.52249730893436, -42.504198062432735), 193.05920344456405);
  clipGoal();
  raiseLiftByOne();
  wc.followPath({PVector(34.44, -50.94), PVector(34.27, 28.45)});
  unclipGoal();
  wc.backwardsFollow({PVector(38.0, 48.08)});
  clipGoal();
  wc.driveTo(-6.71, 48.43);
  wc.backwardsFollow({PVector(42.71, 33.96)});

}
void leftSide1GoalRush(){
  wc.estimateStartPos(PVector(60, -24), 90);
  unclipLiftGoal();
  // wc.backwardsFollow({{0, 0}});
  wc.setMaxAcc(5000000);
  wc.setMaxDeAcc(50000000);
  unclipLiftGoal();
  wc.preventTurn();
  useLineGoalDetect();
  wc.driveTo(-12, -36);
  clipLiftGoal();
  wc.setMaxAcc(300);
  wc.setMaxDeAcc(300);
  // wc.backInto(48, -36);
  // wc.estimateStartPos(PVector(9.987513455328326, -36.82066738428418), 271.33692142088273);
  
  wc.backwardsFollow({PVector(37.54, -60.58), PVector(52.35, -35.4)});
  raiseLiftByOne();
  clipGoal();
  wc.prevStopExit();
  wc.driveTo(36, -36);

}
void rightSide2GoalRush(){
  lowerLiftByOne();
  lowerLiftByOne();
  lowerLiftByOne();
  lowerLiftByOne();
  wc.estimateStartPos(PVector(60, 24), 90);
  unclipLiftGoal();
  // wc.backwardsFollow({{0, 0}});
  wc.setMaxAcc(5000000);
  wc.setMaxDeAcc(50000000);
  unclipLiftGoal();
  wc.preventTurn();
  useLineGoalDetect();

  wc.driveTo(-12, 36);
  wc.setMaxAcc(300);
  wc.setMaxDeAcc(300);
  wc.prevStopExit();
  wc.backwardsFollow({PVector(38, 36)});
  raiseLiftByOne();
  wc.turnTo(45);
  unclipLiftGoal();
  wc.backwardsFollow({PVector(38, 50)});
  clipGoal();
  s(500);
  useLineGoalDetect();
  wc.driveTo(-4.67, -5.67);
  wc.driveTo(30.19, -4.27);

}
void rightCenterGoalRush(){
  wc.estimateStartPos(PVector(60, 24), 90);
  unclipLiftGoal();
  wc.setMaxAcc(5000000);
  wc.setMaxDeAcc(50000000);
  unclipLiftGoal();
  wc.preventTurn();
  useLineGoalDetect();
  wc.driveTo(0, 4.0);
  raiseLiftByOne();
  wc.backwardsFollow({PVector(35.25, 25.81), PVector(35.82, 47.74)});
  clipGoal();

}
void rightSide1GoalRush(){
  lowerLiftByOne();
  lowerLiftByOne();
  lowerLiftByOne();
  lowerLiftByOne();
  wc.estimateStartPos(PVector(60, 24), 90);
  unclipLiftGoal();
  // wc.backwardsFollow({{0, 0}});
  wc.setMaxAcc(5000000);
  wc.setMaxDeAcc(50000000);
  unclipLiftGoal();
  wc.preventTurn();
  useLineGoalDetect();
  wc.driveTo(-12, 36);
  clipLiftGoal();
  
  wc.setMaxAcc(300);
  wc.setMaxDeAcc(300);
  wc.backwardsFollow({PVector(38, 36)});
  raiseLiftByOne();
  wc.backwardsFollow({PVector(40, 50)});
  clipGoal();
  s(1000);
  wc.setSpeedLimit(50);
  wc.driveTo(-8.09, 47.57);
  wc.backwardsFollow({PVector(48.95, 34.48)});


}
void leftSide2GoalRush(){
  
}

void rightSideGoal(){
  
}
void addPids(){
  cout << "Turn P" << endl;
  while(1){
    if(isPressing(Greg.ButtonA)){
      wc.addTurnPid(0, 0, 0);
      break;
    }
    if(isPressing(Greg.ButtonB)){
      wc.addTurnPid(0.1, 0, 0.0);
      break;
    }
  }
  cout << "Turn D" << endl;
  while(1){
    if(isPressing(Greg.ButtonA)){
      break;
    }
    if(isPressing(Greg.ButtonB)){
      wc.addTurnPid(0.0, 0, 0.05);
      break;
    }
  }
  
  cout << "Done" << endl;
}

void skills(){
  // conveyer.ready = true;
  wc.estimateStartPos(PVector(58.598242530755684, -43.638664323374336), 188.66783831282953);
  clipGoal();
  
  useLineGoalDetectNoExit();
  threadFns.push_back([](){
    s(1000);
    raiseLiftByOne();
  });
  wc.driveTo(-24.57, -36.1);
  // wc.estimateStartPos(PVector(-27.166256590509672, -35.20281195079085), 270.9173989455184);
  //Slow for rings
  thread k = thread([](){
    while(abs(wc.botPos().y) > 12){
      s(10);
    }
    unclipGoal();
  });
  wc.setSpeedLimit(50);
  wc.driveTo(-25.47, -4.05);
  wc.setSpeedLimit(100);
  //Up to top
  raiseLiftByOne();
  raiseLiftByOne();
  raiseLiftByOne();
  raiseLiftByOneWait();
  //To platform
  wc.driveTo(-40.85, -4.05);
  //Level platform
  lowerLiftByOne();
  lowerLiftByOne();
  lowerLiftByOne();
  s(800);
  //Drop
  unclipLiftGoal();
  //Back to normal
  raiseLiftByOne();
  raiseLiftByOneWait();
  //Turn to general goal direction
  wc.turnTo(144.78);
  lowerLiftByOne();
  lowerLiftByOne();
  lowerLiftByOne();
  lowerLiftByOne();
  waitForLiftFinish();
  useLineGoalDetect();
  wc.driveTo(-24.07, -24.73);
  clipLiftGoal();
  s(800);
  raiseLiftByOne();
  raiseLiftByOne();
  raiseLiftByOne();
  waitForLiftFinish();
  //To platform
  wc.followPath({PVector(-31.66, -5.39), PVector(-44.88, -2.58)});
  unclipLiftGoal();
  lowerLiftByOne();
  lowerLiftByOne();
  s(500);
  unclipLiftGoal();
  raiseLiftByOne();
  raiseLiftByOne();
  s(400);
  // wc.estimateStartPos(PVector(-42.63198594024605, -13.550790861159918), 294.95957820738136);
  wc.turnTo(wc.botPos().angleTo({0, 0}));
  useLineGoalDetect();
  unclipLiftGoal();
  lowerLiftByOne();
  lowerLiftByOne();
  lowerLiftByOne();
  lowerLiftByOneWait();
  //To tall goal
  wc.setSpeedLimit(40);
  wc.driveTo(3.20, -0.05);
  clipLiftGoal();
  s(500);
  raiseLiftByOne();
  raiseLiftByOne();
  raiseLiftByOne();
  //To platform
  wc.setSpeedLimit(100);
  wc.driveTo(-42.41, -12.33);
  lowerLiftByOne();
  lowerLiftByOne();
  lowerLiftByOne();
  s(1200);
  unclipLiftGoal();
  raiseLiftByOne();
  raiseLiftByOne();
  raiseLiftByOne();
  raiseLiftByOne();
  s(500);
  //
  wc.backwardsFollow({PVector(-32.72, -47.85)});
  clipGoal();
  lowerLiftByOne();
  lowerLiftByOne();
  lowerLiftByOne();
  lowerLiftByOne();
  useLineGoalDetect();
  wc.followPath({PVector(-36, 0), PVector(0, 42.12)});
  raiseLiftByOne();
  // wc.estimateStartPos(PVector(-10.294551845342706, 28.628471001757475), 52.007029876977136);
  raiseLiftByOne();
  raiseLiftByOne();
  raiseLiftByOne();
  raiseLiftByOne();

  wc.followPath({PVector(8.82, 37.62), PVector(59.16, 37.06)});
  wc.turnTo(181.46);
  lowerLiftByOne();
  lowerLiftByOne();
  lowerLiftByOne();
  lowerLiftByOneWait();
  wc.driveTo(59.16, 9.78);
  raiseLiftByOneWait();
  balanceBot();


}
void autonInit(){
  cout << "Auton Init" << endl;
  unclipGoal();
  unclipLiftGoal();
  liftCtrllr.enable();
  goalClipUse = true;
  cout << "Auton Init Done" << endl;

}
void autonomous(){
  autonInit();
  auto startTime = Brain.Timer.system();
  skills();
  cout << (Brain.Timer.system() - startTime) / 1000 << endl;
}

//}
#define sensitivity 12

//Drivercontrol + automation {
class ButtonLatch {
  bool isPressing = false;
  int state = 0;
  int stateLim;
  controller::button& b;
public:
  ButtonLatch(controller::button& b, int stateLim = 2) : stateLim(stateLim), b(b){

  }
  ButtonLatch() = delete;
  bool pressing(){
    if(b.pressing() && !isPressing){
      state++;
      if(state == stateLim + 1){
        state = 1;
      }
      
      isPressing = true;
      return true;
    }
    else if(!b.pressing()){
      isPressing = false;
    }
    return false;
  }
  int getState(){
    return state;
  }
};
void drivercontrol (){
  ButtonLatch R1Latch = ButtonLatch(Greg.ButtonR1);
  ButtonLatch R2Latch = ButtonLatch(Greg.ButtonR2);
  ButtonLatch BLatch = ButtonLatch(Greg.ButtonB);
  static bool driveReversed = false;
  //int wheelsMove = 0;
  liftCtrllr.disable();
  // backLiftCtrllr.disable();
  KillThread::killAll();
  goalHolder = new pneumatics(Brain.ThreeWirePort.H);
  while(1){
    //Drive control, uses quotient/square for smoothness
    double Y1 = abs(Greg.Axis2.value()) > sensitivity ? Greg.Axis2.value() : 0;
    double Y2 = abs(Greg.Axis3.value()) > sensitivity ? Greg.Axis3.value() : 0;
    // Y1 /= 10;
    // Y1 *= Y1;
    // Y2 /= 10;
    // Y2 *= Y2;
    // Y1 *= Greg.Axis2.value() != 0 ? Greg.Axis2.value() / abs(Greg.Axis2.value()) : 1;
    // Y2 *= Greg.Axis3.value() != 0 ? Greg.Axis3.value() / abs(Greg.Axis3.value()) : 1;
    
    double s1 = Y2;
    double s2 = Y1;
    if(driveReversed){
      FL.spin(vex::reverse, s2, pct);
      ML.spin(vex::reverse, s2, pct);
      BL.spin(vex::reverse, s2, pct);
      FR.spin(vex::reverse, s1, pct);
      MR.spin(vex::reverse, s1, pct);
      BR.spin(vex::reverse, s1, pct);
    }
    else {
      FL.spin(fwd, s1, pct);
      ML.spin(fwd, s1, pct);
      BL.spin(fwd, s1, pct);
      FR.spin(fwd, s2, pct);
      MR.spin(fwd, s2, pct);
      BR.spin(fwd, s2, pct);
    }

    if(BLatch.pressing()){
      driveReversed = !driveReversed;
    }
    if(R1Latch.pressing()){
      liftGoalHolder.set(!liftGoalHolder.value());
    }
    if(R2Latch.pressing()){
      conveyer.ready = (bool)goalHolder->value();
      goalHolder->set(!goalHolder->value());
    }
    if(Greg.ButtonL2.pressing()){
      //Move the lift to the next index
      // liftCtrllr.addIndex();
      liftMot.spin(fwd);
    } else if(Greg.ButtonL1.pressing()){
      //Move the lift to the previous index
      // liftCtrllr.subIndex();
      liftMot.spin(vex::reverse);
    } else {
      //Allow the lift index to be changed
      // liftCtrllr.freePress();
      liftMot.stop(hold);
    }
    // if(Greg.ButtonA.pressing()){
    //   backLiftCtrllr.enable();
    //   for(int i = 0; i < 4; i++){
    //     lowerBackLiftByOne();
    //   }
    // }
    // else if(Greg.ButtonX.pressing()){
    //   backLiftCtrllr.enable();
    //   while(backLiftCtrllr.getIndex() != 2){
    //     raiseBackLiftByOne();
    //   }
    // }
    // else if(backLiftCtrllr.disabled()){
    //   flapConveyer.stop(hold);
    // }

    // //Manual conveyer override
    // if(Greg.ButtonUp.pressing()){
    //   conveyer.controller = true;
    //   spinFlaps();
    // }
    // else if(Greg.ButtonDown.pressing()){
    //   conveyer.controller = true;
    //   reverseFlaps();
    // }
    // else if(Greg.ButtonLeft.pressing()){
    //   conveyer.controller = true;
    //   stopFlaps();
    // }
    // else {
    //   //Return conveyer to program control
    //   stopFlaps();
    // }
    // cout << GPS.xPosition(inches) << ", " << GPS.yPosition(inches) << ", " << GPS.heading() << endl;
    s(100);
    
  }
}

void automation(){
  int canMove = 20;
  LinkedList<double> positions;
  int convNoBackTime = 0;
  int convBackTime = 0;
  LinkedList<double> convLastPos = { 0, 0, 0, 0 };
  while(1){
    if(!liftCtrllr.disabled()){
      if(!liftCtrllr.isUp && liftMot.position(deg) < liftCtrllr.getPosition()){
        
        liftMot.spin(fwd, 12, volt);
        if(canMove < 0){
          if(abs(positions.getBase() - liftMot.position(deg)) < 10){
            // cout << "Transferring to High speed" << endl;
            liftMot.spin(fwd, 12, volt);
          }
        }

        liftCtrllr.done = false;
      }
      else if(liftCtrllr.isUp && liftMot.position(deg) > liftCtrllr.getPosition()){
        liftMot.spin(vex::reverse, 12, volt);
        if(canMove < 0){
          if(abs(positions.getBase() - liftMot.position(deg)) < 10){
            // cout << "Transferring to High speed" << endl;
            liftMot.spin(vex::reverse, 12, volt);
          }
        }
        liftCtrllr.done = false;
      }
      else {

        liftCtrllr.done = !liftCtrllr.prevented();;
        liftMot.stop(hold);
      }
    }
    if(conveyer.ready && liftMot.position(deg) < -50){
      if(convBackTime < 0){
        flaps.spin(fwd, 80, pct);
      }
      else {
        flaps.spin(reverse, 100, pct);
      }
      if(abs(flapConveyer.rotation(deg) - convLastPos.getBase()) < 5 && convNoBackTime < 0){
        convBackTime = 300;
        convNoBackTime = 500;
      }
      
      
    }
    else {
      flaps.stop(hold);
      convNoBackTime = 500;
    }
    convBackTime -= liftCtrllr.getSleepTime();
    convNoBackTime -= liftCtrllr.getSleepTime();
    convLastPos.push_back(flapConveyer.rotation(deg));
    convLastPos.popBase();
    positions.push_back(liftMot.position(deg));
    if(canMove < 0){
      positions.popBase();
    }
    canMove--;
    //lift.spin(fwd, liftCtrllr.getPosition());
    liftCtrllr.sleep();
  }
}
//}

//APA {

void programWrite(bool start){
  int numArg = 0;
  bool threaded = false;
  //Set drive mode to auton to allow APA writing
  //driveMode = "auton";
  wc.coastBrake();
  if(start)
    autonInit();
  //Add some space
  cout << "\n\n" << endl;
  while(1){
    //Set the number args
    //left shift 2 bits, add 3 -- 0b11
    if(isPressing(Greg.ButtonUp)){
      numArg <<= 2;
      numArg += 3;
      cout << "// n:" << numArg << endl;
    } 
    //left shift 2 bits, add 2 -- 0b10
    else if(isPressing(Greg.ButtonRight)){
      numArg <<= 2;
      numArg += 2;
      cout << "// n:" << numArg << endl;
    }
    //left shift 2 bits, add 1 -- 0b01
    else if(isPressing(Greg.ButtonLeft)){
      numArg <<= 2;
      numArg += 1;
      cout << "// n:" << numArg << endl;
    }
    //left shift 2 bits, add 0 -- 0b00
    else if(isPressing(Greg.ButtonDown)){
      numArg <<= 2;
      numArg += 0;
      cout << "// n:" << numArg << endl;
    }
    //Resets numArg to 0
    else if(isPressing(Greg.Axis1, -1)){
      numArg = 0;
      cout << "// n:" << numArg << endl;
    }
    //Inverses the value of threaded: true => false, false => true
    else if(isPressing(Greg.ButtonA)){
      threaded = !threaded;
      cout << "// t:" << threaded << endl;
    }
    //Driving controls
    //driveTo:            Axis2 -           1
    //turnTo:             Axis1 -           1
    //backInto:           Axis2 -           -1
    //driveFwd:           Axis3 -           -1
    //followPath:         Axis3 -           1
    //backwardsFollow:    Axis1 -           -1
    else if(isPressing(Greg.Axis2, 1)){
      bool exit = false;
      while(!exit){
        exit = true;
        //Set driveTo
        if(isPressing(Greg.Axis2, 1)){
          cout << "wc.driveTo(" << VECT_XY(wc.botPos());
          if(wc.isOmniDir){
            cout << ", " << wc.botAngle() << ");" << endl;
          }
          else {
            cout << ");" << endl;
          }
        } 
        //Set turnTo
        else if(isPressing(Greg.Axis1, 1)){
          cout << "wc.turnTo(" << wc.botAngle() << ");" << endl;
        }
        //Set backInto
        else if(isPressing(Greg.Axis2, -1)){
          cout << "wc.backInto(" << VECT_XY(positioner.getPos());
          if(wc.isOmniDir){
            cout << ", " << wc.botAngle() << ");" << endl;
          }
          else {
            cout << ");" << endl;
          }
        }
        //Set driveFwd
        else if(isPressing(Greg.Axis3, -1)){

          cout << "//start" << endl;
          
          PVector startPos = wc.botPos();
          while(!isPressing(Greg.Axis2, -1)){
            s(100);
          }
          auto v = wc.botPos();
          double finalDist = startPos.dist2D(v);
          cout << "wc.driveFwd(" << finalDist << ");" << endl;
        }
        //Set followPath
        else if(isPressing(Greg.Axis3, 1)){
          cout << "//start" << endl;
          PVector startPos = wc.botPos();
          cout << "wc.followPath({PVector(" << VECT_XY(startPos) << ")";
          while(!isPressing(Greg.Axis2, -1)){
            if(isPressing(Greg.Axis2, 1)){
              cout << ", PVector(" << VECT_XY(wc.botPos()) << ")";
            }
            s(100);
          }
          if(wc.isOmniDir){
            
            
            cout << "}, " << wc.botAngle() << ");" << endl;
          } else {
            
            
            cout << "});" << endl;
          }
        }
        //Set backwardsFollow
        else if(isPressing(Greg.Axis1, -1)){
          cout << "//start" << endl;
          PVector startPos = wc.botPos();
          cout << "wc.backwardsFollow({PVector(" << VECT_XY(startPos) << ")";
          while(!isPressing(Greg.Axis2, -1)){
            if(isPressing(Greg.Axis2, 1)){
              cout << ", PVector(" << VECT_XY(wc.botPos()) << ")";
            }
            s(100);
          }
          if(wc.isOmniDir){
            
            
            cout << "}, " << wc.botAngle() << ");" << endl;
          } else {
            
            cout << "});" << endl;
          }
        }
        else {
          exit = false;
        }
        s(10);
      }
    }
    //All following functions are executed live, while writing
    
    //
    else if(isPressing(Greg.Axis2, -1)){
      
    }
    //
    else if(isPressing(Greg.Axis1, 1)){
      
    }
    //
    else if(isPressing(Greg.ButtonR1)){
      numArg = 0;
    }
    //
    else if(isPressing(Greg.ButtonR2)){
      if(threaded){
      }
      else {
      }
    }
    //
    else if(isPressing(Greg.ButtonL1)){
      if(threaded){
        raiseLiftByOne();
      }
      else {
        raiseLiftByOneWait();
      }
    }
    //
    else if(isPressing(Greg.ButtonL2)){
      if(threaded){
        lowerLiftByOne();
      }
      else {
        lowerLiftByOneWait();
      }
    }
    //
    else if(isPressing(Greg.Axis3, -1)){
      
    }
    //
    else if(isPressing(Greg.Axis3, 1)){
      waitForLiftFinish();
    }
    //
    else if(isPressing(Greg.Axis4, 1)){

    }
    //
    else if(isPressing(Greg.Axis4, -1)){
      
    }
    //
    else if(isPressing(Greg.ButtonB)){
      
    }
    //Execute specified special function
    else if(isPressing(Greg.ButtonX)){
      specFns[numArg](true);
      numArg = 0;
    }
    //Add Comment
    else if(isPressing(Greg.ButtonY)){
      if(!start){
        return;
      }
    }
    //Sleep for a time to give the brain a break
    s(100);
  }
  
}
void programWrite(void* in){
  programWrite(*(bool*)in);
}


//}

//Brain Drawing Stuff {

void printVars() {
  Brain.Screen.waitForRefresh();
  Brain.Screen.clearScreen(black);
  Brain.Screen.setFillColor(black);
  Brain.Screen.printAt(10, 20, (string("glblBotAngle: ") + toCcp(glblBotAngle)).data());
  Brain.Screen.printAt(10, 40, (string("botAngles.y: ") + toCcp(botAngles.y)).data());
  Brain.Screen.printAt(10, 60, (string("botAngles.z: ") + toCcp(botAngles.z)).data());
  Brain.Screen.printAt(10, 80, (string("tiltAngle: ") + toCcp(tiltAngle)).data());
  Brain.Screen.printAt(10, 100, (string("wc.botPos().x: ") + toCcp(wc.botPos().x)).data());
  Brain.Screen.printAt(10, 120, (string("wc.botPos().y: ") + toCcp(wc.botPos().y)).data());
}
void displayTilt(){
  s(100);
  Brain.Screen.waitForRefresh();
  Brain.Screen.clearScreen(black);
  PVector start = PVector(160, 100);
  PVector v = PVector(0,80);
  v.rotate(tiltAngle);
  v += start;
  Brain.Screen.setFillColor(red);
  Brain.Screen.drawCircle(start.x, start.y, 5);
  Brain.Screen.drawCircle(v.x, v.y, 5);
}
void drawPath(){
  s(100);
  PVector off = PVector(100, 100);
  Brain.Screen.waitForRefresh();
  Brain.Screen.clearScreen(black);
  
  if(wc.drawArr){
    Brain.Screen.setFillColor(blue);
    for(auto v : wc.path){
      v *= 2.0;
      v += off;
      Brain.Screen.drawCircle(v.x, v.y, 2);
    }
  } else {
    Brain.Screen.setFillColor(black);
    Brain.Screen.printAt(40, 40, "No Path");
  }
}
const color grey = color(100, 100, 100);
const color lightGreen = color(100, 255, 100);

const color darkGreen = color(ClrDarkGreen);



void brainOS() {
  int state = 1;		
  int maxState = 3; 
  Button screenLeft = Button(Brain, 10, BRAIN_HEIGHT - 60, 30, 30, black, "<");		
  Button screenRight = Button(Brain, BRAIN_WIDTH - 40, BRAIN_HEIGHT - 60, 30, 30, black, ">");		
  while (1) {		
    switch(state) {
      case 1:
        printVars();
        break;
      case 2:
        displayTilt();
        break;
      case 3:
        drawPath();
        break;
      default:
        windowsLoader();
        break;
    }
    screenLeft.draw();		
    screenRight.draw();	
    if (screenLeft.clicked() && state > 0) {	
      state--;	
    }	
    else if (screenRight.clicked() && state < maxState) {	
      state++;	
    }	
  }	
}
//}
bool init = false;
int main() {
  // testMotorConfiguration();
  thread initThread = thread([](){
    unclipGoal();
    clipLiftGoal();
    gyroInit(angler);
    gyroInit(GPS);
    init = true;
  });

  // updateBotAngle();
  // startTilt = botAngles.z;

  // //thread conveyer = thread(conveyerControl);
  // thread updatePos = thread(trackPos);
  thread gpsUpdate = thread(updateSharePos);

  //Make a thread to execute some auton tasks concurrently
  thread otherThreads = thread(executeThreads);
  
  thread automationThread = thread(automation);

  // //WINDOWS LOADER: YEET BURGER
  thread loader = thread(brainOS);
  // s(1000);
  while(!init){
    s(100);
  }
  s(500);
  clipLiftGoal();
  unclipGoal();
  // autonInit();
  cout << "Init Done" << endl;
  Competition.autonomous(autonomous);
  // Competition.drivercontrol(drivercontrol);
  // wc.backwardsFollow({PVector(0, 0)});
  // testMotorConfiguration();
  while(!Greg.ButtonA.pressing()){
    s(100);
    // cout << frontCounter.pressing() << ", " << frontCounter.rawData() << endl;
  }
  Greg.rumble(".");
  // wc.estimateStartPos({-36, -36}, 0);
  // wc.followPath({PVector(-36, 0), PVector(0, 30.12)});
  autonomous();
  // wc.estimateStartPos({-36, -36}, 0);
  // wc.followPath({PVector(-36, 0), PVector(0, 42.12)});
  // clipGoal();
  // s(1000);
  // raiseLiftByOneWait();
  // // conveyer.ready = true;
  // s(1000);
  // while(1){
  //   wc.turnTo(0);
  //   addPids();
  //   wc.turnTo(180);
  //   addPids();
  // }
  // addPids();
  
  // unclipLiftGoal();
  // while(1){
  //   s(100);
  //   if(!frontCounter.pressing()){
  //     Greg.rumble(".");
  //     s(2000);
  //     while(!Greg.ButtonA.pressing()){
  //       s(10);
  //     }
  //     break;
  //   }
  // }
  // // wc.backwardsFollow({{0, 0}});
  // wc.setMaxAcc(5000000);
  // wc.setMaxDeAcc(50000000);
  // unclipLiftGoal();
  // wc.preventTurn();
  // thread yeet = thread([](){
  //   double start = Brain.Timer.system();
  //   while(1){
  //     if(frontCounter.pressing()){
  //       cout << Brain.Timer.system() - start << endl;
  //       clipLiftGoal();
  //       s(100);
  //       wc.forceEarlyExit();
  //       break;
  //     }
  //     s(10);
  //   }
  // });
  // wc.driveTo(12, 36);
  // clipLiftGoal();
  // yeet.detach();
  // wc.backwardsFollow({{-48, 36}});
  drivercontrol();
  
  //wc.turnTo(0);
  // while(1){
  //   cout << share.position() << endl;
  //   s(50);
  // }
  // while(1){
  //   if(share.velocity() != 0){
  //     cout << "ok" << endl;
  //     s(10);
  //   }
  //   s(1);
  // }
  s(500);
  
  wc.followPath({PVector(0, 48), PVector(-48, 48)});
  // drivercontrol();
  // drivercontrol();
  // autonomous();
  // drivercontrol();
  // cout << "Y " << endl;
  Competition.drivercontrol(drivercontrol);
  // cout << "Yeet " << endl;
  Competition.autonomous(autonomous);
  
  while(1){
    
    s(300);
  }
}






