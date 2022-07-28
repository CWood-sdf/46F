#define NO_MAKE
#include "Odometry/EPA_Wheel_Control.h"
double sign(double v){
  if(v == 0.0){
    return 0;
  }
  return v / abs(v);
}

// void BasicWheelController::popTopTurnPid(){
//   auto& a = customPidsTurn.getEnd();
//   turnCtrl -= a;
//   customPidsTurn.popEnd();
// }
// void BasicWheelController::addTurnPid(PidAdder a){
//   turnCtrl += a;
//   customPidsTurn.push(a);
  
// }
// void BasicWheelController::addTurnPid(double p, double i, double d){
//   addTurnPid(PidAdder(p, i, d));
// }
double BasicWheelController::botAngle (){
  return chassis->botAngle();
}
PVector& BasicWheelController::botPos(){
  return chassis->botPos();
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
  [[maybe_unused]]
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
      chassis->turnLeft(s > speedLimit ? speedLimit : s);
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
    chassis->hardBrake();
  } else if(normAngle > degRange){
    while(timeIn * sleepTime < minTimeIn){
      
      double s = -turnCtrl.getVal(normAngle);
      chassis->turnRight(s > speedLimit ? speedLimit : s);
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
    chassis->hardBrake();
  }
  chassis->hardBrake();
  s(300);

  cout << botAngle() << endl;
  cout << angleCalc() << endl;

  
}
void BasicWheelController::turnTo(double angle){
  turnTo([=](){ return angle; });
}

