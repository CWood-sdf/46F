#define NO_MAKE
#include "Odometry/EPA_Wheel_Control.h"
double sign(double v){
  if(v == 0.0){
    return 0;
  }
  return v / abs(v);
}
void BasicWheelController::addDrivePid(PidAdder a){
  ctrl += a;
  customPidsDrive.push(a);
  
}
void BasicWheelController::addDrivePid(double p, double i, double d){
  addDrivePid(PidAdder(p, i, d));
}
void BasicWheelController::popTopDrivePid(){
  auto& a = customPidsDrive.getEnd();
  ctrl -= a;
  customPidsTurn.popEnd();
}
void BasicWheelController::addSlavePid(PidAdder a){
  slaveCtrl += a;
  customPidsSlave.push(a);
  cout << slaveCtrl << endl;
}
void BasicWheelController::addSlavePid(double p, double i, double d){
  addSlavePid(PidAdder(p, i, d));
}
void BasicWheelController::popTopSlavePid(){
  auto& a = customPidsSlave.getEnd();
  slaveCtrl -= a;
  customPidsSlave.popEnd();
}
void BasicWheelController::addTurnPid(PidAdder a){
  turnCtrl += a;
  customPidsTurn.push(a);
  cout << turnCtrl << endl;
  
}
void BasicWheelController::addTurnPid(double p, double i, double d){
  addTurnPid(PidAdder(p, i, d));
}

void BasicWheelController::driveTo(double x, double y){
  driveTo(PVector(x, y));
}
void BasicWheelController::backInto(double x, double y){
  backInto(PVector(x, y));
}
double BasicWheelController::botAngle (){
  return pos.heading();
}
PVector& BasicWheelController::botPos(){
  return pos.position();
}
//Add a function to be called at a specified distance
void BasicWheelController::addDistFn(double dist, std::function<void()> fn){
  distFns[dist] = fn;
}
//Reuse the old map
void BasicWheelController::reuseDistFns(){
  distFns = oldFns;
}
void BasicWheelController::setFn(std::function<void()> fn){
  afterTurn = fn;
  hasFn = true;
}
void BasicWheelController::callFn(){
  if(hasFn){
    afterTurn();
  }
  hasFn = false;
}
void BasicWheelController::reuseFn(){
  hasFn = true;
}

//A hard brake
void BasicWheelController::hardBrake(){
  Left.stop(hold);
  Right.stop(hold);
}
PID BasicWheelController::getCtrl(){
  return ctrl;
}
PID BasicWheelController::getSlave(){
  return slaveCtrl;
}  
void BasicWheelController::setOldDistFns(){
  oldFns = distFns;
}
void BasicWheelController::useDistFns(double dist){
  for (auto it = distFns.begin(); it != distFns.end() /* not hoisted */; /* no increment */)
  {
    if ((*it).first < dist)
    {

      (*it).second();
      distFns.erase(it++);    // or "it = m.erase(it)" since C++11
    }
    else
    {
      ++it;
    }
  }
  
}
void BasicWheelController::turnTo(std::function<double()> angleCalc){
  //
  auto oldAngleCalc = angleCalc;
  if(reversed && !callingInDrive){
    #ifndef USE_GAME_SPECIFIC
    #warning GSD (Turning add)
    #endif
    angleCalc = [&](){
      return oldAngleCalc() + 180.0;
    };
  }
  //If the auton is on the other side, turn to the opposite angle
  double angle = angleCalc();
  cout << "A: " << angle << endl;
  cout << "A2: " << botAngle() << endl;
  // if(!callingInDrive && reversed){
  //   angle += 90;
  // }
  int timeIn = 0;
  int i = 0;
  //Get the normAngle
  double normAngle = posNeg180(angle - posNeg180(botAngle()));
  //init PID
  turnCtrl.setTarget(0);
  int sleepTime = 30;
  int minTimeIn = 150;
  double degRange = 4.0;
  int speedLimit = 60;
  
  // 
  // 
  // 
  cout << normAngle << endl;
  if(normAngle < -degRange){
    //It is off by 4 degrees, because:
    //    A: if it is within 4 degrees, without the minSpeed 
    //          it is too slow; but, with minSpeed, it overshoots too much
    while(timeIn * sleepTime < minTimeIn){
      
      double s = turnCtrl.getVal(normAngle);
      turnLeft(s > speedLimit ? speedLimit : s);
      task::sleep(sleepTime);
      
      angle = angleCalc();
      
      normAngle = posNeg180(angle - botAngle());
      if(abs(normAngle) < degRange){
        timeIn++;
      }
      else {
        timeIn = 0;
      }
      // 
    }
    hardBrake();
  } else if(normAngle > degRange){
    while(timeIn * sleepTime < minTimeIn){
      
      double s = -turnCtrl.getVal(normAngle);
      turnRight(s > speedLimit ? speedLimit : s);
      task::sleep(sleepTime);
      
      angle = angleCalc();
      // 
      normAngle = posNeg180(angle - botAngle());
      if(abs(normAngle) < degRange){
        timeIn++;
      }
      else {
        timeIn = 0;
      }
    }
    hardBrake();
  }
  hardBrake();
  s(300);

  cout << botAngle() << endl;
  cout << angleCalc() << endl;

  // hardBrake();
  // s(500);
  // normAngle = posNeg180(angle - posNeg180(botAngle()));
  
}
void BasicWheelController::turnTo(double angle){
  turnTo([=](){ return angle; });
}
void BasicWheelController::driveTo(PVector pos){
  
  VectorArr arr = {pos};
  
  
  followPath(arr);
}

