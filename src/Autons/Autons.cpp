#define NO_MAKE
#include "src/Autons/Autons.h"

Auton leftA = "Left" + [](){
  cout << "l" << endl;
};
Auton rightA = "Right" + [](){
  cout << "r" << endl;
  wc.estimateStartPos(PVector(41.849730893433815, -61.9659849300323), 0);
  spinRoller();
  wc.followPath(&purePursuit, {PVector(41.84, -58.17), PVector(18.08, -41.12)});
  //Pick up one disk
  wc.turnTo(19.53);
  //Go to the 3 disk stack
  wc.followPath(&ramsete, {PVector(24.45, -52.83), PVector(31.86, -41.64)});
  //Back up to prevent robot from crossing line
  wc.backwardsFollow(&ramsete, {PVector(27.89, -48.01)});
  //Turn to goal
  wc.turnTo(12.91);

};
Auton skills = "Skills" + [](){
  cout << "s" << endl;
};
Auton winPoint = "Win Point" + [](){
  cout << "w" << endl;
  wc.estimateStartPos(PVector(41.34406604747163, -61.455521155830745), 0);
  spinRoller();
  wc.followPath(&ramsete, {PVector(-2.08, -67.89), PVector(7.99, -44.94)});
  //Pick up and fire the disks at the same time
  //TODO

  
  //Forward to back-face the roller
  wc.followPath(&purePursuit, {PVector(-53.43, 9.87), PVector(-53.10, 37.28)});
  //Contact roller
  wc.backwardsFollow(&purePursuit, {PVector(-58.38, 40.42), PVector(-62.18, 40.42)});
  
  spinRoller();

};
Auton purePursuitTest1 = "pptline" + [](){
  // wc.estimateStartPos(PVector(0, 0), 0);
  testPrintData = "pure pursuit line";
  long startTime = Brain.Timer.system();
  wc.followPath(&purePursuit, {PVector(0, 60)});
  auto oldPath = wc.publicPath;
  double timeTaken = static_cast<double>(Brain.Timer.system() - startTime)/1000.0;
  //Get avg velocity (inches per second)
  double avgVel = oldPath.getLength() / timeTaken;
  double avgPctVel = chassis.realToPct(avgVel);
  testPrintData += " time taken: " + toCcp(timeTaken) + "s";
  testPrintData += " avg vel: " + toCcp(avgVel) + "in/s";
  testPrintData += " avg pct vel: " + toCcp(avgPctVel) + "%";
};
Auton purePursuitTest2 = "pptcurve" + [](){
  // wc.estimateStartPos(PVector(0, 0), 0);
  testPrintData = "pure pursuit curve (2 ctrl pts)";
  long startTime = Brain.Timer.system();
  wc.followPath(&purePursuit, {PVector(0, 60), PVector(60, 60)});
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