bool BasicWheelController::isMoving(){
  return moving;
}
BasicWheelController::chain_method BasicWheelController::estimateStartPos(PVector v, double a){
  cout << sign(botPos().x) << ", " << sign(v.x) << endl;
  if(botPos().mag() < 3){
    chassis->pos.setPos(v, a);
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

BasicWheelController::chain_method BasicWheelController::forceEarlyExit(){
  exitEarly = true;
  CHAIN;
}

BasicWheelController::chain_method BasicWheelController::setExitMode(exitMode m){
  BrakeMode = m;
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
template<class Arr>
size_t BasicWheelController::getNearest(Arr arr, PVector obj){
  size_t i = 0;
  double minDist = obj.dist2D(arr[0]);
  for(int j = 1; j < arr.size(); j++){
    if(obj.dist2D(arr[j]) < minDist){
      i = j;
    }
  }
  return i;
}
template<class Arr>
size_t BasicWheelController::getNearest(Arr arr, PVector obj, size_t start){
  size_t i = start;
  double minDist = obj.dist2D(arr[start]);
  if(start == arr.size() - 1){
    return start;
  }
  for(int j = start; j < arr.size(); j++){
    if(obj.dist2D(arr[j]) < minDist){
      i = j;
    }
  }
  return i;
}

void BasicWheelController::ramseteFollow(VectorArr arr, bool isNeg){
  generalFollow(arr, defaultRamsete, isNeg);
}


// }
/***
Random thoughts 

the general follow function should manage the target point
it should find the closest path point
all that info needs to be sent to the controller, maybe a struct in controller
probably have some option to call turnTo at the start
follow should manage the exiting
controller should prolly have some deInit function
follow should call spin the wheels
all controller should do is just to take target and nearest point data and return wheel speed
chassis should manage wheel spinning nitty gritty
**/
void BasicWheelController::generalFollow(VectorArr arr, Controller* controller, bool isNeg){
  double purePursuitDist = 16.0; // Distance to pure pursuit target
  
  controller->init();
  #ifndef USE_GAME_SPECIFIC
  #warning GSD (Auton position reverse)
  #endif
  //Change to new game dependency
  if(reversed){
    for(auto& a : arr){
      a *= -1.0;
    }
  }
  
  if(controller->isTurnAtStart())
  //Simple initialization and turn to first point
  {
    //Allow the array to be drawn
    this->drawArr = true;
    arr.push_front(botPos());
    //Construct the original bezier
    VectorArr bezier = bezierCurve(arr);
    this->publicPath = bezier;
    //Turn to the first point purePursuitDist away
    // get first point
    double dist = botPos().dist2D(bezier[0]);
    int i = 0;
    while(dist < purePursuitDist){
      dist += bezier[i].dist2D(bezier[i + 1]);
      i++;
    }
    if(!controller->isTurnAtStart()){
      callingInDrive = true;
      turnTo(botPos().angleTo(bezier[i - 1]) + 180.0 * isNeg);
      callingInDrive = false;
    }
    cout << "Turn Done" << endl;
    afterTurn();
    afterTurn = [](){};
  }

  path.make(arr, chassis);
  //Make extended path
  auto bezier = path.getBezier();
  VectorArr extendedPath;
  {
    extendedPath = bezier;
    PVector ext = (PVector)bezier.last() - bezier[bezier.size() - 2];
    double extraDist = 0.7;
    for(int i = 1; i < 24.0 / extraDist; i++){
      extendedPath.push((PVector)bezier.last() + ext * extraDist * i);
    }
  }

  //The index of the pursuit target
  int bezierIndex = 0;
  //The pursuit target
  PVector pursuit = path[bezierIndex];
  //Lots variables

  //Allow the array to be drawn
  this->drawArr = true;
  this->publicPath = bezier;
  //The last dist
  //double lastDist = 12 * 2 * 24;
  //A timer
  timer t = timer();
  [[maybe_unused]]
  int time = 0, //Counts the time in loop iterations
  timeIn = 0, // The amount of time spent near the target
  maxTimeIn = 5, // The time needed before exit
  sleepTime = 10, // The sleep time
  g = 0; // A debug output counter

  double 
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
    int nearestIndex = getNearest(path, botPos(), lastIndex);
    lastIndex = nearestIndex;
    if(exitEarly){
      cout << "Exit due to external thread request" << endl;
      exitEarly = false;
      break;
    }
    // Keep the Pure Pursuit target purePursuitDist inches away from the bot
    while(pursuit.dist2D(botPos()) < purePursuitDist && pursuit != path.last()){
      pursuit = path[bezierIndex];
      ++bezierIndex;
    }
    if (bezierIndex >= path.size()) {
        bezierIndex = path.size() - 1;
    }
    //Near the target, increment timeIn
    if(botPos().dist2D(path.last()) < minAllowedDist && pursuit == path.last()){
      timeIn++;
      
    }
    else {
      timeIn = 0;
    }
    //The distance to the pursuit target
    double dist = botPos().dist2D(pursuit);
    
    //If the bot's not moving, and it's not currently accelerating
    if(chassis->pos.velocity() < 0.1 && t.time(timeUnits::msec) > 1000){
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
    //Use the distFns for the current dist
    useDistFns(botPos().dist2D(path.last()));
    //The point extended beyond the path to make sure normAngle doesn't get big near the target
    PVector virtualPursuit = pursuit;
    if(botPos().dist2D(pursuit) < purePursuitDist && pursuit == path.last()){
      //A vector that is parallel wih last point
      PVector last = (PVector)path.last() - path[path.size() - 4];
      //Distance to be added so that virtualPursuit is still purePursuitDist away from bot
      double addDist = purePursuitDist - botPos().dist2D(pursuit);
      //Make last to be proper size

      last *= addDist / last.mag();
      virtualPursuit += last;
    }
    dist = botPos().dist2D(virtualPursuit);
    //Angle of robot to target
    double angle = baseAngle(botPos().angleTo(virtualPursuit));
    
    //The angle that it needs to travel at
    double normAngle = posNeg180(angle - botAngle() + 180 * isNeg);
    
    Controller::Input input;
    input.angleTarget = angle;
    input.currentAngle = posNeg180(botAngle() + 180 * isNeg);
    input.target = virtualPursuit;
    input.position = botPos();
    input.dist = dist;
    auto copy = path[bezierIndex];
    copy.bezierPt = virtualPursuit;
    input.targetPt = copy;
    input.chassis = chassis;
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

    auto speeds = controller->followTo(input);
    double speed = 0;
    switch(speeds.first.second){
    case Controller::FwdVelTps::inps:
      speed = chassis->realToPct(speeds.first.first);
      break;
    case Controller::FwdVelTps::pct:
      speed = speeds.first.first;
      break;
    }
    //speed *= -(isNeg * 2.0 - 1);

    if(g++ == 2){
      //cout << normAngle << ", " << extraSpeed << endl;
      // cout << extraSpeed << ", " << normAngle << endl;
      // cout << speed << ", " << dist << endl;
      g = 0;
    }
    // double targetVel = abs(speed);
    
    double orgSpeed = speed + 0.000000001;
    //Slew speed
    if(abs(speed) > path[nearestIndex].targetSpeed){
      orgSpeed = speed;
      speed = path[nearestIndex].targetSpeed;
    }
    if(abs(speed) > chassis->speedLimit){
      orgSpeed = speed;
      speed /= abs(speed);
      speed *= chassis->speedLimit;
    }
    double rightExtra;
    {
      double targetRobotVel = chassis->pctToReal(speed);
      switch (speeds.second.second) {
      case Controller::AngularVelTps::curvature:
        rightExtra = chassis->realToPct(speeds.second.first * (chassis->trackWidth + 3.0) * targetRobotVel / 2.0);
        break;
      case Controller::AngularVelTps::pctDiff:
        rightExtra = speeds.second.first;
        break;
      case Controller::AngularVelTps::radps:
        rightExtra = speeds.second.first * chassis->trackWidth / -2.0;
        break;
      }
    }
    //This is half done, pls fix
    // if ((abs(speed + rightExtra) > 100.0 || abs(speed - rightExtra) > 100.0) && abs(rightExtra) < 100) {
    //   double newFwdVel = chassis->pctToReal(speed);
    //   double left, right;
    //   switch (speeds.second.second) {
    //   case Controller::AngularVelTps::curvature: [[likely]]
    //   TOP:
    //     left = chassis->realToPct(newFwdVel * (2.0 + speeds.second.first * chassis->trackWidth) / 2.0);
    //     right = chassis->realToPct(newFwdVel * (2.0 - speeds.second.first * chassis->trackWidth) / 2.0);
    //     if (abs(left) > 100) {
    //       newFwdVel = newFwdVel * 99.0 / left;
    //       goto TOP;
    //     }
    //     if (abs(right) > 100) {
    //       newFwdVel = newFwdVel * 99.0 / right;
    //       goto TOP;
    //     }
    //     speed = chassis->realToPct(newFwdVel);
    //     break;
    //   case Controller::AngularVelTps::pctDiff:
    //   TOP2:
    //     left = speed + speeds.second.first;
    //     right = speed - speeds.second.first;
    //     if (abs(left) > 100) {
    //       speeds.second.first = speeds.second.first * 99.0 / left;
    //       goto TOP2;
    //     }
    //     if (abs(right) > 100) {
    //       speeds.second.first = speeds.second.first * 99.0 / right;
    //       goto TOP2;
    //     }
    //     break;
    //   case Controller::AngularVelTps::radps:
    //   TOP3:
    //     rightExtra = chassis->pctToReal(speed) * chassis->trackWidth / -2.0;
    //     if (abs(speed + rightExtra) > 100 || abs(speed - rightExtra) > 100) {
    //       if (abs(speed + rightExtra) > 100) {
    //         speed = speed * 99.0 / (speed + rightExtra);
    //       }
    //       if (abs(speed - rightExtra) > 100) {
    //         speed = speed * 99.0 / (speed - rightExtra);
    //       }
    //       goto TOP3;
    //     } 
    //     break;
    //   }
    // }
    if(isNeg){
      // rightExtra *= -1;
      speed *= -1.0;
    }
    //Mindblowing lines right here
    //Move the robot
    chassis->driveFromDiff(-speed, -rightExtra, fwd);
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
        chassis->hardBrake();
        break;
      case exitMode::coast:
        chassis->coastBrake();
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
  controller->deInit();
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
//The beefiest function in this file
void BasicWheelController::purePursuitFollow(VectorArr arr, bool isNeg){
  generalFollow(arr, defaultPurePursuit, isNeg);
  

}

void MechWheelController::moveAt(double angle, double speed, double turnSpeed){
  //There are actual equations governing this
  // double Y1 = cos(angle * DEG_TO_RAD) * speed;
  // double Y2 = cos(angle * DEG_TO_RAD) * speed;
  // double X1 = sin(angle * DEG_TO_RAD) * speed;
  // double X2 = sin(angle * DEG_TO_RAD) * speed;
  // double FLS = Y1 + X1;
  // double BLS = Y1 - X1;
  // double FRS = Y2 - X2;
  // double BRS = Y2 + X2;
  // FLS += turnSpeed;
  // BLS += turnSpeed;
  // FRS -= turnSpeed;
  // BRS -= turnSpeed;
  // //Spin da motors
  // BL->spin(vex::forward, BLS, velocityUnits::pct);
  // BR->spin(vex::forward, BRS, velocityUnits::pct);
  // FL->spin(vex::forward, FLS, velocityUnits::pct);
  // FR->spin(vex::forward, FRS, velocityUnits::pct);
}
void MechWheelController::followPath(VectorArr arr, double targetAngle){
  

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
  


  
}