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
/*
The Plan

-2 distance sensors and color sensor for the roller
-At least 2 line trackers for auto indexer and rollers
-Maybe auto intake
-Vision sensor goal positioning
*/
//main.cpp
#include "BrainOS/VariableConfig.h"
#include "BrainOS/ConnectionTest.h"
#include "Autons/Autons.h"
using namespace ClassFns;
using namespace vex;
void spinRoller() {
  rachetColor.setLightPower(50, percent);
  bool targetRed = wc.isRed();
  
  auto hue = rachetColor.hue();
  bool isRed = hue > 300 || hue < 60;
  bool lastRed = isRed;
  chassis.driveFromDiff(-60, 0, fwd);
  int count = 0;
  bool countUp = true;
  while(1){
    if(isRed != lastRed && isRed != targetRed){
      break;
    }
    intake.spin(vex::reverse, 50);
    auto hue = rachetColor.hue();
    lastRed = isRed;
    isRed = hue > 300 || hue < 60;
    cout << hue << ", " << isRed << ", " << targetRed << endl;
    // if(!isRed){
    //   if(abs(hue - 240) < 60){
    //     isRed = false;
    //   }
    //   else {
    //     //Ambiguous colors, set isRed to lastRed to prevent exit
    //     isRed = lastRed;
    //   }
    // }
    if(countUp){
      count++;
    }
    else {
      count--;
    }
    if(count == 20){
      countUp = false;
      chassis.driveFromDiff(30, 0, fwd);
    }
    else if(count == 0){
      countUp = true;
      chassis.driveFromDiff(-80, 0, fwd);
    }
    s(10);
  }
  intake.spin(vex::reverse, -100);
  chassis.driveFromDiff(-60, 0, fwd);
  s(500);
  chassis.coastBrake();
  intake.stop(hold);
  //Give the system some time to clear momentum
  s(300);
}
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
//       wc.addTurnPid(0, 0, 0);p
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
}

