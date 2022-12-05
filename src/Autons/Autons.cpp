#define NO_MAKE
#include "src/Autons/Autons.h"

Auton leftA = "Left" + []()
{
  wc.estimateStartPos(PVector(-61.39, 41.17), 88.59);
  spinRoller();
  // Fire
  wc.faceTarget({49.60, 49.88});
  intakeController.setFiring();
  // Drive to 3-stack
  goalRaise.open();
  wc.driveTo(-22.45, -3.39);
  goalRaise.close();
  intakeController.intakeMultiple(3);
  wc.faceTarget({49.42, 50.22});
  // Fire
  intakeController.setFiring();
};
Auton rightA = "Right" + []()
{
  wc.estimateStartPos(PVector(16.81, -62.47), 0);
  wc.faceTarget({48.57, 50.05});
  // Fire
  intakeController.setFiring();
  // Move fwd to allow for roller
  wc.driveTo(21.59, -42.32);
  // Drive to roller
  wc.backwardsFollow(&pidController, {PVector(39.01, -54.1)});
  wc.backwardsFollow(&pidController, {PVector(39.18, -60.76)});
  spinRoller();
  // Drive along 3-ground
  wc.followPath(&pidController, {PVector(-14.26, -11.07)});
  wc.faceTarget({48.57, 50.05});
  // Fire
  intakeController.setFiring();
};
Auton skills = "Skills" + []()
{
  cout << "s" << endl;
};
Auton winPoint = "Win Point" + []()
{
  wc.estimateStartPos(PVector(-61.39, 41.17), 88.59);
  spinRoller();
  // Fire
  wc.faceTarget({49.60, 49.88});
  intakeController.setFiring();
  // Drive to 3-stack
  goalRaise.open();
  wc.driveTo(-22.45, -3.39);
  goalRaise.close();
  intakeController.intakeMultiple(3);
  wc.faceTarget({49.60, 49.88});
  // Fire disks
  intakeController.setFiring();
  wc.followPath(&purePursuit, {PVector(25.52, -52.57), PVector(41.23, -57.52)});
  wc.backwardsFollow(&pidController, {PVector(41.23, -60.59)});
  spinRoller();
};
// Auton purePursuitTest1 = "pptline" + [](){
//   // wc.estimateStartPos(PVector(0, 0), 0);
//   testPrintData = "pure pursuit line";
//   // leftWhls.spinVolt(fwd, 100);
//   // rghtWhls.spinVolt(fwd, 100);
//   // s(500);
//   // chassis.coastBrake();
//   long startTime = Brain.Timer.system();
//   wc.followPath(&pidController, {PVector(0, -48)});
//   auto oldPath = wc.publicPath;
//   double timeTaken = static_cast<double>(Brain.Timer.system() - startTime)/1000.0;
//   //Get avg velocity (inches per second)
//   double avgVel = oldPath.getLength() / timeTaken;
//   double avgPctVel = chassis.realToPct(avgVel);
//   testPrintData += " \ntime taken: " + toCcp(timeTaken) + "s";
//   testPrintData += " \navg vel: " + toCcp(avgVel) + "in/s";
//   testPrintData += " \navg pct vel: " + toCcp(avgPctVel) + "%";
//   // s(40000);
// };
// Auton purePursuitTest2 = "pptcurve" + [](){
//   // wc.estimateStartPos(PVector(0, 0), 0);
//   testPrintData = "pure pursuit curve (2 ctrl pts)";
//   long startTime = Brain.Timer.system();
//   wc.setExitDist(6);
//   wc.followPath(&purePursuit, {PVector(0, -48), PVector(-48, -48)});
//   auto oldPath = wc.publicPath;
//   double timeTaken = static_cast<double>(Brain.Timer.system() - startTime)/1000.0;
//   //Get avg velocity (inches per second)
//   double avgVel = oldPath.getLength() / timeTaken;
//   double avgPctVel = chassis.realToPct(avgVel);
//   testPrintData += " time taken: " + toCcp(timeTaken) + "s";
//   testPrintData += " avg vel: " + toCcp(avgVel) + "in/s";
//   testPrintData += " avg pct vel: " + toCcp(avgPctVel) + "%";
// };
// Auton purePursuitTest3 = "pptcurve2" + [](){
//   // wc.estimateStartPos(PVector(0, 0), 0);
//   testPrintData = "pure pursuit curve (3 ctrl pts)";
//   long startTime = Brain.Timer.system();
//   wc.followPath(&purePursuit, {PVector(0, 60), PVector(60, 60), PVector(60, 0)});
//   auto oldPath = wc.publicPath;
//   double timeTaken = static_cast<double>(Brain.Timer.system() - startTime)/1000.0;
//   //Get avg velocity (inches per second)
//   double avgVel = oldPath.getLength() / timeTaken;
//   double avgPctVel = chassis.realToPct(avgVel);
//   testPrintData += " time taken: " + toCcp(timeTaken) + "s";
//   testPrintData += " avg vel: " + toCcp(avgVel) + "in/s";
//   testPrintData += " avg pct vel: " + toCcp(avgPctVel) + "%";
// };
// Auton purePursuitTest4 = "pptcurve3" + [](){
//   // wc.estimateStartPos(PVector(0, 0), 0);
//   testPrintData = "pure pursuit curve (4 ctrl pts)";
//   long startTime = Brain.Timer.system();
//   wc.followPath(&purePursuit, {PVector(0, 60), PVector(60, 60), PVector(60, 0), PVector(0, 0)});
//   auto oldPath = wc.publicPath;
//   double timeTaken = static_cast<double>(Brain.Timer.system() - startTime)/1000.0;
//   //Get avg velocity (inches per second)
//   double avgVel = oldPath.getLength() / timeTaken;
//   double avgPctVel = chassis.realToPct(avgVel);
//   testPrintData += " time taken: " + toCcp(timeTaken) + "s";
//   testPrintData += " avg vel: " + toCcp(avgVel) + "in/s";
//   testPrintData += " avg pct vel: " + toCcp(avgPctVel) + "%";
// };