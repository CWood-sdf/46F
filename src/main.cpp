/*
._______   _______  ____    _______
.|__  __|  | __  |  |   \   | __  |
.  | |     | | | |  | |\ \  | | | |
.  | |     | |_| |  | |/ /  | |_| |
.  |_|     |_____|  |___/   |_____|
.
. Testing items
.  -Wheelbase controls
.  -Auton fns (esp roller spinner)
. Put abs in Controllers.cpp line 38 maybe
. Maybe insert a sleep command after the error cout in Path::make

*/
// main.cpp
#include "BrainOS/VariableConfig.h"
#include "BrainOS/ConnectionTest.h"
#include "Autons/Autons.h"
using namespace ClassFns;
using namespace vex;
void spinRoller()
{
  bool intakeDisabled = intakeController.disabled;
  intakeController.disable();
  rachetColor.setLightPower(50, percent);
  bool targetRed = wc.isRed();

  auto hue = rachetColor.hue();
  bool isRed = hue > 300 || hue < 60;
  bool lastRed = isRed;
  chassis.driveFromDiff(-60, 0, fwd);
  int count = 0;
  bool countUp = true;
  int i = 0;
  while (1)
  {
    if (isRed != lastRed && isRed == targetRed && i > 10)
    {
      break;
    }
    intake.spin(vex::reverse, 100);
    auto hue = rachetColor.hue();
    lastRed = isRed;
    isRed = hue > 300 || hue < 60;
    if (countUp)
    {
      count++;
    }
    else
    {
      count--;
    }
    if (count == 20)
    {
      countUp = false;
      chassis.driveFromDiff(10, 0, fwd);
    }
    else if (count == 0)
    {
      countUp = true;
      chassis.driveFromDiff(-20, 0, fwd);
    }
    i++;
    s(10);
  }
  // intake.spin(vex::reverse, -100);
  chassis.driveFromDiff(-10, 0, fwd);
  // s(500);
  chassis.coastBrake();
  intake.stop(hold);
  if (!intakeDisabled)
  {
    intakeController.enable();
  }
}
competition Competition;

string testPrintData = "";
void printTestData(bool)
{
  Brain.Screen.clearScreen(black);
  Brain.Screen.setCursor(1, 1);
  Brain.Screen.print(testPrintData.data());
}

void autonInit()
{
  cout << "Auton Init" << endl;
  rachetColor.setLight(ledState::on);
  rachetColor.setLightPower(100, percent);
  rachetColor.integrationTime(50);
  intakeController.autonInit();
  intakeController.disable();
  cout << "Auton Init Done" << endl;
}
void autonomous()
{
  while (!Auton::isDone())
  {
    s(100);
  }

  // Auton timer system, used in testing
  auto startTime = Brain.Timer.system();
  autonInit();
  // Put auton call here
  Auton::callAuton();
  // wc.driveTo(1, 1);
  // Print time
  cout << "Auton Time: " << (Brain.Timer.system() - startTime) / 1000 << endl;
}

//}
#define sensitivity 12

// Drivercontrol + automation {

// This class allows a button latch to exist
class ButtonLatch
{
  bool isPressing = false;
  int state = 0;
  int stateLim;
  const controller::button &b;

public:
  ButtonLatch(const controller::button &b, int stateLim = 2) : stateLim(stateLim), b(b)
  {
  }
  ButtonLatch() = delete;
  bool pressing()
  {
    if (b.pressing() && !isPressing)
    {
      state++;
      if (state == stateLim + 1)
      {
        state = 1;
      }

      isPressing = true;
      return true;
    }
    else if (!b.pressing())
    {
      isPressing = false;
    }
    return false;
  }
  int getState()
  {
    return state;
  }
};