bool BasicWheelController::isMoving(){
  return moving;
}
BasicWheelController::chain_method BasicWheelController::estimateStartPos(PVector v, double a){
  cout << sign(botPos().x) << ", " << sign(v.x) << endl;
  if(botPos().mag() < 3){
    pos.setPos(v, a);
  }
  #ifndef USE_GAME_SPECIFIC
  #warning GSD (Conditions for reversing auton)
  #endif
  else if(sign(botPos().x) != sign(v.x)){
    reversed = true;
    cout << "Reversing auton" << endl;
  }
  else {
    reversed = false;
  }
  CHAIN;
}
void BasicWheelController::preventTurn(){
  turnPrevent = true;
}
BasicWheelController::chain_method BasicWheelController::forceEarlyExit(){
  exitEarly = true;
  CHAIN;
}
BasicWheelController::chain_method BasicWheelController::setSpeedLimit(double v){
  speedLimit = v;
  CHAIN;
}
BasicWheelController::chain_method BasicWheelController::setMaxAcc(double v){
  maxAcc = v;
  CHAIN;
}
BasicWheelController::chain_method BasicWheelController::setMaxDeAcc(double v){
  maxDAcc = v;
  CHAIN;
}
BasicWheelController::chain_method BasicWheelController::setExitMode(exitMode m){
  BrakeMode = m;
  CHAIN;
}
BasicWheelController::chain_method BasicWheelController::setKConst(double v){
  kConst = v;
  CHAIN;
}
BasicWheelController::chain_method BasicWheelController::setExitDist(double v){
  exitDist = v;
  CHAIN;
}
PVector BasicWheelController::getLastTarget(){
  return lastTarget;
}
BasicWheelController::chain_method BasicWheelController::prevStopExit(){
  stopExitPrev = true;
  CHAIN
}
BasicWheelController::chain_method BasicWheelController::setPathRadius(double r){
  pathRadius = r;
  CHAIN
}

