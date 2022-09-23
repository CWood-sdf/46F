/*
._______   _______  ____    _______
.|__  __|  | __  |  |   \   | __  |
.  | |     | | | |  | |\ \  | | | | 
.  | |     | |_| |  | |/ /  | |_| |
.  |_|     |_____|  |___/   |_____|

  Maybe add a configuration screen that can configure variables, and it can have multiple screens
. Finish writing vision util
. Testing items
.  -Wheelbase controls
.  -Vision relative position detection
.  -Auton init with potentiometers
.  -Auton fns (esp roller spinner)
.  -Flywheel again i guess
.  -Vision odom
. Put abs in Controllers.cpp line 38 maybe
. Maybe insert a sleep command after the error cout in Path::make

*/
//main.cpp
#include "Updaters.h"
#include "AutonInit/Init.h"
#include "BrainOS/BotTracker.h"
#include "lv_conf.h"
using namespace ClassFns;
using namespace vex;

competition Competition;//
//Returns true if a button is pressing at the start, but doesn't return until button releaseed
bool isPressing(const controller::button& btn){
  if(btn.pressing()){
    while(btn.pressing()){
      s(10);
    }
    return true;
  }
  
  return false;
}


//Similar to isPressing(controller::button&), but only does it if a joystick axis is going in a certain direction
bool isPressing(const controller::axis& joystick, int mult) {
  if (abs(joystick.value()) > 50 && joystick.value() * mult > 1) {
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

// void addPids(){
//   cout << "Turn P" << endl;
//   while(1){
//     if(isPressing(Greg.ButtonA)){
//       wc.addTurnPid(0, 0, 0);
//       break;
//     }
//     if(isPressing(Greg.ButtonB)){
//       wc.addTurnPid(0.1, 0, 0.0);
//       break;
//     }
//   }
//   cout << "Turn D" << endl;
//   while(1){
//     if(isPressing(Greg.ButtonA)){
//       break;
//     }
//     if(isPressing(Greg.ButtonB)){
//       wc.addTurnPid(0.0, 0, 0.05);
//       break;
//     }
//   }
  
//   cout << "Done" << endl;
// }
void randomAutonTest(){
  // wc.generalFollow({PVector(48, 0), PVector(48, 48)}, &ramsete, false);
  // wc.generalFollow({PVector(48, 0), PVector(48, 48)}, &purePursuit, false);
  // wc.generalFollow({PVector(48, 48)}, &pidController, false);

}
Auton leftA = "Left" + [](){
  cout << "l" << endl;

};
Auton rightA = "Right" + [](){
  cout << "r" << endl;
};
Auton coolA = "Cool" + [](){
  //sdf
};
Auton csdf = "sdfff" + [](){
  
};
Auton scsdf = "sdffsdff" + [](){
  
};
void autonInit(){
  cout << "Auton Init" << endl;
  cout << "Auton Init Done" << endl;
}
void autonomous(){
  while(!Auton::isDone()){
    s(100);
  }

  //Auton timer system, used in testing
  auto startTime = Brain.Timer.system();
  autonInit();
  //Put auton call here
  Auton::callAuton();
  wc.driveTo(1, 1);
  //Print time
  cout << (Brain.Timer.system() - startTime) / 1000 << endl;
}

//}
#define sensitivity 12 

//Drivercontrol + automation {

//This class allows a button latch to exist
class ButtonLatch {
  bool isPressing = false;
  int state = 0;
  int stateLim;
  const controller::button& b;
public:
  ButtonLatch(const controller::button& b, int stateLim = 2) : stateLim(stateLim), b(b){

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
  ButtonLatch UpLatch = ButtonLatch(Greg.ButtonUp);
  ButtonLatch DownLatch = ButtonLatch(Greg.ButtonDown);
  int currentVelocity = 540;
  int flywheelI = 1;
  static bool driveReversed = false;
  //Protection from multiple instances of drivercontrol running
  //Is true if there is no instance of drivercontrol running
  static bool allEmpty = false;
  //An array of the current and past instances of drivercontrol
  //The pair::first is whether the instance has exited or not
  //The pair::second is a pointer to the primary bool a few lines down
  static vector<pair<bool, bool*>> countsExist = {};
  //The count of drivercontrol instances
  static int count = 0;
  
  //Is true if this drivercontrol instance should be running
  bool primary = count == 0 || allEmpty ? true : false;
  //Push back the array
  countsExist.push_back({true, &primary});
  //The index of this drivercontrol instance in countsExist
  int localCount = count;
  count++;
  while(1){
    //Place driver code in here
    if (primary) {
      //Drive control, uses quotient/square for smoothness
      double Y1 = abs(Greg.Axis2.value()) > sensitivity ? Greg.Axis2.value() : 0;
      double X1 = abs(Greg.Axis1.value()) > sensitivity ? Greg.Axis1.value() : 0;
      // Y1 /= 10;
      // Y1 *= Y1;
      // Y2 /= 10;
      // Y2 *= Y2;
      // Y1 *= Greg.Axis2.value() != 0 ? Greg.Axis2.value() / abs(Greg.Axis2.value()) : 1;
      // Y2 *= Greg.Axis3.value() != 0 ? Greg.Axis3.value() / abs(Greg.Axis3.value()) : 1;

      double s1 = Y1 + X1;
      double s2 = Y1 - X1;
      if (driveReversed) {
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
      if (UpLatch.pressing() && currentVelocity < 600) {
        currentVelocity += 60;
        flyTBH.addTarget(currentVelocity);
        flyTBH.setTarget(flywheelI++);
      }
      if (DownLatch.pressing() && currentVelocity > 60) {
        currentVelocity -= 60;
        flyTBH.addTarget(currentVelocity);
        flyTBH.setTarget(flywheelI++);
      }
      if (BLatch.pressing()) {
        driveReversed = !driveReversed;
      }
    }
    else {    }
    s(10);

    
  }
  //Let other instances know that this drivercontrol can't run
  countsExist[localCount].first = false;
  //Search for a working drivercontrol instance, and set it to working
  //If it's not working, allEmpty will be true
  for(auto [exist, ptr] : countsExist){
    if(exist){
      *ptr = true;
      allEmpty = false;
      break;
    }
    else {
      allEmpty = true;
    }
  }
}
void runFlywheel(){
  //int index = 0;
  // flyPID.setTarget(0);
  // timer ok;
  while (1) {
    // if(flywheelPID){
    //   flyPID.step();
    // }
    // else {
    flyTBH.step();
    // }
    s(50);
  }
}
//}
// template<typename T>
// concept okok = true;

// template<okok T>
// void test(T t){
//   cout << "ok" << endl;
// }


//Brain Drawing Stuff {

void printVars(bool) {
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
void drawPath(bool){
  // s(100);
  PVector off = PVector(100, 100);
  Brain.Screen.waitForRefresh();
  Brain.Screen.clearScreen(black);
  
  if(wc.drawArr){
    Brain.Screen.setFillColor(blue);
    for(auto v : wc.publicPath){
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

void displayBot(bool);
#define V5_LVGL_RATE    4
void  vexTaskSleep( uint32_t time );
bool init = false;
void brainOS() {
  while(!init){
    s(500);
  }
  
  
  bos::bosFns.push_back(windowsLoader);
  bos::bosFns.push_back(bos::BosFn(graphFlywheelTBH, true));
  bos::bosFns.push_back(printVars);
  bos::bosFns.push_back(drawPath);
  bos::bosFns.push_back(bos::BosFn(displayBot, true));
  
  bos::bosFns.push_back(Auton::selectAuton);
  //int state = 0;		
  //int maxState = 3; 
  Button screenLeft = Button(Brain, 10, BRAIN_HEIGHT - 60, 30, 30, black, "<", -40, -30);		
  Button screenRight = Button(Brain, BRAIN_WIDTH - 40, BRAIN_HEIGHT - 60, 30, 30, black, ">", -40, -30);		
  //lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);
  bos::bosFns.getCurrent()->call(true);
  while (1) {		
    if(bos::bosFns.size(0)){
      cout << "bosFns is empty for some reason" << endl;
      break;
    }

    auto result = bos::bosFns.getCurrent()->call(false);
    if(bos::bosFns.getCurrent()->lvgl()){
      lv_tick_inc(V5_LVGL_RATE);
      lv_task_handler();
    }
    if(result){
      if(bos::bosFns.getCurrent()->lvgl()){
        cout << "Clean" << endl;
        //Remove all objects
        lv_obj_clean(lv_scr_act());
        lv_anim_del_all();
      }
      bos::bosFns.popCurrent();
      if(bos::bosFns.getCurrent()->lvgl()){
        cout << "remake" << endl;
        //Tell it to remake
        bos::bosFns.getCurrent()->call(true);
      }
    }
    screenLeft.draw();		
    screenRight.draw();	
    if (screenLeft.clicked() && &bos::bosFns.getBase() != &bos::bosFns.getCurrent()) {	
      if(bos::bosFns.getCurrent()->lvgl()){
        cout << "Clean" << endl;
        //Remove all objects
        lv_obj_clean(lv_scr_act());
        lv_anim_del_all();
      }
      auto oldPtr = &bos::bosFns.getCurrent();
      bos::bosFns.moveCurrentLeft();
      auto newPtr = &bos::bosFns.getCurrent();
      if(oldPtr != newPtr && bos::bosFns.getCurrent()->lvgl()){
        cout << "remake" << endl;
        //Tell it to remake
        bos::bosFns.getCurrent()->call(true);
      }
    }	
    else if (screenRight.clicked() && &bos::bosFns.getEnd() != &bos::bosFns.getCurrent()) {	
      if(bos::bosFns.getCurrent()->lvgl()){
        cout << "Clean" << endl;
        //Remove all objects
        lv_obj_clean(lv_scr_act());
        lv_anim_del_all();
      }
      auto oldPtr = &bos::bosFns.getCurrent();
      bos::bosFns.moveCurrentRight();
      auto newPtr = &bos::bosFns.getCurrent();
      if(oldPtr != newPtr && bos::bosFns.getCurrent()->lvgl()){
        cout << "remake" << endl;
        //Tell it to remake
        bos::bosFns.getCurrent()->call(true);
      }
    }	

    // Allow other tasks to run
    vex::task::sleep(V5_LVGL_RATE);
    Brain.Screen.waitForRefresh();
  }	
}
//}


int main() {
  
  //Init has to be in thread, otherwise it won't work with comp switch
  thread initThread = thread([](){
    v5_lv_init();
    cout << "<< Lvgl initialized >>" << endl;
    s(100);
    gyroInit(angler);
    s(500);
    gyroInit(GPS);
    //Gyro's initialized
    testMotorConnection();
    cout << "<< Motor connection test complete >>" << endl;
    s(500);
    flyTBH.setTarget(0);
    cout << "<< Flywheel initialized >>" << endl;
    s(500);
    init = true;
  });
  while(!init){
    s(100);
  }
  [[maybe_unused]]
  KillThread gpsUpdate = thread(updateSharePos);

  //Make a thread to execute some auton tasks concurrently
  [[maybe_unused]]
  KillThread otherThreads = thread(executeThreads);

  //Awesome brain screen control thread
  thread loader = thread(brainOS);

  thread flywheelControl = thread(runFlywheel);

  drivercontrol();
  //autonomous();
  // Competition.autonomous(autonomous);
  // Competition.drivercontrol(drivercontrol);

  //Prevent main from exiting
  while(1){
    s(300);
  }
}






