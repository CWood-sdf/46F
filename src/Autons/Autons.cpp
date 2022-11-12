#define NO_MAKE
#include "src/Autons/Autons.h"

Auton leftA = "Left" + [](){
  wc.estimateStartPos({60, -36}, 90);
  rachetColor.setLightPower(50, percent);
  flyTBH.setTargetSpeed(520);
  flyTBH.setDisabled(true);
  auto acc = flyTBH.maxRateGain;
  flyTBH.maxRateGain = 100;
  flywheelNm.spin(fwd, 52000/600);
  s(2000);
  flyTBH.setDisabled(false);
  flyTBH.maxRateGain = acc;
  s(50);
  
  // wc.faceTarget({-40.79, 50.06});
  while(!flyTBH.ready()){
    s(10);
  }
  intake.spin(fwd, 100);
  s(5000);
  intake.stop(hold);
  flyTBH.setTargetSpeed(0);
  pidController.setTurn();
  wc.setExitDist(3.0);
  wc.turnTo(-90);
  chassis.driveFromDiff(-20, 0, fwd);
  intake.spin(vex::reverse, 100);
  s(3000);
  intake.stop(hold);
  rachetColor.setLightPower(0, percent);
};
Auton rightA = "Right" + [](){
  flyTBH.setTargetSpeed(500);
  flyTBH.setDisabled(true);
  auto acc = flyTBH.maxRateGain;
  flyTBH.maxRateGain = 100;
  flywheelNm.spin(fwd, 52000/600);
  s(2000);
  flyTBH.setDisabled(false);
  flyTBH.maxRateGain = acc;
  s(50);
  
  // wc.faceTarget({-40.79, 50.06});
  while(!flyTBH.ready()){
    s(10);
  }
  intake.spin(fwd, 100);
  s(3000);
  intake.stop(hold);
  flyTBH.setTargetSpeed(0);

  wc.estimateStartPos(PVector(-17.60, 65.52), 180);
  
  wc.driveTo(-17.7, 12.08+65.52);
  wc.backwardsFollow(&pidController, {PVector(-26.68, 11.42)+PVector(-17.60, 65.52)});
  wc.backwardsFollow(&pidController, {PVector(-24.04, 4.48)+PVector(-17.60, 65.52)});
  spinRoller();

};
Auton skills = "Skills" + [](){
  cout << "s" << endl;
};
// Auton winPoint = "Win Point" + [](){
//   wc.estimateStartPos(PVector(-61.61, 41.32), 90.03);
//   spinRoller();
//   wc.faceTarget({49.77, 50.01});
//   //TODO: fire disks
//   wc.followPath(&pidController, {PVector(-25.96, 0.35)});
//   wc.faceTarget({49.77, 50.01});
//   //TODO: fire disks
//   wc.followPath(&purePursuit, {PVector(18.20, -47.89), PVector(31.32, -46.3)});
//   wc.backwardsFollow(&purePursuit, {PVector(39.48, -53.04), PVector(40.02, -61.37)});
//   spinRoller();

// };
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