size_t BasicWheelController::getNearest(VectorArr arr, PVector obj){
  size_t i = 0;
  double minDist = obj.dist2D(arr[0]);
  for(int j = 1; j < arr.size(); j++){
    if(obj.dist2D(arr[j]) < minDist){
      i = j;
    }
  }
  return i;
}
size_t BasicWheelController::getNearest(VectorArr arr, PVector obj, size_t start){
  size_t i = 0;
  double minDist = obj.dist2D(arr[start]);
  if(start == arr.size() - 1){
    return start;
  }
  for(int j = start + 1; j < arr.size(); j++){
    if(obj.dist2D(arr[j]) < minDist){
      i = j;
    }
  }
  return i;
}
//The beefiest function in this file
void BasicWheelController::followPath(VectorArr arr, bool isNeg){
  
  double purePursuitDist = 16.0; // Distance to pure pursuit target
  VectorArr bezier;

  #ifndef USE_GAME_SPECIFIC
  #warning GSD (Auton position reverse)
  #endif
  //Change to new game dependency
  if(reversed){
    for(auto& a : arr){
      a *= -1.0;
    }
  }
  auto arrCopy = arr;
  
  //Simple initialization and turn to first point
  {
    //Allow the array to be drawn
    this->drawArr = true;
    arr.push_front(botPos());
    //Construct the original bezier
    bezier = bezierCurve(arr);
    //Copy the array for future use
    arrCopy = arr;
    this->path = bezier;
    //Turn to the first point purePursuitDist away
    // get first point
    double dist = botPos().dist2D(bezier[0]);
    int i = 0;
    while(dist < purePursuitDist){
      dist += bezier[i].dist2D(bezier[i + 1]);
      i++;
    }
    if(!turnPrevent){
      callingInDrive = true;
      turnTo(botPos().angleTo(bezier[i - 1]) + 180.0 * isNeg);
      callingInDrive = false;
    }
    turnPrevent = false;
    cout << "Turn Done" << endl;
    afterTurn();
    afterTurn = [](){};
  }
  //Remake bezier, get curvatures, set target speeds
  vector<double> targetSpeeds;
  {
    //Get original arr
    arr = arrCopy;
    arr.push_front(botPos());
    bezier = bezierCurve(arr);
    
    vector<double> curvatures = bezierCurvature(arr);
    targetSpeeds = vector<double>();
    
    for(int i = 0; i < bezier.size(); i++){
      targetSpeeds.push_back(abs(min(speedLimit, kConst / curvatures[i])));
      if(targetSpeeds.back() > 100){
        targetSpeeds.back() = 100;
      }
    }
    //Smooth targetSpeeds
    //vf^2 = vi^2 + 2ad
    double startVel = 30;
    targetSpeeds[0] = startVel;
    targetSpeeds.push_back(40);
    for(int i = 1; i < bezier.size(); i++){
      //I think this math of converting inches to percent works
      double d = bezier[i].dist2D(bezier[i - 1]);
      double a = maxAcc;
      if(startVel < targetSpeeds[i]){
        startVel = targetSpeeds[i] = sqrt(pow(startVel * 3.75 * M_PI / 9.0, 2) + 2.0 * a * d) * (9.0 / (3.75 * M_PI));
        if(startVel > targetSpeeds[i + 1]){
          startVel = targetSpeeds[i + 1];
        }
      }
      else if(startVel > targetSpeeds[i]){
        int startI = i;
        //Go backwards until reached speed
        do {
          startVel = targetSpeeds[i];
          i--;
          double a = maxDAcc;
          double d = bezier[i].dist2D(bezier[i + 1]);
          startVel = targetSpeeds[i] = sqrt(pow(startVel * 3.75 * M_PI / 9.0, 2) + 2.0 * a * d) * (9.0 / (3.75 * M_PI));
        } while(startVel < targetSpeeds[i - 1]);
        i = startI;
        startVel = targetSpeeds[i];
      }
    }
  }
  
  //Make extended path
  VectorArr extendedPath;
  {
    extendedPath = bezier;
    PVector ext = bezier.last() - bezier[bezier.size() - 2];
    double extraDist = 0.7;
    for(int i = 1; i < 24.0 / extraDist; i++){
      extendedPath.push(bezier.last() + ext * extraDist * i);
    }
  }

  //The index of the pursuit target
  int bezierIndex = 0;
  //The pursuit target
  PVector pursuit = bezier[bezierIndex];
  //Lots variables

  //Allow the array to be drawn
  this->drawArr = true;
  this->path = bezier;
  //The last dist
  double lastDist = 12 * 2 * 24;
  //A timer
  timer t = timer();
  int time = 0, //Counts the time in loop iterations
  timeIn = 0, // The amount of time spent near the target
  maxTimeIn = 5, // The time needed before exit
  sleepTime = 10, // The sleep time
  g = 0; // A debug output counter

  double normAngle = 0.0, //The normAngle
  //Going with an hopefully possible 1 in accuracy
  minAllowedDist = exitDist == 0.0 ? 4.0 : exitDist; // The maximum distance from target before starting timeIn count
  cout << minAllowedDist << endl;
  #undef DEBUG
  #ifdef DEBUG  
  struct {
    vector<double> outSpeeds, encSpeeds, targSpeeds, angles, cp, cd, sp, sd;
    vector<PVector> pos, pursuit;
    void add(double out, double enc, double targ, PVector p, double angle, double acp, double acd, double asp, double asd, PVector apursuit){
      outSpeeds.push_back(out);
      encSpeeds.push_back(enc);
      targSpeeds.push_back(targ);
      pos.push_back(p);
      angles.push_back(angle);
      cp.push_back(acp);
      cd.push_back(acd);
      sp.push_back(asp);
      sd.push_back(asd);
      pursuit.push_back(apursuit);
    }
  } realTime;
  #endif
  
  //Save the current distance fns
  setOldDistFns();
  int timesStopped = 0;
  moving = true;
  PVector lastPos = botPos();
  int lastIndex = 0;
  //Loop
  while(timeIn * sleepTime < maxTimeIn){
    //Get the nearest pure pursuit position
    int nearestIndex = getNearest(bezier, botPos(), lastIndex);
    lastIndex = nearestIndex;
    if(exitEarly){
      cout << "Exit due to external thread request" << endl;
      exitEarly = false;
      break;
    }
    // Keep the Pure Pursuit target purePursuitDist inches away from the bot
    while(pursuit.dist2D(botPos()) < purePursuitDist && pursuit != bezier.last()){
      pursuit = bezier[bezierIndex];
      ++bezierIndex;
    }
    
    //Near the target, increment timeIn
    if(botPos().dist2D(bezier.last()) < minAllowedDist && pursuit == bezier.last()){
      timeIn++;
      
    }
    else {
      timeIn = 0;
    }
    //The distance to the pursuit target
    double dist = botPos().dist2D(pursuit);
    
    //If the bot's not moving, and it's not currently accelerating
    if(pos.velocity() < 0.1 && t.time(timeUnits::msec) > 1000){
      timesStopped++;
    }
    else {
      timesStopped = 0;
    }
    //50 ms not moving -> exit
    if(timesStopped * sleepTime > 50 && !stopExitPrev){
      cout << "Stop Exit" << endl;
      break;
    }
    //Get the speed of the robot
    double speed = ctrl.getVal(abs(dist)) * (isNeg * 2.0 - 1.0);
    
    //Use the distFns for the current dist
    useDistFns(botPos().dist2D(bezier.last()));
    //The point extended beyond the path to make sure normAngle doesn't balloon near the target
    PVector virtualPursuit = pursuit;
    if(botPos().dist2D(pursuit) < purePursuitDist && pursuit == bezier.last()){
      //A vector that is parallel wih last point
      PVector last = bezier.last() - bezier[bezier.size() - 4];
      //Distance to be added so that virtualPursuit is still purePursuitDist away from bot
      double addDist = purePursuitDist - botPos().dist2D(pursuit);
      //Make last to be proper size

      last *= purePursuitDist / last.mag();
      virtualPursuit += last;
    }
    //Angle of robot to target
    double angle = baseAngle(botPos().angleTo(virtualPursuit));
    
    //The angle that it needs to travel at
    double normAngle = posNeg180(angle - botAngle() + 180 * isNeg);
    

    /*** NOTHING HAPPENING IN NEXT TWO BLOCKS ***/
    //So that the robot can take tight turns, 
    //if the turn is too tight, then the robot direction of travel will flip
    //Also if it's basically at the end, 
    //  then the robot will take the more efficient path backwards to the target
    //  rather than turn around
    if(abs(normAngle) >= 150){
      //isNeg = !isNeg;
      // cout << botPos() << ", " << pursuit << endl;
      // cout << normAngle << endl;
      // cout << "Reverse Neg" << endl;
      //Decrease timeIn because we aren't doing any sleeping this round
      if(timeIn > 0){
        //timeIn--;
      }
      //Send it back up top to recalibrate the speeds without sleeping
      //continue;
    }

    /**** IF THE DATE IS NOT FRIDAY, FEB 4, 2022, DON'T TOUCH THE NEXT LINE ***/
    //Get the turn speed and divide by 2 because it is being applied to both wheels
    double rightExtra = -slaveCtrl.getVal(normAngle) / 4.0 - 10.0;
    PVector currentPos = botPos();
    PVector estimateIn100 = (currentPos - lastPos) * (100.0 / (double)sleepTime) + currentPos;
    int indexIn100 = getNearest(extendedPath, estimateIn100);
    if(bezier[indexIn100].dist2D(estimateIn100) < pathRadius){
      rightExtra = 0;

    }
    //normAngle is too big for program to handle -> exit
    if(abs(normAngle) > 70){
      cout << "Ok exit" << endl;
      break;
    }
    //2 the extra turn speed as given if the distance is greater than 6 inches, 
    //Otherwise divide by 10 so that the bot isn't over turning
    double extraSpeed = (dist > 6.0 ? rightExtra : rightExtra / 2.0);
    /***   ^^MIGHT NOT BE NECESSARY IF virtualPursuit DOES IT'S JOB    ****/

    if(g++ == 2){
      cout << normAngle << ", " << extraSpeed << endl;
      // cout << extraSpeed << ", " << normAngle << endl;
      // cout << speed << ", " << dist << endl;
      g = 0;
    }
    //Slew speed
    if(abs(speed) > targetSpeeds[nearestIndex]){
      double orgSpeed = speed;
      speed = targetSpeeds[nearestIndex];
      //Change extraSpeed to match original speed : extraSpeed ratio
      extraSpeed *= orgSpeed / speed / 2.0;
    }
    if(abs(speed) > speedLimit){
      speed /= abs(speed);
      speed *= speedLimit;
    }
    //Mindblowing lines right here
    //Move the robot
    moveRight(abs(speed - extraSpeed), isNeg ? reverse : fwd);
    moveLeft(abs(speed + extraSpeed), isNeg ? reverse : fwd);
    lastPos = botPos();
    //Sleep (WOW, HE'S A GENIUS)
    s(sleepTime);
    
    #ifdef DEBUG
    realTime.add(speed, pos.velocity(), targetSpeeds[nearestIndex], botPos(), botAngle(), ctrl.p, ctrl.d, slaveCtrl.p, slaveCtrl.d, pursuit);
    #endif
  }
  moving = false;
  //Stop drawing the path
  //De-init code
  {
    //Set the last target for external stuff
    lastTarget = bezier.last();
    //Stop the bot
    switch(BrakeMode){
      case exitMode::normal:
        hardBrake();
        break;
      case exitMode::coast:
        coastBrake();
        break;
      case exitMode::nothing:
        break;
    }
    this->drawArr = false;
    cout << "Path stop" << endl;
    //Print postion and target position
    cout << botPos() << ", " << bezier.last() << endl;
    exitDist = 0.0;
  }
  stopExitPrev = false;
  //Print all the lists
  #ifdef DEBUG
    s(1000);
    cout << endl << endl;
    cout << "p.frameRate(" << 1.0 / (double)sleepTime * 1000 << ");\n";
    cout << "main.inputData([";
    int line = 0;
    for(auto i : bezier){
      cout << "p.createVector(" << i << "), " << (line++ % 3 == 0 ? "\n" : "");
      s(20);

    }
    s(100);
    cout << "]);";
    cout << "\nmain.auxiliaryData.pos = [";
    for(auto i : realTime.pos){
      cout << "p.createVector(" << i << "), " << (line++ % 3 == 0 ? "\n" : "");
      s(20);
    }
    s(100);
    cout << "];\n\nmain.auxiliaryData.angle = [";
    for(auto i : realTime.angles){
      cout << i << ", " << (line++ % 3 == 0 ? "\n" : "");
      s(10);
    }
    s(100);
    cout << "];\n\nmain.auxiliaryData.pursuit = [";
    for(auto i : realTime.pursuit){
      cout << "p.createVector(" << i << "), " << (line++ % 3 == 0 ? "\n" : "");
      s(20);
    }
    s(100);
    cout << "];\n";
    cout << "main.resetI();main.setHighlight(0);\n";
    cout << "outputVel.inputData([" << flush;
    s(100);
    for(auto i : realTime.outSpeeds){
      cout << i << ", " << (line++ % 3 == 0 ? "\n" : "");
      s(20);
    }
    s(100);
    cout << "]);\n";
    cout << "encVel.inputData([";
    for(auto i : realTime.encSpeeds){
      cout << i << ", " << (line++ % 3 == 0 ? "\n" : "");
      s(20);
    }
    s(100);
    cout << "]);\n";
    cout << "targetVel.inputData([";
    for(auto i : realTime.targSpeeds){
      cout << i << ", " << (line++ % 3 == 0 ? "\n" : "");
      s(10);
    }
    s(100);
    cout << "]);\n";
    cout << "slaveP.inputData([";
    for(auto i : realTime.sp){
      cout << i << ", " << (line++ % 3 == 0 ? "\n" : "");
      s(10);
    }
    s(100);
    cout << "]);\n";
    cout << "slaveD.inputData([";
    for(auto i : realTime.sd){
      cout << i << ", " << (line++ % 3 == 0 ? "\n" : "");
      s(10);
    }
    s(100);
    cout << "]);\n";
    cout << "ctrlP.inputData([";
    for(auto i : realTime.cp){
      cout << i << ", " << (line++ % 3 == 0 ? "\n" : "");
      s(10);
    }
    s(100);
    cout << "]);\n";
    cout << "ctrlD.inputData([";
    for(auto i : realTime.cd){
      cout << i << ", " << (line++ % 3 == 0 ? "\n" : "");
      s(10);
    }
    s(100);
    cout << "]);\n";
    cout << "main.xRange = [48, -48]; main.yRange = [60, -60]; \nctrlD.customizeRange();\nctrlP.customizeRange();\nslaveP.customizeRange();\nslaveD.customizeRange();\ntargetVel.customizeRange();\nencVel.customizeRange();\noutputVel.customizeRange();";
    cout << endl;
  #endif
  

}
void BasicWheelController::followPath(VectorArr arr){

  followPath(arr, false);

}
void BasicWheelController::backwardsFollow(VectorArr arr){

  followPath(arr, true);

}
void BasicWheelController::backInto(PVector pos){
  
  backwardsFollow({ pos });

}
void BasicWheelController::basicPIDDrive(double targetDist, bool isNeg){
  double purePursuitDist = 16.0; // Distance to pure pursuit target
  
  PVector startPos = botPos();
  double startAngle = botAngle();
  //The last dist
  double lastDist = 12 * 2 * 24;
  //A timer
  timer t = timer();
  int time = 0, //Counts the time in loop iterations
  timeIn = 0, // The amount of time spent near the target
  maxTimeIn = 5, // The time needed before exit
  sleepTime = 10, // The sleep time
  g = 0; // A debug output counter

  double normAngle = 0.0, //The normAngle
  //Going with an hopefully possible 1 in accuracy
  minAllowedDist = exitDist == 0.0 ? 1.0 : exitDist; // The maximum distance from target before starting timeIn count
  cout << minAllowedDist << endl;
  
  
  //Save the current distance fns
  setOldDistFns();
  int timesStopped = 0;
  moving = true;
  //Loop
  while(timeIn * sleepTime < maxTimeIn){
    if(exitEarly){
      cout << "Exit due to external thread request" << endl;
      exitEarly = false;
      break;
    }
    //Near the target, increment timeIn
    if(botPos().dist2D(startPos) < minAllowedDist){
      timeIn++;
      
    }
    else {
      timeIn = 0;
    }
    //The distance to the pursuit target
    double dist = targetDist - botPos().dist2D(startPos);

    //If the bot's not moving, and it's not currently accelerating
    if(pos.velocity() < 0.1 && t.time(timeUnits::msec) > 1000){
      timesStopped++;
    }
    else {
      timesStopped = 0;
    }
    //50 ms not moving -> exit
    if(timesStopped * sleepTime > 50 && !stopExitPrev){
      cout << "Stop Exit" << endl;
      break;
    }
    //Get the speed of the robot
    double speed = ctrl.getVal(abs(dist) > 16.0 ? 16.0 : abs(dist)) * (isNeg * 2.0 - 1.0);
    
    //Use the distFns for the current dist
    useDistFns(dist);
    
    //Angle of robot to target
    double angle = startAngle;
    
    //The angle that it needs to travel at
    double normAngle = posNeg180(angle - botAngle() + 180 * isNeg);

    
    /**** IF THE DATE IS NOT FRIDAY, FEB 4, 2022, DON'T TOUCH THE NEXT LINE ***/
    //Get the turn speed and divide by 2 because it is being applied to both wheels
    double rightExtra = -slaveCtrl.getVal(normAngle) / 4.0 - 10.0;
    
    //normAngle is too big for program to handle -> exit
    if(abs(normAngle) > 70){
      cout << "Ok exit" << endl;
      break;
    }
    //Use the extra turn speed as given if the distance is greater than 6 inches, 
    //Otherwise divide by 10 so that the bot isn't over turning
    double extraSpeed = (dist > 6.0 ? rightExtra : rightExtra / 2.0);
    /***   ^^MIGHT NOT BE NECESSARY IF virtualPursuit DOES IT'S JOB    ****/

    if(g++ == 2){
      cout << normAngle << ", " << extraSpeed << endl;
      g = 0;
    }
    
    if(abs(speed) > speedLimit){
      speed /= abs(speed);
      speed *= speedLimit;
    }
    //Mindblowing lines right here
    //Move the robot
    moveRight(abs(speed - extraSpeed), isNeg ? reverse : fwd);
    moveLeft(abs(speed + extraSpeed), isNeg ? reverse : fwd);
    //Sleep (WOW, HE'S A GENIUS)
    s(sleepTime);
    
  }
  moving = false;
  //Stop drawing the path
  //De-init code
  {
    //Stop the bot
    switch(BrakeMode){
      case exitMode::normal:
        hardBrake();
        break;
      case exitMode::coast:
        coastBrake();
        break;
      case exitMode::nothing:
        break;
    }
    cout << "Path stop" << endl;
    //Print postion and target position
    cout << targetDist << ", " << botPos().dist2D(startPos) << endl;
    exitDist = 0.0;
  }
  stopExitPrev = false;
  
}
void BasicWheelController::driveFwd(double dist){
  basicPIDDrive(dist, false);
}
void BasicWheelController::driveBack(double dist){
  basicPIDDrive(dist, true);
}

