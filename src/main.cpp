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
#define MAKE
#include "Autons/Autons.h"
#include "BrainOS/ConnectionTest.h"
#include "BrainOS/VariableConfig.h"
#include "ButtonLatch.h"
using namespace ClassFns;

using namespace vex;
void spinRoller()
{
#if BOT == 1
    bool intakeDisabled = intakeController.disabled;
    intakeController.disable();
#endif
    rachetColor.setLightPower(50, percent);
    bool targetRed = wc.isRed();

    auto hue = rachetColor.hue();
    bool isRed = hue > 300 || hue < 60;
    bool lastRed = isRed;
    chassis.driveFromDiff(-60, 0, fwd);
    int count = 0;
    bool countUp = true;
    int i = 0;
    int time = 0;
    while (1)
    {
        if (time > 3000)
        {
            break;
        }
        if (isRed != lastRed && !isRed == targetRed && i > 10)
        {
            break;
        }
        intake.spin(vex::reverse, 40);
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
            chassis.driveFromDiff(40, 0, fwd);
        }
        else if (count == 0)
        {
            countUp = true;
            chassis.driveFromDiff(-100, 0, fwd);
        }
        i++;
        s(10);
        time += 10;
    }
    // intake.spin(vex::reverse, -100);
    chassis.driveFromDiff(-10, 0, fwd);
    // s(500);
    chassis.coastBrake();
    intake.stop(hold);
#if BOT == 1
    if (!intakeDisabled)
    {
        intakeController.enable();
    }
#endif
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
#if BOT == 1
    intakeController.autonInit();
    intakeController.enable();
#endif
    intakeController.enable();
    cout << "Auton Init Done" << endl;
}
void autonomous()
{
    while (!Auton::isSelected())
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
void calibrateFlywheelSpeed()
{
    unsigned int i = 0;
    unsigned int iMod = 10;
    int currentSpeed = 350;
    int direction = 0;
    [[maybe_unused]] ButtonLatch XLatch = ButtonLatch(Greg.ButtonX);
    [[maybe_unused]] ButtonLatch YLatch = ButtonLatch(Greg.ButtonY);
    intakeController.enable();
    while (1)
    {
        if (Greg.ButtonL1.pressing())
        {
            direction = 1;
        }
        else if (Greg.ButtonL2.pressing())
        {
            direction = -1;
        }
        else
        {
            direction = 0;
        }
        if (Greg.ButtonR1.pressing())
        {
            iMod = 10;
        }
        else if (Greg.ButtonR2.pressing())
        {
            iMod = 1;
        }
        i++;
        i %= iMod;
        if (i == 0)
        {
            currentSpeed += direction;
            if (direction != 0)
            {
                cout << "Current Speed: " << currentSpeed << endl;
            }
            flyTBH.setTargetSpeed(currentSpeed);
        }
        if (XLatch.pressing())
        {
            intakeController.intake();
        }
        if (YLatch.pressing())
        {
            intakeController.setFiring();
        }

        s(10);
    }
}
// This class allows a button latch to exist
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
#if BOT == 1
    flyTBH.setTargetSpeed(367);
    flyTBH.setDisabled(false);
    vector<int> speeds = {367, 421, 457, 502};
    int speedIndex = 0;
#endif

    static bool driveReversed = false;
    // Protection from multiple instances of drivercontrol running
    // Is true if there is no instance of drivercontrol running
    static bool allEmpty = false;
    // An array of the current and past instances of drivercontrol
    // The pair::first is whether the instance has exited or not
    // The pair::second is a pointer to the primary bool a few lines down
    static vector<pair<bool, bool*>> countsExist = {};
    // The count of drivercontrol instances
    static int count = 0;
    // Is true if this drivercontrol instance should be running
    bool primary = count == 0 || allEmpty ? true : false;
    // Push back the array
    countsExist.push_back({true, &primary});
    // The index of this drivercontrol instance in countsExist
    int localCount = count;
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
            if (Greg.ButtonLeft.pressing())
            {
                s1 = -50, s2 = 50;
            }
            if (Greg.ButtonRight.pressing())
            {
                s1 = 50, s2 = -50;
            }
            if (s1 == 0)
            {
                leftWheels.stop(coast);
            }
            if (s2 == 0)
            {
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
            if (UpLatch.pressing())
            {
                wc.faceTarget({50, 50});
            }
            if (DownLatch.pressing())
            {
                wc.faceTarget({-50, -50});
            }
            // if(XLatch.pressing()){
            //   wc.turnTo(wc.botPos().angleTo({-50, -50}));
            // }
            // if(YLatch.pressing()){
            //   wc.turnTo(wc.botPos().angleTo({50, 50}));
            // }
            if (Greg.ButtonY.pressing())
            {
                int time = 0;
                while (Greg.ButtonY.pressing())
                {
                    s(10);
                    time += 10;
                    if (time >= 100)
                    {
                        endgame.open();
                        Greg.rumble("....");
                        break;
                    }
                }
            }
#if BOT == 1
            if (L1Latch.pressing())
            {
                speedIndex = (speedIndex + 1) % speeds.size();
                flyTBH.setTargetSpeed(speeds[speedIndex]);
                Greg.rumble(string(speedIndex + 1, '.').data());
            }
            if (L2Latch.pressing())
            {
                speedIndex--;
                if (speedIndex < 0)
                {
                    speedIndex = speeds.size() - 1;
                }
                speedIndex = speedIndex % speeds.size();
                flyTBH.setTargetSpeed(speeds[speedIndex]);
                Greg.rumble(string(speedIndex + 1, '.').data());
            }

            if (Greg.ButtonR1.pressing())
            {
                intakeController.disable();
                intake.spin(fwd, 50);
            }
            else if (Greg.ButtonR2.pressing())
            {
                intakeController.disable();
                intake.spin(vex::reverse, 50);
            }
            else
            {
                intakeController.enable();
                // intake.stop(hold);
            }

            // if (BLatch.pressing()) {
            //   driveReversed = !driveReversed;
            // }

            if (XLatch.pressing())
            {
                intakeController.intake();
            }
            if (BLatch.pressing())
            {
                intakeController.setFiring();
            }
#elif BOT == 2
            if (Greg.ButtonR1.pressing())
            {
                intake.spin(fwd, 100);
            }
            else if (Greg.ButtonR2.pressing())
            {
                intake.spin(vex::reverse, 100);
            }
            else
            {
                intake.stop(hold);
            }
#endif
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
#if BOT == 1
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
        s(20);
    }
}
#endif
//}