string testPrintData = "";
void printTestData(bool){
  Brain.Screen.clearScreen(black);
  Brain.Screen.setCursor(1, 1);
  Brain.Screen.print(testPrintData.data());
}

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
  // wc.driveTo(1, 1);
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
  ButtonLatch XLatch = ButtonLatch(Greg.ButtonX);
  ButtonLatch YLatch = ButtonLatch(Greg.ButtonY);
  ButtonLatch LeftLatch = ButtonLatch(Greg.ButtonLeft);
  ButtonLatch RightLatch = ButtonLatch(Greg.ButtonRight);
  ButtonLatch L1Latch = ButtonLatch(Greg.ButtonL1);
  ButtonLatch L2Latch = ButtonLatch(Greg.ButtonL2);
  // int currentVelocity = 510;
  // int flywheelI = 1;
  flyTBH.setTargetSpeed(510);
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
  int flywheelSpeed = 510;
  count++;
  while(1){
    //Place driver code in here
    if (primary) {
      //Drive control, uses quotient/square for smoothness
      double Y1 = abs(Greg.Axis3.value()) > sensitivity ? Greg.Axis3.value() : 0;
      double X1 = abs(Greg.Axis4.value()) > sensitivity ? Greg.Axis4.value() : 0;
      // Y1 /= 10;
      // Y1 *= Y1;
      // Y2 /= 10;
      // Y2 *= Y2;
      // Y1 *= Greg.Axis2.value() != 0 ? Greg.Axis2.value() / abs(Greg.Axis2.value()) : 1;
      // Y2 *= Greg.Axis3.value() != 0 ? Greg.Axis3.value() / abs(Greg.Axis3.value()) : 1;

      double s1 = Y1 + X1;
      double s2 = Y1 - X1;
      if(L1Latch.pressing()){
        flywheelSpeed += 600 * 7 / 100;
        if(flywheelSpeed > 600){
          flywheelSpeed = 600;
        }
        flyTBH.setTargetSpeed(flywheelSpeed);
      }
      if(L2Latch.pressing()){
        flywheelSpeed -= 600 * 7 / 100;
        if(flywheelSpeed < 0){
          flywheelSpeed = 0;
        }
        flyTBH.setTargetSpeed(flywheelSpeed);
      }
      if(Greg.ButtonLeft.pressing()){
        s1 = -50, s2 = 50;
      }
      if(Greg.ButtonRight.pressing()){
        s1 = 50, s2 = -50;
      }
      if (driveReversed) {
        leftWheels.spin(vex::reverse, s1, pct);
        rightWheels.spin(vex::reverse, s2, pct);
      }
      else {
        leftWheels.spin(fwd, s1, pct);
        rightWheels.spin(fwd, s2, pct);
      }
      if(Greg.ButtonR1.pressing()){
        intake.spin(fwd, 100);
      }
      else if(Greg.ButtonR2.pressing()){
        intake.spin(vex::reverse, 100);
      }
      else{
        intake.stop(hold);
      }
      // if (BLatch.pressing()) {
      //   driveReversed = !driveReversed;
      // }

      if(XLatch.pressing()){
        wc.turnTo(wc.botPos().angleTo({-50, -50}));
      }
      if(YLatch.pressing()){
        wc.turnTo(wc.botPos().angleTo({50, 50}));
      }
      // if(Greg.ButtonRight.pressing()){
      //   //If continuously pressing for 0.5s, release endgame
      //   int time = 0;
      //   while(Greg.ButtonRight.pressing()){
      //     s(10);
      //     time += 10;
      //     if(time >= 500){
      //       endgame.open();
      //       Greg.rumble("...."); 
      //       break;
      //     }
      //   }
      // }
      
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
enum class Alliance : int {
  red = 0,
  blue = 1
};

void displayBot(bool);
#define V5_LVGL_RATE    4
void  vexTaskSleep( uint32_t time );
bool init = false;
void fn(bool ){
  Brain.Screen.clearScreen(black);
  if(Brain.Screen.pressing()){
    intake.spinVolt(fwd, 10000);
  }
  else {
    intake.stop(coast);
  }
}
bool helpAlignBot(bool){
  static double desiredAngle = 255;
  static PVector position = PVector({60.5, -39});
  // position.angleTo({-50, -50});
  static bool init = false;
  if(!init && Auton::isDone()){
    if(Auton::selectedName() == "Right"){
      desiredAngle = 194;
    }
    init = true;
    if(wc.isRed()){
      desiredAngle = posNeg180(desiredAngle + 180);
      
    }
  }
  Brain.Screen.clearScreen(black);
  Brain.Screen.setPenColor(red);
  if(abs(wc.botAngle() - desiredAngle) < 1){
    Brain.Screen.setPenColor(green);
  }
  Brain.Screen.setCursor(1, 1);
  Brain.Screen.print("Desired angle: %f", desiredAngle);
  Brain.Screen.setCursor(2, 1);
  Brain.Screen.print("Current angle: %f", wc.botAngle());
  Brain.Screen.setPenColor(white);
  if(Brain.Screen.pressing() && Brain.Screen.yPosition() < 100){
    return true;
  }
  return false;
}
void brainOS() {
  while(!init){
    s(500);
  }
  cout << "Start" << endl;
  VariableConfig setAlliance = VariableConfig({ "red", "blue" }, "Alliance", 0, [](int i) {
    if (i == 0) {
      wc.setRed();
    }
    else {
      wc.setBlue();
    }
  });
  setAlliance.addBypass([](){
    // return false;
    return Auton::selectedName() == skills.getName();
  });
  // VariableConfig setSDFsdfdf = VariableConfig({"sdfsdf", "sdasdwsdf", "werwerwe", "sdff", "???"}, "Thing");
  bos::bosFns.pushBack(testConnection);
  bos::bosFns.pushBack(helpAlignBot);
  bos::bosFns.pushBack(bos::BosFn(graphFlywheelTBH, true));
  bos::bosFns.pushBack(VariableConfig::drawAll);
  bos::bosFns.pushBack(bos::BosFn(printTestData));
  bos::bosFns.pushBack(bos::BosFn(displayBot, true));
  // bos::bosFns.push_back(fn);
  bos::bosFns.pushBack(windowsLoader);
  

  // bos::bosFns.push_back(Auton::selectAuton);
  //int state = 0;
  //int maxState = 3;
  Button screenLeft = Button(Brain, 0, BRAIN_HEIGHT - 60, 40, 40, black, color(50, 50, 50), "<", -40, -30);
  Button screenRight = Button(Brain, BRAIN_WIDTH - 40, BRAIN_HEIGHT - 60, 40, 40, black, color(50, 50, 50), ">", -40, -30);
  //lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);
  bos::bosFns.getCurrent()->call(true);
  while (1) {
    if(bos::bosFns.size(0)){
      cout << "bosFns is empty for some reason" << endl;
      break;
    }
    //Have buttons clicked first so that clicking them overrides the screenx
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
        //Tell it to remake
        bos::bosFns.getCurrent()->call(true);
      }
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
    cout << "<< Gyros initialized >>" << endl;
    //Gyro's initialized
    testMotorConnection();
    // testMotorConfiguration();
    cout << "<< Motor connection test complete >>" << endl;
    s(500);
    // flyTBH.setTarget(0);
    flyTBH.setTargetSpeed(0);
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
  // chassis.setSpeedLimit(30);
  // VectorArr path = VectorArr();
  // purePursuit.setTurn();

  // flyTBH.setTargetSpeed(500);
  // flyTBH.setDisabled(true);
  // auto acc = flyTBH.maxRateGain;
  // flyTBH.maxRateGain = 100;
  // flywheelNm.spin(fwd, 52000/600);
  // s(2000);
  // flyTBH.setDisabled(false);
  // flyTBH.maxRateGain = acc;
  // s(50);
  
  // // wc.faceTarget({-40.79, 50.06});
  // while(!flyTBH.ready()){
  //   s(10);
  // }
  // intake.spin(fwd, 100);
  // s(1000);
  // intake.stop(hold);
  // s(1000);
  // while(share.gpsBad() || !Greg.ButtonA.pressing()){
  //   s(100);
  // }
  // flyTBH.setTargetSpeed(0);

  // wc.setExitDist(2);
  // pidController.setTurn();
  // wc.path.setK(2);
  // chassis.setMaxAcc(1000);
  // chassis.setMaxDAcc(1000);
  // chassis.setSpeedLimit(70);
  // rachetColor.setLightPower(50, pct);
  // wc.estimateStartPos(PVector(-14.13, 61.72), 185.83);
  // wc.driveTo(-29.98, 47.68);
  // wc.backwardsFollow(&pidController, {PVector(-43.39, 54.12)});
  // wc.backwardsFollow(&pidController, {PVector(-39.72, 60.73)});
  
  // wc.setExitMode(BasicWheelController::exitMode::coast);
  // spinRoller();
  // wc.estimateStartPos(PVector(0, 0), 0);
  // spinRoller();


  //autonomous();
  Competition.autonomous(autonomous);
  Competition.drivercontrol(drivercontrol);

  //Prevent main from exiting
  while(1){
    s(300);
  }
}