void BasicWheelController::moveLeft(int speed, directionType d){
  Left.spinVolt(d, speed);
}
void BasicWheelController::moveRight(int speed, directionType d){
  Right.spinVolt(d, speed);
}
void BasicWheelController::turnLeft(int speed){
  Right.spinVolt(fwd, speed);
  Left.spinVolt(reverse, speed);
}
void BasicWheelController::turnRight(int speed){
  Left.spinVolt(fwd, speed);
  Right.spinVolt(reverse, speed);
}
void BasicWheelController::coastBrake(){
  Left.stop(coast);
  Right.stop(coast);
}



void MechWheelController::moveAt(double angle, double speed, double turnSpeed){
  double Y1 = cos(angle * DEG_TO_RAD) * speed;
  double Y2 = cos(angle * DEG_TO_RAD) * speed;
  double X1 = sin(angle * DEG_TO_RAD) * speed;
  double X2 = sin(angle * DEG_TO_RAD) * speed;
  double FLS = Y1 + X1;
  double BLS = Y1 - X1;
  double FRS = Y2 - X2;
  double BRS = Y2 + X2;
  FLS += turnSpeed;
  BLS += turnSpeed;
  FRS -= turnSpeed;
  BRS -= turnSpeed;
  //Spin da motors
  BL->spin(vex::forward, BLS, velocityUnits::pct);
  BR->spin(vex::forward, BRS, velocityUnits::pct);
  FL->spin(vex::forward, FLS, velocityUnits::pct);
  FR->spin(vex::forward, FRS, velocityUnits::pct);
}
void MechWheelController::followPath(VectorArr arr, double targetAngle){
  //Add the bot position to the front so that it has a smoother start
  arr.push_front(botPos());
  VectorArr bezier = bezierCurve(arr);
  this->drawArr = true;
  this->path = bezier;
  PVector pursuit = bezier.first();
  int i = 0;
  double lastDist = 12 * 2 * 24;
  timer t = timer();
  int time = 0;
  int speedLimit = 70;
  double angle = botAngle();
  int bezierIndex = 0;
  double normAngle = posNeg180(angle - posNeg180(botAngle()));
  double sign = 1.0;
  double startSign = normAngle > 0.0 ? 1.0 : -1.0;
  setOldDistFns();
  double lastTurnAngle = 0;
  int g = 0;
  while(pursuit != bezier.last() || botPos().dist2D(pursuit) > 2.0){
    //Maintain distance of 8 inches to Pure Pursuit target
    while(pursuit.dist2D(botPos()) < 8.0 && i < bezier.size()){
      
      pursuit = bezier[bezierIndex];
      
      ++bezierIndex;
    }
    double dist = botPos().dist2D(pursuit);
    if(abs(dist - lastDist) <= 0.4 && t.time(timeUnits::msec) > 700 && time++ * 10 > 700){
      cout << "Stop Exit" << endl;
      break;
    }
    double speed = -ctrl.getVal(dist);// * (bezier.size() - i) * 0.1;
    useDistFns(botPos().dist2D(bezier.last()));
    //Impose speed limit
    if(speed > speedLimit){
      speed = speedLimit;
    }
    //Angle between bot position and pursuit target
    angle = baseAngle(botPos().angleTo(pursuit));
    //The angle that it needs to travel at
    double normAngle = posNeg180(angle - botAngle());

    //The angle that it needs to turn to
    double normAngle2 = posNeg180(targetAngle - botAngle());

    double newSign = normAngle2 > 0.0 ? 1.0 : -1.0;
    // if(newSign / startSign == -1.0){
    //   slaveCtrl.setTarget(0.0);
    //   startSign = newSign;
    // }
    double turnSpeed = -turnCtrl.getVal(normAngle2);
    if(lastTurnAngle != 0 && abs(normAngle2) - 0.01 > abs(lastTurnAngle)){
      //turnSpeed *= 2.0;
    }
    
    lastTurnAngle = normAngle2;
    moveAt(normAngle, speed, turnSpeed / 4.0);
    s(1);

  }
  hardBrake();
  s(1500);
  this->drawArr = false;
  cout << botPos() << ", " << bezier.last() << endl;
  cout << botAngle() << endl;

}
void MechWheelController::driveTo(PVector pos, double finalAngle){
  followPath({ pos }, finalAngle);
}
void MechWheelController::driveTo(double x, double y, double finalAngle){
  driveTo(PVector(x, y), finalAngle);
}
void MechWheelController::backInto(double x, double y, double finalAngle){
  backInto(PVector(x, y), finalAngle);
}
void MechWheelController::backInto(PVector pos, double finalAngle){
  backwardsFollow({ pos }, finalAngle);
}
void MechWheelController::backwardsFollow(VectorArr arr, double targetAngle){
  arr.push_front(botPos());
  VectorArr bezier = bezierCurve(arr);
  this->drawArr = true;
  this->path = bezier;
  PVector pursuit = bezier.first();
  int i = 0;
  double lastDist = 12 * 2 * 24;
  timer t = timer();
  int time = 0;
  int speedLimit = 70;
  double angle = botAngle();
  double normAngle = posNeg180(angle - posNeg180(botAngle()));
  double sign = 1.0;
  double startSign = normAngle > 0.0 ? 1.0 : -1.0;
  setOldDistFns();
  double lastTurnAngle = 0;
  int g = 0;
  int bezierIndex = 0;
  while(pursuit != bezier.last() || botPos().dist2D(pursuit) > 2.0){
    //Maintain distance of 8 inches to Pure Pursuit target
    while(pursuit.dist2D(botPos()) < 8.0 && i < bezier.size()){
      
      pursuit = bezier[bezierIndex];
      
      ++bezierIndex;
    }
    double dist = botPos().dist2D(pursuit);
    if(abs(dist - lastDist) <= 0.4 && t.time(timeUnits::msec) > 700 && time++ * 10 > 700){
      cout << "Stop Exit" << endl;
      break;
    }
    double speed = -ctrl.getVal(dist);// * (bezier.size() - i) * 0.1;
    useDistFns(botPos().dist2D(bezier.last()));
    //Impose speed limit
    if(speed > speedLimit){
      speed = speedLimit;
    }
    //Angle between bot position and pursuit target
    angle = baseAngle(botPos().angleTo(pursuit));
    //The angle that it needs to travel at
    double normAngle = posNeg180(angle - botAngle());

    //The angle that it needs to turn to
    double normAngle2 = posNeg180(targetAngle - botAngle());

    double newSign = normAngle2 > 0.0 ? 1.0 : -1.0;
    // if(newSign / startSign == -1.0){
    //   slaveCtrl.setTarget(0.0);
    //   startSign = newSign;
    // }
    double turnSpeed = -turnCtrl.getVal(normAngle2);
    if(lastTurnAngle != 0 && abs(normAngle2) - 0.01 > abs(lastTurnAngle)){
      //turnSpeed *= 2.0;
    }
    
    lastTurnAngle = normAngle2;
    moveAt(normAngle, -speed, turnSpeed / 4.0);
    s(1);

  }
  hardBrake();
  s(1500);
  this->drawArr = false;
  cout << botPos() << ", " << bezier.last() << endl;
  cout << botAngle() << endl;


  
}