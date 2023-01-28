#include "src/Autons/Autons.h"
#if BOT == 1
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
void launchDisks()
{
    intakeController.setFiring();
    intakeController.waitForFiring();
}
Auton skills = "Skills" + []()
{
    PVector goal = {46, 50};
    wc.setRed();
    flyTBH.setDisabled(false);
    flyTBH.setTargetSpeed(450);
    wc.estimateStartPos(PVector(-61.39, 41.34), 89.36);
    intakeController.enable();
    spinRoller();
    intakeController.enable();
    s(100);

    // Get first disk
    intakeController.intake();
    wc.turnTo(65.04);
    wc.driveDistance(21.29);
    wc.turnTo(90.79);
    launchDisks();
    flyTBH.setTargetSpeed(431);

    // Spin roller
    wc.turnTo(180.67);
    wc.backwardsDriveDistance(10.38);
    spinRoller();

    wc.driveDistance(2.23);

    // Next disk
    intakeController.intake();
    wc.turnTo(125.59);
    wc.driveDistance(37.53);
    wc.turnTo(75.17);
    launchDisks();
    flyTBH.setTargetSpeed(420);

    // Next disk
    intakeController.intake();
    wc.turnTo(127.38);
    wc.driveDistance(12.26);
    wc.turnTo(60.96);
    launchDisks();

    // Next disk
    intakeController.intake();
    flyTBH.setTargetSpeed(420);
    wc.turnTo(133.64);
    wc.driveDistance(19.87);
    wc.turnTo(44.00);
    launchDisks();

    // Big stack
    intakeController.intake();
    flyTBH.setTargetSpeed(450);
    wc.turnTo(125.84);
    wc.driveDistance(38.22);
    wc.turnTo(10.45);
    launchDisks();

    // Roller
    flyTBH.setTargetSpeed(535);
    wc.turnTo(143.77);
    wc.driveDistance(33.70);
    wc.turnTo(269.25);
    wc.backwardsDriveDistance(12);
    spinRoller();

    // Disk
    intakeController.intake();
    wc.driveDistance(9.08);
    wc.turnTo(250.34);
    wc.driveDistance(11.90);
    wc.turnTo(-89.06);
    launchDisks();

    // Last roller
    wc.turnTo(0);
    wc.backwardsDriveDistance(15.27);
    spinRoller();

    // Endgame
    wc.turnTo(-41.99);
    endgame.open();

    // // thread runIntakeThread = thread([]()
    // //                                 {
    // //   while(1){
    // //     if(intaking){
    // //       intake.spin(fwd, 50);
    // //     }
    // //     if ((intakeMiddle.isObjectDetected() && intakeMiddle.objectDistance(inches) < 4) && intaking)
    // //     {
    // //       intaking = false;
    // //       intake.stop(hold);
    // //     }
    // //     s(10);
    // //   } });
    // // botAngles.x = 90;
    // // wc.estimateStartPos(PVector(61.34, -39.07), 271.99);
    // flyTBH.setTargetSpeed(510);
    // // flywheelNm.spin(fwd, 100);
    // wc.estimateStartPos(PVector(-61.39, 41.17), 90);
    // // flywheelNm.stop(coast);

    // wc.turnTo(90);
    // spinRoller();
    // chassis.driveFromDiff(20, 0, fwd);
    // s(700);
    // chassis.coastBrake();

    // // intaking = true;

    // intakeController.intake();
    // wc.turnTo(59.68);
    // wc.driveDistance(12, &pidController);

    // wc.faceTarget(goal);
    // intakeController.setFiring();
    // intakeController.waitForFiring();

    // flyTBH.setTargetSpeed(431);
    // wc.driveTo(-40, 48);
    // wc.backInto(-40.99, 63.09);
    // wc.turnTo(180);
    // chassis.driveFromDiff(-20, 0, fwd);
    // s(700);
    // chassis.coastBrake();
    // spinRoller();
    // chassis.driveFromDiff(20, 0, fwd);
    // s(500);
    // chassis.coastBrake();

    // // intaking = true;

    // intakeController.intake();
    // wc.driveTo(-12, 36);
    // wc.faceTarget(goal);
    // intakeController.setFiring();
    // intakeController.waitForFiring();

    // intakeController.intake();
    // wc.driveTo(-0, 26);
    // wc.faceTarget(goal);
    // intakeController.setFiring();
    // intakeController.waitForFiring();

    // intakeController.intake();
    // flyTBH.setTargetSpeed(425);
    // wc.driveTo(-12, 12);
    // wc.faceTarget(goal);
    // intakeController.setFiring();
    // intakeController.waitForFiring();

    // intakeController.intake();
    // flyTBH.setTargetSpeed(467);
    // wc.driveTo(37, -13);
    // wc.faceTarget(goal);
    // intakeController.setFiring();
    // intakeController.waitForFiring();

    // flyTBH.setTargetSpeed(535);
    // wc.driveTo(50.94, -37.33);
    // intakeController.intake();
    // wc.faceTarget({48, -48});
    // wc.driveDistance(12, &pidController);
    // wc.faceTarget(goal);
    // intakeController.setFiring();
    // intakeController.waitForFiring();

    // wc.backInto(59, -37);
    // wc.turnTo(-89.83);
    // chassis.driveFromDiff(-20, 0, fwd);
    // s(700);
    // chassis.coastBrake();
    // spinRoller();
    // chassis.driveFromDiff(20, 0, fwd);
    // s(500);
    // chassis.coastBrake();

    // wc.driveTo(42.54, -45.91);
    // wc.backInto(42.07, -57.7);
    // wc.turnTo(0);
    // chassis.driveFromDiff(-20, 0, fwd);
    // s(700);
    // chassis.coastBrake();
    // spinRoller();
    // chassis.driveFromDiff(20, 0, fwd);
    // s(500);
    // chassis.coastBrake();

    // wc.faceTarget({-68.79, 69.19});
    // endgame.open();
};
Auton winPoint = "Win Point" + []()
{
    flyTBH.setTargetSpeed(485);
    intakeController.enable();
    wc.estimateStartPos(PVector(-60.36, 40.58), 91.46);
    spinRoller();
    s(20);
    if (intakeController.count == 0)
    {
        intakeController.intake();
    }
    wc.addDistFn(12, []()
        { chassis.setSpeedLimit(50); });
    wc.followPath(&pidController, {PVector(-48.31, 0.46), PVector(-12.31, -11.91)});
    chassis.setSpeedLimit(100);
    s(100);
    wc.faceTarget({49.93, 50.16});
    launchDisks();
    flyTBH.setTargetSpeed(487);
    intakeController.intake();
    wc.driveTo(-0.10, -23.97);
    s(100);
    wc.faceTarget({49.93, 50.16});
    launchDisks();
    intakeController.intake();
    wc.driveTo(40.68, -56.99);
    // wc.backInto(40.68, -61.12);
    wc.turnTo(0);
    wc.backwardsDriveDistance(12);
    spinRoller();
};
Auton skillsPt = "Skils driveTo" + []()
{
    flyTBH.setTargetSpeed(350);
    wc.estimateStartPos(PVector(-62.51, 40.91), 89.23);
    wc.driveTo(-42.04, 51.98);
    wc.faceTarget({49.93, 49.50});
    wc.backInto(-41.71, 62.05);
    // spinRoller();
    intakeController.intake();

    wc.followPath(&purePursuit, {PVector(-41.87, 45.54), PVector(-12.81, 36.29)});
    wc.faceTarget({49.93, 49.50});
    launchDisks();

    intakeController.intake();
    wc.driveTo(0.22, 23.74);
    wc.faceTarget({49.93, 49.50});
    launchDisks();

    intakeController.intake();
    wc.driveTo(12.94, 11.19);
    wc.faceTarget({49.93, 49.50});
    launchDisks();

    intakeController.intake();
    wc.driveTo(36.22, -12.25);
    wc.faceTarget({49.93, 49.50});
    launchDisks();

    wc.driveTo(57.36, -41.47);
    wc.turnTo(-89.85);
    wc.backwardsDriveDistance(4.01);
    wc.driveTo(41.50, -52.04);
    wc.faceTarget({-49.47, -50.06});
    wc.turnTo(0.16);
    wc.backwardsDriveDistance(9.30);
};
#elif BOT == 2
Auton leftA = "Left" + []() {

};
Auton rightA = "Right" + []() {

};
Auton skills = "Skills" + []() {

};
Auton winPoint = "Win Point" + []() {

};
#endif
