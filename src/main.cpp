//main.cpp
#include "updaters.h"
#include "AutonInit/Init.h"
#include "BrainOS/Manager.h"
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

//Moves bot backwards at the given speed for the given amount of time 
void backwards(int t, int sp = 40){
  Left.spin(vex::reverse, sp, pct);
  Right.spin(vex::reverse, sp, pct);
  s(t);
  Left.stop();
  Right.stop();
}
void fwds(int t, double sp = 30){
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
Auton leftA = "Left" + [](){
  cout << "l" << endl;
};
Auton rightA = "Right" + [](){
  cout << "r" << endl;
};
void autonInit(){
  cout << "Auton Init" << endl;
  cout << "Auton Init Done" << endl;
}
void autonomous(){
  //Auton timer system, used in testing
  auto startTime = Brain.Timer.system();
  autonInit();
  //Put auton call here
  Auton::callAuton();
  //Print time
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
  ButtonLatch BLatch = ButtonLatch(Greg.ButtonB);
  static bool driveReversed = false;
  //int wheelsMove = 0;
  // backLiftCtrllr.disable();
  KillThread::killAll();
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
    s(10);
    
  }
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
  bos::bosFns.push_back(windowsLoader);
  bos::bosFns.push_back(windowsLoader);
  bos::bosFns.push_back(printVars);
  bos::bosFns.push_back(displayTilt);
  bos::bosFns.push_back(drawPath);
  bos::bosFns.push_back(Auton::selectAuton);
  
  int state = 0;		
  int maxState = 3; 
  Button screenLeft = Button(Brain, 10, BRAIN_HEIGHT - 60, -30, -30, black, "<");		
  Button screenRight = Button(Brain, BRAIN_WIDTH - 40, BRAIN_HEIGHT - 60, -30, -30, black, ">");		
  while (1) {		
    if(bos::bosFns.size(0)){
      cout << "bosFns is empty for some reason" << endl;
      break;
    }
    auto result = bos::bosFns.getCurrent()->call();
    if(result){
      bos::bosFns.popCurrent();
    }
    screenLeft.draw();		
    screenRight.draw();	
    if (screenLeft.clicked()) {	
      bos::bosFns.moveCurrentLeft();
    }	
    else if (screenRight.clicked()) {	
      bos::bosFns.moveCurrentRight();
    }	
    Brain.Screen.waitForRefresh();
  }	
}
//}
bool init = false;
int main() {
  //Init has to be in thread, otherwise it won't work with comp switch
  thread initThread = thread([](){
    v5_lv_init();
    gyroInit(angler);
    gyroInit(GPS);
    init = true;
  });

  KillThread gpsUpdate = thread(updateSharePos);

  //Make a thread to execute some auton tasks concurrently
  KillThread otherThreads = thread(executeThreads);

  //WINDOWS LOADER: YEET BURGER
  thread loader = thread(brainOS);
  
  Competition.autonomous(autonomous);
  Competition.drivercontrol(drivercontrol);

  //Prevent main from exiting
  while(1){
    s(300);
  }
}