void drivercontrol()
{
  [[maybe_unused]] ButtonLatch BLatch = ButtonLatch(Greg.ButtonB);
  [[maybe_unused]] ButtonLatch UpLatch = ButtonLatch(Greg.ButtonUp);
  [[maybe_unused]] ButtonLatch DownLatch = ButtonLatch(Greg.ButtonDown);
  [[maybe_unused]] ButtonLatch XLatch = ButtonLatch(Greg.ButtonX);
  [[maybe_unused]] ButtonLatch YLatch = ButtonLatch(Greg.ButtonY);
  [[maybe_unused]] ButtonLatch LeftLatch = ButtonLatch(Greg.ButtonLeft);
  [[maybe_unused]] ButtonLatch RightLatch = ButtonLatch(Greg.ButtonRight);
  [[maybe_unused]] ButtonLatch L1Latch = ButtonLatch(Greg.ButtonL1);
  [[maybe_unused]] ButtonLatch L2Latch = ButtonLatch(Greg.ButtonL2);
  // int currentVelocity = 510;
  // int flywheelI = 1;
  flyTBH.setTargetSpeed(367);
  static bool driveReversed = false;
  // Protection from multiple instances of drivercontrol running
  // Is true if there is no instance of drivercontrol running
  static bool allEmpty = false;
  // An array of the current and past instances of drivercontrol
  // The pair::first is whether the instance has exited or not
  // The pair::second is a pointer to the primary bool a few lines down
  static vector<pair<bool, bool *>> countsExist = {};
  // The count of drivercontrol instances
  static int count = 0;

  // Is true if this drivercontrol instance should be running
  bool primary = count == 0 || allEmpty ? true : false;
  // Push back the array
  countsExist.push_back({true, &primary});
  // The index of this drivercontrol instance in countsExist
  int localCount = count;
  double flywheelSpeed = 367;
  count++;
  // TODO: intake controls
  while (1)
  {
    // Place driver code in here
    if (primary)
    {
      // Drive control, uses quotient/square for smoothness
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
      if (Greg.ButtonL1.pressing())
      {
        flywheelSpeed += 0.5;
        if (flywheelSpeed > 600)
        {
          flywheelSpeed = 600;
        }
        flyTBH.setTargetSpeed(flywheelSpeed);
      }
      if (Greg.ButtonL2.pressing())
      {
        flywheelSpeed -= 0.5;
        if (flywheelSpeed < 0)
        {
          flywheelSpeed = 0;
        }
        flyTBH.setTargetSpeed(flywheelSpeed);
      }
      if (Greg.ButtonLeft.pressing())
      {
        s1 = -50, s2 = 50;
      }
      if (Greg.ButtonRight.pressing())
      {
        s1 = 50, s2 = -50;
      }
      if (s1 == s2 && s2 == 0)
      {
        leftWheels.stop(coast);
        rightWheels.stop(coast);
      }
      else if (driveReversed)
      {
        leftWheels.spin(vex::reverse, s1, pct);
        rightWheels.spin(vex::reverse, s2, pct);
      }
      else
      {
        leftWheels.spin(fwd, s1, pct);
        rightWheels.spin(fwd, s2, pct);
      }
      if (Greg.ButtonR1.pressing())
      {
        intakeController.disable();
        intake.spin(fwd, 100);
      }
      else if (Greg.ButtonR2.pressing())
      {
        intakeController.disable();
        intake.spin(vex::reverse, 100);
      }
      else
      {
        intakeController.disable();
        intake.stop(hold);
      }
      if (UpLatch.pressing())
      {
        if (wc.isRed())
        {
          wc.faceTarget({50, 50});
        }
        else
        {
          wc.faceTarget({-50, -50});
        }
      }
      // if (BLatch.pressing()) {
      //   driveReversed = !driveReversed;
      // }

      // if (XLatch.pressing())
      // {
      //   intakeController.intake();
      // }
      // if (BLatch.pressing())
      // {
      //   intakeController.setFiring();
      // }
      // if(XLatch.pressing()){
      //   wc.turnTo(wc.botPos().angleTo({-50, -50}));
      // }
      // if(YLatch.pressing()){
      //   wc.turnTo(wc.botPos().angleTo({50, 50}));
      // }
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
    else
    {
    }
    s(10);
  }
  // Let other instances know that this drivercontrol can't run
  countsExist[localCount].first = false;
  // Search for a working drivercontrol instance, and set it to working
  // If it's not working, allEmpty will be true
  for (auto [exist, ptr] : countsExist)
  {
    if (exist)
    {
      *ptr = true;
      allEmpty = false;
      break;
    }
    else
    {
      allEmpty = true;
    }
  }
}
void runIntake()
{
  while (1)
  {
    if (!intakeController.disabled)
    {
      bool flywheelReady = flyTBH.ready();
      // cout << flywheelReady << endl;
      intakeController.updateValues(flywheelReady);
      // cout << hex;
      // cout << intakeController.diskMask << endl;
      if (intakeController.spinMotor())
      {
        intake.spin(fwd, 70);
      }
      else if (intakeController.reverseMotor())
      {
        intake.spin(vex::reverse, 100);
      }
      else
      {
        intake.stop(hold);
      }
    }
    s(10);
  }
}
void runFlywheel()
{
  // int index = 0;
  //  flyPID.setTarget(0);
  //  timer ok;
  while (1)
  {
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

// Brain Drawing Stuff {

enum class Alliance : int
{
  red = 0,
  blue = 1
};

void displayBot(bool);
#define V5_LVGL_RATE 4
void vexTaskSleep(uint32_t time);
bool init = false;
bool helpAlignBot(bool)
{
  static double desiredAngle = 255;
  // static PVector position = PVector({60.5, -39});
  // position.angleTo({-50, -50});
  static bool init = false;
  if (!init && Auton::isDone())
  {
    if (Auton::selectedName() == "Right")
    {
      desiredAngle = 194;
    }
    init = true;
    if (wc.isRed())
    {
      desiredAngle = posNeg180(desiredAngle + 180);
    }
  }
  Brain.Screen.clearScreen(black);
  Brain.Screen.setPenColor(red);
  if (abs(wc.botAngle() - desiredAngle) < 1)
  {
    Brain.Screen.setPenColor(green);
  }
  Brain.Screen.setCursor(1, 1);
  Brain.Screen.print("Desired angle: %f", desiredAngle);
  Brain.Screen.setCursor(2, 1);
  Brain.Screen.print("Current angle: %f", wc.botAngle());
  Brain.Screen.setPenColor(white);
  if (Brain.Screen.pressing() && Brain.Screen.yPosition() < 100)
  {
    return true;
  }
  return false;
}
void brainOS()
{
  ButtonLatch leftLatch = ButtonLatch(Greg.ButtonLeft);
  ButtonLatch rightLatch = ButtonLatch(Greg.ButtonRight);
  while (!init)
  {
    s(500);
  }
  cout << "Start" << endl;
  VariableConfig setAlliance = VariableConfig({"red", "blue"}, "Alliance", 0, [](int i)
                                              {
    if (i == 0) {
      wc.setRed();
    }
    else {
      wc.setBlue();
    } });
  setAlliance.addBypass([]()
                        {
    // return false;
    return Auton::selectedName() == skills.getName(); });
  // VariableConfig setSDFsdfdf = VariableConfig({"sdfsdf", "sdasdwsdf", "werwerwe", "sdff", "???"}, "Thing");
  bos::bosFns.pushBack(testConnection);
  bos::bosFns.pushBack(bos::BosFn(graphFlywheelTBH, true));
  bos::bosFns.pushBack(bos::BosFn(displayBot, true));
  // bos::bosFns.pushBack(helpAlignBot);
  bos::bosFns.pushBack(VariableConfig::drawAll);
  bos::bosFns.pushBack(bos::BosFn(printTestData));
  // bos::bosFns.push_back(fn);
  bos::bosFns.pushBack(windowsLoader);

  // bos::bosFns.push_back(Auton::selectAuton);
  // int state = 0;
  // int maxState = 3;
  Button screenLeft = Button(Brain, 0, BRAIN_HEIGHT - 60, 40, 40, black, color(50, 50, 50), "<", -40, -30);
  Button screenRight = Button(Brain, BRAIN_WIDTH - 40, BRAIN_HEIGHT - 60, 40, 40, black, color(50, 50, 50), ">", -40, -30);
  // lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);
  bos::bosFns.getCurrent()->call(true);
  while (1)
  {
    if (bos::bosFns.size(0))
    {
      cout << "bosFns is empty for some reason" << endl;
      break;
    }
    // Have buttons clicked first so that clicking them overrides the screenx
    if ((screenLeft.clicked() || (!Competition.isDriverControl() && leftLatch.pressing())) && &bos::bosFns.getBase() != &bos::bosFns.getCurrent())
    {
      if (bos::bosFns.getCurrent()->lvgl())
      {
        cout << "Clean" << endl;
        // Remove all objects
        lv_obj_clean(lv_scr_act());
        lv_anim_del_all();
      }
      auto oldPtr = &bos::bosFns.getCurrent();
      bos::bosFns.moveCurrentLeft();
      auto newPtr = &bos::bosFns.getCurrent();
      if (oldPtr != newPtr && bos::bosFns.getCurrent()->lvgl())
      {
        // Tell it to remake
        bos::bosFns.getCurrent()->call(true);
      }
    }
    else if ((screenRight.clicked() || (!Competition.isDriverControl() && rightLatch.pressing())) && &bos::bosFns.getEnd() != &bos::bosFns.getCurrent())
    {
      if (bos::bosFns.getCurrent()->lvgl())
      {
        cout << "Clean" << endl;
        // Remove all objects
        lv_obj_clean(lv_scr_act());
        lv_anim_del_all();
      }
      auto oldPtr = &bos::bosFns.getCurrent();
      bos::bosFns.moveCurrentRight();
      auto newPtr = &bos::bosFns.getCurrent();
      if (oldPtr != newPtr && bos::bosFns.getCurrent()->lvgl())
      {
        // Tell it to remake
        bos::bosFns.getCurrent()->call(true);
      }
    }
    auto result = bos::bosFns.getCurrent()->call(false);
    if (bos::bosFns.getCurrent()->lvgl())
    {
      lv_tick_inc(V5_LVGL_RATE);
      lv_task_handler();
    }
    if (result)
    {
      if (bos::bosFns.getCurrent()->lvgl())
      {
        cout << "Clean" << endl;
        // Remove all objects
        lv_obj_clean(lv_scr_act());
        lv_anim_del_all();
      }
      bos::bosFns.popCurrent();
      if (bos::bosFns.getCurrent()->lvgl())
      {
        cout << "remake" << endl;
        // Tell it to remake
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
extern motor ML;
extern pneumatics pto;
int main()
{
  // Init has to be in thread, otherwise it won't work with comp switch
  thread initThread = thread([]()
                             {
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
    wc.path.setK(1.2);
    // flyTBH.setTarget(0);
    flyTBH.setTargetSpeed(0);
    intakeController.disable();
    cout << "<< Flywheel initialized >>" << endl;
    intakeController.autonInit();

    s(500);
    init = true; });
  while (!init)
  {
    s(100);
  }
  [[maybe_unused]] KillThread gpsUpdate = thread(updateSharePos);

  // Make a thread to execute some auton tasks concurrently
  [[maybe_unused]] KillThread otherThreads = thread(executeThreads);

  // TODO: Intake thread
  [[maybe_unused]] KillThread intakeThread = thread(runIntake);
  // Awesome brain screen control thread
  thread loader = thread(brainOS);

  thread flywheelControl = thread(runFlywheel);
  // wc.prevStopExit();
  // wc.driveTo(-20, 48);
  // autonomous();
  // chassis.coastBrake();
  // flyTBH.setTargetSpeed(0);
  // drivercontrol();
  Competition.autonomous(autonomous);
  Competition.drivercontrol(drivercontrol);

  // Prevent main from exiting
  while (1)
  {
    s(300);
  }
}
