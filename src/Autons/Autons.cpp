#define NO_MAKE
#include "src/Autons/Autons.h"

Auton leftA = "Left" + [](){
  wc.estimateStartPos(PVector(-61.61, 41.32), 90.03);
  spinRoller();
  wc.faceTarget({49.77, 50.01});
  //TODO: fire disks
  wc.followPath(&ramsete, {PVector(-49.73, 24.83), PVector(-42.81, 31.39)});
  wc.backwardsFollow(&pidController, {PVector(-46.54, 27.84)});
  wc.faceTarget({49.77, 50.01});
  //TODO: fire disks
  wc.followPath(&pidController, {PVector(-28.80, 2.83)});
  wc.faceTarget({49.77, 50.01});
  //TODO: fire disks

};
Auton rightA = "Right" + [](){
  wc.estimateStartPos(PVector(13.05, -61.73), 222.54);
  wc.backwardsFollow(&purePursuit, {PVector(24.23, -50.38), PVector(41.26, -52.15), PVector(41.44, -61.55)});
  spinRoller();
  wc.faceTarget({49.06, 49.84});
  //TODO: fire disks
  wc.followPath(&ramsete, {PVector(22.46, -52.68), PVector(30.97, -42.22)});
  wc.backwardsFollow(&pidController, {PVector(28.13, -46.12)});
  wc.faceTarget({49.06, 49.84});
  //TODO: fire disks
  wc.followPath(&purePursuit, {PVector(17.67, -43.99), PVector(-12.12, -11.35)});
  wc.faceTarget({49.06, 49.84});
  //TODO: fire disks

};
Auton skills = "Skills" + [](){
  cout << "s" << endl;
};
Auton winPoint = "Win Point" + [](){
  wc.estimateStartPos(PVector(-61.61, 41.32), 90.03);
  spinRoller();
  wc.faceTarget({49.77, 50.01});
  //TODO: fire disks
  wc.followPath(&pidController, {PVector(-25.96, 0.35)});
  wc.faceTarget({49.77, 50.01});
  //TODO: fire disks
  wc.followPath(&purePursuit, {PVector(18.20, -47.89), PVector(31.32, -46.3)});
  wc.backwardsFollow(&purePursuit, {PVector(39.48, -53.04), PVector(40.02, -61.37)});
  spinRoller();

};
Auton purePursuitTest1 = "pptline" + [](){
  // wc.estimateStartPos(PVector(0, 0), 0);
  testPrintData = "pure pursuit line";
  // leftWhls.spinVolt(fwd, 100);
  // rghtWhls.spinVolt(fwd, 100);
  // s(500);
  // chassis.coastBrake();
  long startTime = Brain.Timer.system();
  wc.followPath(&pidController, {PVector(0, -48)});
  auto oldPath = wc.publicPath;
  double timeTaken = static_cast<double>(Brain.Timer.system() - startTime)/1000.0;
  //Get avg velocity (inches per second)
  double avgVel = oldPath.getLength() / timeTaken;
  double avgPctVel = chassis.realToPct(avgVel);
  testPrintData += " \ntime taken: " + toCcp(timeTaken) + "s";
  testPrintData += " \navg vel: " + toCcp(avgVel) + "in/s";
  testPrintData += " \navg pct vel: " + toCcp(avgPctVel) + "%";
  // s(40000);
};
Auton purePursuitTest2 = "pptcurve" + [](){
  // wc.estimateStartPos(PVector(0, 0), 0);
  testPrintData = "pure pursuit curve (2 ctrl pts)";
  long startTime = Brain.Timer.system();
  wc.setExitDist(6);
  wc.followPath(&purePursuit, {PVector(0, -48), PVector(-48, -48)});
  auto oldPath = wc.publicPath;
  double timeTaken = static_cast<double>(Brain.Timer.system() - startTime)/1000.0;
  //Get avg velocity (inches per second)
  double avgVel = oldPath.getLength() / timeTaken;
  double avgPctVel = chassis.realToPct(avgVel);
  testPrintData += " time taken: " + toCcp(timeTaken) + "s";
  testPrintData += " avg vel: " + toCcp(avgVel) + "in/s";
  testPrintData += " avg pct vel: " + toCcp(avgPctVel) + "%";
};
Auton purePursuitTest3 = "pptcurve2" + [](){
  // wc.estimateStartPos(PVector(0, 0), 0);
  testPrintData = "pure pursuit curve (3 ctrl pts)";
  long startTime = Brain.Timer.system();
  wc.followPath(&purePursuit, {PVector(0, 60), PVector(60, 60), PVector(60, 0)});
  auto oldPath = wc.publicPath;
  double timeTaken = static_cast<double>(Brain.Timer.system() - startTime)/1000.0;
  //Get avg velocity (inches per second)
  double avgVel = oldPath.getLength() / timeTaken;
  double avgPctVel = chassis.realToPct(avgVel);
  testPrintData += " time taken: " + toCcp(timeTaken) + "s";
  testPrintData += " avg vel: " + toCcp(avgVel) + "in/s";
  testPrintData += " avg pct vel: " + toCcp(avgPctVel) + "%";
};
Auton purePursuitTest4 = "pptcurve3" + [](){
  // wc.estimateStartPos(PVector(0, 0), 0);
  testPrintData = "pure pursuit curve (4 ctrl pts)";
  long startTime = Brain.Timer.system();
  wc.followPath(&purePursuit, {PVector(0, 60), PVector(60, 60), PVector(60, 0), PVector(0, 0)});
  auto oldPath = wc.publicPath;
  double timeTaken = static_cast<double>(Brain.Timer.system() - startTime)/1000.0;
  //Get avg velocity (inches per second)
  double avgVel = oldPath.getLength() / timeTaken;
  double avgPctVel = chassis.realToPct(avgVel);
  testPrintData += " time taken: " + toCcp(timeTaken) + "s";
  testPrintData += " avg vel: " + toCcp(avgVel) + "in/s";
  testPrintData += " avg pct vel: " + toCcp(avgPctVel) + "%";
};