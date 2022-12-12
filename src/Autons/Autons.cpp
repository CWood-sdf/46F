#define NO_MAKE
#include "src/Autons/Autons.h"
void waitIntakeDone()
{
  while (intakeController.clearingDisks || intakeController.clearingLastDisk || intakeController.intaking)
  {
    wait(20, msec);
  }
}
Auton leftA = "Left" + []()
{
  // flyTBH.setDisabled(true);

  flyTBH.setTargetSpeed(520);
  // flywheelNm.spin(fwd, 100);
  wc.estimateStartPos(PVector(-61.39, 41.17), 88.59);
  wc.faceTarget({49.60, 49.88});
  wc.faceTarget({49.60, 49.88});
  // intake.spin(fwd, 100);
  // s(2000);
  // flyTBH.setDisabled(false);
  s(100);
  // Fire
  intakeController.disable();
  while (!flyTBH.ready())
  {
    wait(20, msec);
  }
  intake.spin(fwd, 100);
  s(100);
  intake.stop(hold);
  s(500);
  while (!flyTBH.ready())
  {
    wait(20, msec);
  }
  intake.spin(fwd, 100);
  while (!flyTBH.ready())
  {
    wait(20, msec);
  }
  intake.stop(hold);
  // intakeController.setFiring();
  // waitIntakeDone();
  // flyTBH.setTargetSpeed(0);
  wc.turnTo(90);
  spinRoller();
  // // Drive to 3-stack
  // goalRaise.open();
  // wc.driveTo(-30.48, 6.59);

  // goalRaise.close();
  // intakeController.intakeMultiple(3);
  // wc.faceTarget({49.42, 50.22});
  // // Fire
  // intakeController.setFiring();
};
Auton rightA = "Right" + []()
{
  wc.estimateStartPos(PVector(16.81, -62.47), 0);
  wc.faceTarget({48.57, 50.05});
  // // Fire
  // intakeController.setFiring();
  // // Move fwd to allow for roller
  // wc.driveTo(21.59, -42.32);
  // // Drive to roller
  // wc.backwardsFollow(&pidController, {PVector(39.01, -54.1)});
  // wc.backwardsFollow(&pidController, {PVector(39.18, -60.76)});
  // spinRoller();
  // // Drive along 3-ground
  // wc.followPath(&pidController, {PVector(-14.26, -11.07)});
  // wc.faceTarget({48.57, 50.05});
  // // Fire
  // intakeController.setFiring();
};
Auton skills = "Skills" + []()
{
  /***
   * TODO: Test like this:
   * - Work out wheelbase mvts
   * - Add intake and goal raise
   * - Test one cmd at a time
   *
   */
  wc.estimateStartPos(PVector(59.34, -38.83), 270.53);
  wc.faceTarget({-49.96, -50.22});
  // Fire
  // intakeController.setFiring();
  wc.turnTo(-90);
  spinRoller();
  // // Drive to 3-stack
  // wc.driveTo(35.21, -36.43);
  // wc.faceTarget({-49.27, -50.13});
  // // Fire
  // intakeController.setFiring();
  // // Continue to roller
  // wc.backwardsFollow(&pidController, {PVector(40.16, -58.45)});
  // wc.turnTo(-0.41);
  // spinRoller();
  // // Drive to 3-row
  // wc.followPath(&pidController, {PVector(21.06, -47.21), PVector(-15.34, -7.66)});
  // wc.faceTarget({-49.72, -49.68});
  // // Fire
  // intakeController.setFiring();
  // // Drive to 3-stack
  // wc.driveTo(-36.91, 12.78);
  // wc.faceTarget({-49.72, -49.68});
  // // Fire
  // intakeController.setFiring();
  // // Drive to roller
  // wc.backwardsFollow(&pidController, {PVector(-57.14, 38.84)});
  // wc.backwardsFollow(&pidController, {PVector(-60.96, 39.07)});
  // spinRoller();
  // // Drive to next roller, prolly picks up a disk, but whatevs
  // wc.driveTo(-41.63, 53.45);
  // wc.backwardsFollow(&pidController, {PVector(-41.63, 60.87)});
  // spinRoller();
  // // Drive to 3-stack
  // wc.driveTo(-35.79, 37.27);
  // // Drive to closer to goal
  // wc.driveTo(-9.72, 53.45);
  // wc.faceTarget({50.49, 49.86});
  // // Fire
  // intakeController.setFiring();
  // // Drive along 3-row
  // wc.backwardsFollow(&pidController, {PVector(-23.88, 44.91)});
  // wc.driveTo(19.71, 3.56);
  // wc.faceTarget({50.49, 49.86});
  // // Fire
  // intakeController.setFiring();
  // // Drive to 3-stack
  // wc.driveTo(45.10, -20.25);
  // wc.faceTarget({49.37, 50.31});
  // // Fire
  // intakeController.setFiring();
  // // Endgame time!
  // wc.backwardsFollow(&pidController, {PVector(59.48, -60.25)});
  // // Face the endgame direction
  // wc.turnTo(-44.79);
  // // Fire endgame
};
Auton winPoint = "Win Point" + []()
{
  wc.estimateStartPos(PVector(57.46, -38.45), 269.79);
  wc.faceTarget({-49.05, -50.13});
  wc.turnTo(269.62);
  spinRoller();
  wc.driveTo(30.72, -6.09);
  wc.faceTarget({-50.40, -49.46});
  wc.driveTo(-17.14, 41.09);
  wc.faceTarget({-50.40, -49.46});
  wc.backwardsFollow(&pidController, {PVector(-40.28, 57.95)});
  wc.backwardsFollow(&pidController, {PVector(-40.51, 61.32)});
  spinRoller();
};
