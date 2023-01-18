#define NO_MAKE
#include "src/Autons/Autons.h"
void waitIntakeDone()
{
    while (intakeController.clearingDisks || intakeController.intaking)
    {
        wait(20, msec);
    }
}
void waitFlywheelReady(int minTime = 0)
{
    int readyCount = 5;
    bool canExit = false;
    while (readyCount > 0 || minTime > 0)
    {
        if (flyTBH.ready())
        {
            readyCount--;
        }
        else
        {
            readyCount = 8;
        }
        if (readyCount < 0)
        {
            canExit = true;
        }
        if (canExit && minTime < 0)
        {
            break;
        }
        wait(50, msec);
        minTime -= 50;
    }
}
void waitFlywheelNotReady(int minTime = 0)
{
    int readyCount = 5;
    bool canExit = false;
    while (readyCount > 0 || minTime > 0)
    {
        if (!flyTBH.ready())
        {
            readyCount--;
        }
        else
        {
            readyCount = 5;
        }
        if (readyCount < 0)
        {
            canExit = true;
        }
        if (canExit && minTime < 0)
        {
            break;
        }
        wait(50, msec);
        minTime -= 50;
    }
}
void graphFlywheelTBH(bool);
Auton leftA = "Left" + []()
{
    // flyTBH.setDisabled(true);
    // bos::bosFns.moveCurrentLeft();
    // bos::bosFns.moveCurrentLeft();
    LinkedList<bool> flywheelReady = {};
    // bos::bosFns.moveCurrentLeft();
    // bos::bosFns.moveCurrentLeft();
    // bos::bosFns.getCurrent()->call(true);
    intakeController.disable();
    flyTBH.setTargetSpeed(528);
    flyTBH.setDisabled(true);
    s(100);
    flywheelNm.spinVolt(fwd, 528.0 / 6.0);
    // flywheelNm.spin(fwd, 100);
    wc.estimateStartPos(PVector(-61.39, 41.17), 88.59);

    // wc.faceTarget({49.60, 49.88});
    // wc.faceTarget({49.60, 49.88});
    // intake.spin(fwd, 100);
    // s(2000);
    // flyTBH.setDisabled(false);
    s(100);
    // Fire
    // waitFlywheelReady();
    s(3500);
    cout << "0" << endl;
    intake.spin(fwd, 50);
    s(600);
    cout << "1" << endl;
    intake.stop(hold);
    // spinRoller();
    // chassis.driveFromDiff(20, 0, fwd);
    // s(300);
    // chassis.coastBrake();

    // wc.faceTarget({49.60, 49.88});
    // s(500);
    // waitFlywheelReady();
    cout << 1.5 << endl;
    s(3000);
    intake.spin(fwd, 50);
    // waitFlywheelNotReady(1000);
    s(1000);
    cout << "2" << endl;
    intake.stop(hold);
    // intakeController.setFiring();
    // waitIntakeDone();
    flyTBH.setTargetSpeed(0);
    wc.turnTo(90);
    spinRoller();
    flyTBH.setDisabled(false);
    // // // Drive to 3-stack
    // // goalRaise.open();
    // wc.driveTo(-30.48, 6.59);

    // // goalRaise.close();
    // // intakeController.intakeMultiple(3);
    // wc.faceTarget({49.42, 50.22});
    // // // Fire
    // intake.spin(fwd, 60);
    // intakeController.setFiring();
};
Auton rightA = "Right" + []()
{
    // wc.setBlue();
    cout << "Red: " << wc.isRed() << endl;
    intakeController.disable();
    wc.estimateStartPos(PVector(16.81, -62.47), 0);
    // pidController.settings.setBrakeMode(WheelController::exitMode::hold);
    // bos::bosFns.moveCurrentLeft();
    // bos::bosFns.moveCurrentLeft();
    // bos::bosFns.getCurrent()->call(true);
    flyTBH.setTargetSpeed(530);
    flyTBH.setDisabled(true);
    s(100);
    flywheelNm.spinVolt(fwd, 533.0 / 6.0);
    // flywheelNm.spin(fwd, 100);
    wc.estimateStartPos(PVector(-61.39, 41.17), 88.59);

    // wc.faceTarget({49.60, 49.88});
    // wc.faceTarget({49.60, 49.88});
    // intake.spin(fwd, 100);
    // s(2000);
    // flyTBH.setDisabled(false);
    s(100);
    // Fire
    // waitFlywheelReady();
    s(3500);
    cout << "0" << endl;
    intake.spin(fwd, 50);
    s(600);
    cout << "1" << endl;
    intake.stop(hold);

    flyTBH.setTargetSpeed(0);
    flyTBH.setDisabled(false);

    // // Fire
    // intakeController.setFiring();
    // // Move fwd to allow for roller
    wc.driveTo(21.59, -44.32);
    // // Drive to roller
    // wc.backwardsFollow(&pidController, {PVector(39.01, -56.1)});
    pidController.settings.setBrakeMode(WheelController::exitMode::hold);
    wc.backInto(37.18, -56.76);
    wc.turnTo(-10);
    chassis.driveFromDiff(-20, 0, fwd);
    s(300);
    chassis.coastBrake();
    spinRoller();
    // // Drive along 3-ground
    // wc.followPath(&pidController, {PVector(-14.26, -11.07)});
    // wc.faceTarget({48.57, 50.05});
    // // Fire
    // intakeController.setFiring();
};
bool intaking = false;
void forceGpsUpdate()
{
    int time = 0;
    while (share.timeSinceLastGpsMs() > 3000)
    {
        s(10);
        time += 10;
        if (time > 2000)
        {
            cout << "GPS update fail" << endl;
            break;
        }
    }
}
Auton skills = "Skills" + []()
{
    PVector goal = {46, 50};
    wc.setRed();
    // thread runIntakeThread = thread([]()
    //                                 {
    //   while(1){
    //     if(intaking){
    //       intake.spin(fwd, 50);
    //     }
    //     if ((intakeMiddle.isObjectDetected() && intakeMiddle.objectDistance(inches) < 4) && intaking)
    //     {
    //       intaking = false;
    //       intake.stop(hold);
    //     }
    //     s(10);
    //   } });
    // botAngles.x = 90;
    // wc.estimateStartPos(PVector(61.34, -39.07), 271.99);
    flyTBH.setTargetSpeed(510);
    // flywheelNm.spin(fwd, 100);
    wc.estimateStartPos(PVector(-61.39, 41.17), 90);
    // flywheelNm.stop(coast);

    wc.turnTo(90);
    spinRoller();
    chassis.driveFromDiff(20, 0, fwd);
    s(700);
    chassis.coastBrake();
    forceGpsUpdate();

    // intaking = true;

    intakeController.intake();
    wc.driveTo(-40.66, 56.97);
    forceGpsUpdate();
    wc.faceTarget(goal);
    intakeController.setFiring();
    intakeController.waitForFiring();

    flyTBH.setTargetSpeed(431);
    wc.backInto(-40.99, 63.09);
    wc.turnTo(180);
    chassis.driveFromDiff(-20, 0, fwd);
    s(700);
    chassis.coastBrake();
    spinRoller();
    chassis.driveFromDiff(20, 0, fwd);
    s(500);
    chassis.coastBrake();
    forceGpsUpdate();

    // intaking = true;

    intakeController.intake();
    wc.driveTo(-12, 36);
    forceGpsUpdate();
    wc.faceTarget(goal);
    intakeController.setFiring();
    intakeController.waitForFiring();

    intakeController.intake();
    wc.driveTo(-6, 26);
    forceGpsUpdate();
    wc.faceTarget(goal);
    intakeController.setFiring();
    intakeController.waitForFiring();

    intakeController.intake();
    flyTBH.setTargetSpeed(425);
    wc.driveTo(-3, 17);
    wc.faceTarget(goal);
    intakeController.setFiring();
    intakeController.waitForFiring();
    forceGpsUpdate();

    intakeController.intake();
    flyTBH.setTargetSpeed(467);
    wc.driveTo(37, -13);
    forceGpsUpdate();
    wc.faceTarget(goal);
    intakeController.setFiring();
    intakeController.waitForFiring();

    flyTBH.setTargetSpeed(535);
    wc.driveTo(50.94, -37.33);
    // intakeController.intake();
    // wc.driveTo(45.40, -50.25);
    // forceGpsUpdate();
    // wc.faceTarget(goal);
    // intakeController.setFiring();
    // intakeController.waitForFiring();

    wc.backInto(59, -37);
    wc.turnTo(-89.83);
    chassis.driveFromDiff(-20, 0, fwd);
    s(700);
    chassis.coastBrake();
    spinRoller();
    chassis.driveFromDiff(20, 0, fwd);
    s(500);
    chassis.coastBrake();
    forceGpsUpdate();

    wc.driveTo(42.54, -45.91);
    wc.backInto(42.07, -57.7);
    wc.turnTo(0);
    chassis.driveFromDiff(-20, 0, fwd);
    s(700);
    chassis.coastBrake();
    spinRoller();
    chassis.driveFromDiff(20, 0, fwd);
    s(500);
    chassis.coastBrake();

    wc.faceTarget({-68.79, 69.19});
    endgame.open();
};
Auton winPoint = "Win Point" + []()
{
    wc.estimateStartPos(PVector(-60.26, 40.17), 102.22);
    wc.followPath(&pidController, {PVector(-44.54, 0.13), PVector(18.49, -50.82)});
    wc.driveTo(21.59, -44.32);
    wc.backwardsFollow(&pidController, {PVector(41.18, -64.76)});
    wc.turnTo(0);
};