// Brain Drawing Stuff {

enum class Alliance : int
{
    red = 0,
    blue = 1
};

void displayBot(bool);
bool init = false;
bool helpAlignBot(bool)
{
    static double desiredAngle = 255;
    // static PVector position = PVector({60.5, -39});
    // position.angleTo({-50, -50});
    static bool init = false;
    if (!init && Auton::isSelected())
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

//}
int main()
{
    // Init has to be in thread, otherwise it won't work with comp switch
    thread initThread = thread([]()
        {
    v5_lv_init();
    cout << "<< Lvgl initialized >>" << endl;
    positioner.init();
    positioner.setPos({0, 0}, 0);
    cout << "<< Odometry initialized >>" << endl;
    testMotorConnection();
    // testMotorConfiguration();
    cout << "<< Motor connection test complete >>" << endl;
    s(500);
    wc.path.setK(1.4);
    chassis.setMaxAcc(200);
    chassis.setMaxDAcc(160);
    cout << "<< Chassis initialized >>" << endl;
#if BOT == 1
    // flyTBH.setTarget(0);
    flyTBH.setTargetSpeed(0);
    intakeController.disable();
    cout << "<< Flywheel initialized >>" << endl;
    VariableConfig setAlliance = VariableConfig({"red", "blue"}, "Alliance", 0, [](int i)
        {
    if (i == 0) {
      wc.setRed();
    }
    else {
      wc.setBlue();
    } });
    BosFn::addNewFn(testConnection);
#if BOT == 1
    BosFn::addNewFn(graphFlywheelTBH);
#endif
    BosFn::addNewFn(displayBot);
#if BOT == 1
    BosFn::addNewFn(BosFn([](bool refresh)
        { intakeController.drawState(refresh); }));
#endif
    BosFn::addNewFn(VariableConfig::drawAll);
    BosFn::addNewFn(windowsLoader);
    BosFn::useTransparentScreenSwitchButtons();
    cout << "<< BrainOS functions initialized >>" << endl;
    intakeController.autonInit();

#endif
    endgame.close();
    s(500);
    init = true; });
    thread posUpdate = thread(updatePos);

    // Make a thread to execute some auton tasks concurrently
    thread otherThreads = thread(executeThreads);
#if BOT == 1
    thread intakeThread = thread(runIntake);
    thread flywheelControl = thread(runFlywheel);
#endif
    // Awesome brain screen control thread
    thread loader = thread(BosFn::runBrainOS);

    // wc.prevStopExit();
    // wc.driveTo(-20, 48);
    // calibrateFlywheelSpeed();
    autonomous();
    // // chassis.coastBrake();
    // flyTBH.setTargetSpeed(0);
    // flyTBH.setDisabled(false);
    // while (1)
    // {
    //   s(100);
    // }
    // drivercontrol();
    // wc.followPath(&purePursuit, {{0, 30}, {30, 30}});
    Competition.autonomous(autonomous);
    Competition.drivercontrol(drivercontrol);

    // Prevent main from exiting
    while (1)
    {
        s(300);
    }
}
