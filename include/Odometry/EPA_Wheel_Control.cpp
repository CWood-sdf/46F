#define NO_MAKE
#include "Controllers.h"
double sign(double v)
{
  if (v == 0.0)
  {
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
double WheelController::botAngle()
{
  return chassis->botAngle();
}
PVector &WheelController::botPos()
{
  return chassis->botPos();
}
// Add a function to be called at a specified distance
void WheelController::addDistFn(double dist, std::function<void()> fn)
{
  distFns[dist] = fn;
}
// Reuse the old map
void WheelController::reuseDistFns()
{
  distFns = oldFns;
}
void WheelController::setFn(std::function<void()> fn)
{
  afterTurn = fn;
  hasFn = true;
}
void WheelController::callFn()
{
  if (hasFn)
  {
    afterTurn();
  }
  hasFn = false;
}
void WheelController::reuseFn()
{
  hasFn = true;
}

// A hard brake

void WheelController::setOldDistFns()
{
  oldFns = distFns;
}
void WheelController::useDistFns(double dist)
{
  for (auto it = distFns.begin(); it != distFns.end() /* not hoisted */; /* no increment */)
  {
    if ((*it).first < dist)
    {

      (*it).second();
      distFns.erase(it++); // or "it = m.erase(it)" since C++11
    }
    else
    {
      ++it;
    }
  }
}
void WheelController::turnTo(std::function<double()> angleCalc)
{
  //
  auto oldAngleCalc = angleCalc;
  if (!isRed() && !callingInDrive)
  {
#ifndef USE_GAME_SPECIFIC
#warning GSD (Turning add)
#endif
    angleCalc = [&]()
    {
      return posNeg180(oldAngleCalc() + 180);
    };
  }
  // If the auton is on the other side, turn to the opposite angle
  double angle = angleCalc();
  cout << "A: " << angle << endl;
  cout << "A2: " << botAngle() << endl;
  // if(!callingInDrive && reversed){
  //   angle += 90;
  // }
  int timeIn = 0;
  [[maybe_unused]] int i = 0;
  // Get the normAngle
  double normAngle = posNeg180(angle - posNeg180(botAngle()));
  // init PID
  turnCtrl.setTarget(0);
  int sleepTime = 20;
  int minTimeIn = 200;
  double degRange = 4.0;
  int speedLimit = 60;

  //
  //
  //
  // cout << normAngle << endl;

  // It is off by 4 degrees, because:
  //     A: if it is within 4 degrees, without the minSpeed
  //           it is too slow; but, with minSpeed, it overshoots too much
  while (timeIn * sleepTime < minTimeIn)
  {

    double speed = turnCtrl.getVal(normAngle);

    chassis->turnLeft(speed > speedLimit ? speedLimit : speed);
    s(sleepTime);

    angle = angleCalc();

    normAngle = posNeg180(angle - botAngle());
    if (abs(normAngle) < degRange)
    {
      timeIn++;
    }
    else
    {
      timeIn = 0;
    }
    //
  }
  chassis->hardBrake();

  // s(300);

  cout << botAngle() << endl;
  cout << angleCalc() << endl;
}
void WheelController::turnTo(double angle)
{
  turnTo([=]()
         { return angle; });
}
// Implement faceTarget
void WheelController::faceTarget(PVector target)
{
  if (!isRed())
  {
    target.x *= -1.0;
    target.y *= -1.0;
  }
  callingInDrive = true;
  turnTo(botPos().angleTo(target));
  callingInDrive = false;
}

bool WheelController::isMoving()
{
  return moving;
}
#ifndef WINDOWS
WheelController::chain_method WheelController::estimateStartPos(PVector v, double a)
{
  cout << sign(botPos().x) << ", " << sign(v.x) << endl;
  // If it thinks it is at (0, 0), just use the given value
  if (botPos().mag() < 6)
  {
    chassis->pos.setPos(v, a);
  }
#ifndef USE_GAME_SPECIFIC
#warning GSD (Conditions for reversing auton)
#endif
  // // If they are on opposite sides, reverse the auton
  // else if (sign(botPos().x) != sign(v.x) && sign(botPos().y) != sign(v.y))
  // {
  //   reversed = true;
  //   cout << "Reversing auton" << endl;
  // }
  // else
  // {
  //   reversed = false;
  // }
  CHAIN;
}
#endif
WheelController::chain_method WheelController::forceEarlyExit()
{
  exitEarly = true;
  CHAIN;
}

WheelController::chain_method WheelController::setExitMode(exitMode m)
{
  BrakeMode = m;
  CHAIN;
}

WheelController::chain_method WheelController::setExitDist(double v)
{
  exitDist = v;
  CHAIN;
}
PVector WheelController::getLastTarget()
{
  return lastTarget;
}
WheelController::chain_method WheelController::prevStopExit()
{
  stopExitPrev = true;
  CHAIN
}
WheelController::chain_method WheelController::setPathRadius(double r)
{
  pathRadius = r;
  CHAIN
}
double WheelController::getPathRadius()
{
  return pathRadius;
}
WheelController::chain_method WheelController::setFollowPathDist(double d)
{
  followPathDist = d;
  CHAIN;
}
double WheelController::getFollowPathDist()
{
  return followPathDist;
}
WheelController::chain_method WheelController::setFollowPathMaxTimeIn(int t)
{
  followPathMaxTimeIn = t;
  CHAIN;
}
int WheelController::getFollowPathMaxTimeIn()
{
  return followPathMaxTimeIn;
}
template <class Arr>
size_t WheelController::getNearest(Arr arr, PVector obj)
{
  size_t i = 0;
  double minDist = obj.dist2D(arr[0]);
  for (int j = 1; j < arr.size(); j++)
  {
    if (obj.dist2D(arr[j]) < minDist)
    {
      i = j;
    }
  }
  return i;
}
template <class Arr>
size_t WheelController::getNearest(Arr arr, PVector obj, size_t start)
{
  size_t i = start;
  double minDist = obj.dist2D(arr[start]);
  if (start == arr.size() - 1)
  {
    return start;
  }
  for (int j = start; j < arr.size(); j++)
  {
    if (obj.dist2D(arr[j]) < minDist)
    {
      i = j;
    }
  }
  return i;
}

void WheelController::ramseteFollow(VectorArr arr, bool isNeg)
{
  generalFollow(arr, defaultRamsete, isNeg);
}

// }

void WheelController::driveTo(double x, double y)
{
  generalFollow({PVector(x, y)}, defaultPid, false);
}
void WheelController::backInto(double x, double y)
{
  generalFollow({PVector(x, y)}, defaultPid, true);
}

/**
 * Boolean Settings:
 *  - Turn at the start
 *  - Use forward distance
 * Other settings:
 *  - virtual pursuit distance
 *  - exit distance
 *  - exit mode
 *  - path radius
 *  - follow path distance
 *  - max time in
 */
void WheelController::generalFollowTurnAtStart(VectorArr &arr, double &purePursuitDist, bool &isNeg)
{
  this->drawArr = true;
  auto arrCopy = arr;
  arrCopy.push_front(botPos());
  // Construct the original bezier
  VectorArr bezier = bezierCurve(arrCopy);

  // cout << "sdfa" << endl; s(100);
  this->publicPath = bezier;
  // Turn to the first point purePursuitDist away
  //  get first point
  double dist = botPos().dist2D(bezier[0]);
  int i = 0;
  // cout << "Cool" << endl;
  while (dist < purePursuitDist)
  {
    dist += bezier[i].dist2D(bezier[i + 1]);
    i++;
  }
  callingInDrive = true;
  auto angle = botPos().angleTo(bezier[i - 1]);
  turnTo(angle + 180.0 * isNeg);
  callingInDrive = false;
  // cout << "Cool" << endl;
  cout << "Turn Done" << endl;
  afterTurn();
  afterTurn = []() {};
}
PVector WheelController::generalFollowGetVirtualPursuit(PVector &pursuit, Controller *controller)
{
  PVector virtualPursuit = pursuit;
  if (!(botPos().dist2D(pursuit) < controller->settings.virtualPursuitDist && pursuit == path.last()))
  {
    return virtualPursuit;
  }
  // A vector that is parallel wih last point
  PVector last = (PVector)path.last() - path[path.size() - 4];
  // Distance to be added so that virtualPursuit is still purePursuitDist away from bot
  double addDist = controller->settings.virtualPursuitDist - botPos().dist2D(pursuit);
  // Make last to be proper size

  last *= addDist / last.mag();
  virtualPursuit += last;
  return virtualPursuit;
}
double WheelController::generalFollowGetDist(int &bezierIndex, Controller *controller, PVector &pursuit)
{
  double dist = 0.0;
  if (controller->settings.useDistToGoal)
  {
    dist = botPos().dist2D(pursuit);
    // Make the dist signed so that PID will work
    // If we're not at the end, just return the distance, it won't be signed bc the point is always ahead
    if (bezierIndex != path.size() - 1)
    {
      return dist;
    }
    // Get a point near the end
    PVector pathNearEnd = path[path.size() - 3];
    // Multiply dist by -1 if the bot is past the target
    // Something abt dot products is that they are negative when the angle between is greater than 90
    double distSign = sign((pathNearEnd - path.last()).dot(botPos() - path.last()));
    dist *= distSign;
    return dist;
  }
  // If we're not near the end, let's not do fancy math
  if (bezierIndex != path.size() - 1)
  {
    return botPos().dist2D(path[bezierIndex]);
  }
  // A vector that is parallel with the end
  PVector pathDir = (PVector)path[bezierIndex] - path[bezierIndex - 1];
  // The bot's point centered with the 2nd to last point
  PVector botDir = botPos() - path[bezierIndex - 1];
  // The angle to make the parallel vector straight
  double pathDirAngle = pathDir.heading2D();
  // Rotate the vectors back by the pathDirAngle
  pathDir.rotateXY(-pathDirAngle);
  botDir.rotateXY(-pathDirAngle);
  // The dist is the difference in the y's
  dist = pathDir.y - botDir.y;
  return dist;
}
void WheelController::generalFollow(VectorArr &arr, Controller *controller, bool isNeg)
{
  double purePursuitDist = controller->settings.followPathDist; // Distance to pure pursuit target

  controller->init();
#ifndef USE_GAME_SPECIFIC
#warning GSD (Auton position reverse)
#endif
  // Change to new game dependency
  if (reversed)
  {
    for (auto &a : arr)
    {
      a *= -1.0;
    }
  }

  // cout << "sdfa" << endl; s(100);
  if (controller->settings.turnAtStart)
  // Simple initialization and turn to first point
  {
    generalFollowTurnAtStart(arr, purePursuitDist, isNeg);
  }

  // cout << "sdddfa" << endl; s(100);
  // bool sg = false;
  path.make(arr, chassis);
  // s(1000);
  // cout << "yeet" << endl; s(1000);
  // //Make extended path
  auto bezier = path.getBezier();
  // VectorArr extendedPath;
  // {
  //   extendedPath = bezier;
  //   PVector ext = (PVector)bezier.last() - bezier[bezier.size() - 2];
  //   double extraDist = 0.7;
  //   for(int i = 1; i < 24.0 / extraDist; i++){
  //     extendedPath.push((PVector)bezier.last() + ext * extraDist * i);
  //   }
  // }

  // The index of the pursuit target
  int bezierIndex = 0;
  // The pursuit target
  PVector pursuit = path[bezierIndex];
  // Lots variables

  // Allow the array to be drawn
  this->drawArr = true;
  this->publicPath = bezier;
// The last dist
// double lastDist = 12 * 2 * 24;
// A timer
#ifndef TEST

  timer t = timer();

#else

  // cout << "Sdff" << endl; s(100);
  Timer t = Timer();

// cout << "Sdff" << endl; s(100);
#endif
  [[maybe_unused]] int time = 0,                  // Counts the time in loop iterations
      timeIn = 0,                                 // The amount of time spent near the target
      maxTimeIn = controller->settings.maxTimeIn, // The time needed before exit
      sleepTime = 10,                             // The sleep time
      g = 0;                                      // A debug output counter

  double
      // Going with an hopefully possible 1 in accuracy
      minAllowedDist = controller->settings.exitDist; // The maximum distance from target before starting timeIn count
  // cout << minAllowedDist << endl;
#undef DEBUG
#ifdef DEBUG
  struct
  {
    vector<double> outSpeeds, encSpeeds, targSpeeds, angles, cp, cd, sp, sd;
    vector<PVector> pos, pursuit;
    void add(double out, double enc, double targ, PVector p, double angle, double acp, double acd, double asp, double asd, PVector apursuit)
    {
      cout << "%"
           << "outputVel: " << out << ", "
           << "encVel: " << enc << ", "
           << "targetVel: " << targ << ", "
           << "main.pos: " << p.x << "@" << p.y << ", "
           << "main.angle: " << angle << ", "
           << "slaveP: " << asp << ", "
           << "slaveD: " << asd << ", "
           << "ctrlP: " << acp << ", "
           << "ctrlD: " << acd << ", "
           << "main.pursuit: " << apursuit.x << "@" << apursuit.y << endl;
      // outSpeeds.push_back(out);
      // encSpeeds.push_back(enc);
      // targSpeeds.push_back(targ);
      // pos.push_back(p);
      // angles.push_back(angle);
      // cp.push_back(acp);
      // cd.push_back(acd);
      // sp.push_back(asp);
      // sd.push_back(asd);
      // pursuit.push_back(apursuit);
    }
  } realTime;
#endif
  // Save the current distance fns
  setOldDistFns();
  int timesStopped = 0;
  moving = true;
  PVector lastPos = botPos();
  int lastIndex = 0;
  drawFn = [this]()
  {
    Brain.Screen.clearScreen(black);
    for (int i = 0; i < path.size(); i++)
    {
      // Brain.Screen.printAt(10, i * 10, "%f", path[i].targetSpeed);
      Brain.Screen.drawCircle(i, path[i].targetSpeed, 1);
    }
  };
  double dist = 0.0; // The distance from the target
#ifdef DEBUG
  cout.precision(1);
  // Loop through path and print out all the points
  for (int i = 0; i < path.size(); i++)
  {
    cout << "%main: " << path[i].x << "@" << path[i].y << endl;
  }
#endif
  // Loop
  while (timeIn * sleepTime < maxTimeIn)
  {

    // Get the nearest pure pursuit position
    int nearestIndex = getNearest(path, botPos(), lastIndex);
    lastIndex = nearestIndex;
    if (exitEarly)
    {
      cout << "Exit due to external thread request" << endl;
      exitEarly = false;
      break;
    }

    // Keep the Pure Pursuit target purePursuitDist inches away from the bot
    while (pursuit.dist2D(botPos()) < purePursuitDist && pursuit != path.last())
    {
      pursuit = path[bezierIndex];
      ++bezierIndex;
    }

    if (bezierIndex >= path.size())
    {
      bezierIndex = path.size() - 1;
    }
    // Near the target, increment timeIn
    if (dist < minAllowedDist && pursuit == path.last())
    {
      timeIn++;
    }
    else
    {
      timeIn = 0;
    }

    // The distance to the pursuit target
    dist = botPos().dist2D(pursuit);
#ifndef TEST
    // If the bot's not moving, and it's not currently accelerating
    if (chassis->pos.velocity() < 0.1 && t.time(timeUnits::msec) > 1000)
    {
      timesStopped++;
    }
    else
    {
      timesStopped = 0;
    }
#endif
    // 50 ms not moving -> exit
    if (timesStopped * sleepTime > 50 && !stopExitPrev)
    {
      cout << "Stop Exit" << endl;
      break;
    }
    // Use the distFns for the current dist
    useDistFns(botPos().dist2D(path.last()));
    // The point extended beyond the path to make sure normAngle doesn't get big near the target
    PVector virtualPursuit = generalFollowGetVirtualPursuit(pursuit, controller);
    // cout << virtualPursuit << endl;
    dist = generalFollowGetDist(bezierIndex, controller, pursuit);
    // cout << "Cool" << endl;
    // Angle of robot to target
    double angle = baseAngle(botPos().angleTo(virtualPursuit));

    // The angle that it needs to travel at
    // double normAngle = posNeg180(angle - botAngle() + 180 * isNeg);
    // cout << "N " << normAngle << endl;
    // cout << "D " << dist << endl;
    Controller::Input input = Controller::Input();
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
    // So that the robot can take tight turns,
    // if the turn is too tight, then the robot direction of travel will flip
    // Also if it's basically at the end,
    //   then the robot will take the more efficient path backwards to the target
    //   rather than turn around
    // if (abs(normAngle) >= 150)
    // {
    //   // isNeg = !isNeg;
    //   //  cout << botPos() << ", " << pursuit << endl;
    //   //  cout << normAngle << endl;
    //   //  cout << "Reverse Neg" << endl;
    //   // Decrease timeIn because we aren't doing any sleeping this round
    //   if (timeIn > 0)
    //   {
    //     // timeIn--;
    //   }
    //   // Send it back up top to recalibrate the speeds without sleeping
    //   // continue;
    // }

    auto speeds = controller->followTo(input);
    // cout << "S1: " << speeds.first.first << endl;
    double speed = 0;
    switch (speeds.first.second)
    {
    case Controller::ForwardVel::inps:
      speed = chassis->realToPct(speeds.first.first);
      // cout << "S2: " << speed << endl;
      break;
    case Controller::ForwardVel::pct:
      speed = speeds.first.first;
      break;
    }
    // speed *= -(isNeg * 2.0 - 1);

    if (g++ == 2)
    {
      // cout << normAngle << ", " << extraSpeed << endl;
      //  cout << extraSpeed << ", " << normAngle << endl;
      //  cout << speed << ", " << dist << endl;
      g = 0;
    }
    // double targetVel = abs(speed);
    [[maybe_unused]] double orgSpeed = speed + 0.000000001;
    // Slew speed
    if (abs(speed) > abs(path[nearestIndex].targetSpeed))
    {
      orgSpeed = speed;
      speed = path[nearestIndex].targetSpeed * sign(speed);
    }
    // cout << "S3: " << speed << endl;
    if (abs(speed) > abs(chassis->speedLimit))
    {
      orgSpeed = speed;
      speed /= abs(speed);
      speed *= chassis->speedLimit;
    }
    // cout << "S4: " << speed << endl;
    double rightExtra;
    {
      double targetRobotVel = chassis->pctToReal(speed);
      switch (speeds.second.second)
      {
      case Controller::AngularVel::curvature:
        rightExtra = chassis->realToPct(speeds.second.first * (chassis->trackWidth + 3.0) * targetRobotVel / 2.0);
        break;
      case Controller::AngularVel::pctDiff:
        rightExtra = speeds.second.first;
        break;
      case Controller::AngularVel::radps:
        rightExtra = chassis->realToPct(speeds.second.first * chassis->trackWidth / -2.0);
        break;
      }
    }
    // This is half done, pls fix
    //  if ((abs(speed + rightExtra) > 100.0 || abs(speed - rightExtra) > 100.0) && abs(rightExtra) < 100) {
    //    double newFwdVel = chassis->pctToReal(speed);
    //    double left, right;
    //    switch (speeds.second.second) {
    //    case Controller::AngularVel::curvature: [[likely]]
    //    TOP:
    //      left = chassis->realToPct(newFwdVel * (2.0 + speeds.second.first * chassis->trackWidth) / 2.0);
    //      right = chassis->realToPct(newFwdVel * (2.0 - speeds.second.first * chassis->trackWidth) / 2.0);
    //      if (abs(left) > 100) {
    //        newFwdVel = newFwdVel * 99.0 / left;
    //        goto TOP;
    //      }
    //      if (abs(right) > 100) {
    //        newFwdVel = newFwdVel * 99.0 / right;
    //        goto TOP;
    //      }
    //      speed = chassis->realToPct(newFwdVel);
    //      break;
    //    case Controller::AngularVel::pctDiff:
    //    TOP2:
    //      left = speed + speeds.second.first;
    //      right = speed - speeds.second.first;
    //      if (abs(left) > 100) {
    //        speeds.second.first = speeds.second.first * 99.0 / left;
    //        goto TOP2;
    //      }
    //      if (abs(right) > 100) {
    //        speeds.second.first = speeds.second.first * 99.0 / right;
    //        goto TOP2;
    //      }
    //      break;
    //    case Controller::AngularVel::radps:
    //    TOP3:
    //      rightExtra = chassis->pctToReal(speed) * chassis->trackWidth / -2.0;
    //      if (abs(speed + rightExtra) > 100 || abs(speed - rightExtra) > 100) {
    //        if (abs(speed + rightExtra) > 100) {
    //          speed = speed * 99.0 / (speed + rightExtra);
    //        }
    //        if (abs(speed - rightExtra) > 100) {
    //          speed = speed * 99.0 / (speed - rightExtra);
    //        }
    //        goto TOP3;
    //      }
    //      break;
    //    }
    //  }
    if (isNeg)
    {
      // rightExtra *= -1;
      speed *= -1.0;
    }
    // speed *= -1.0;
    // cout << "S: " << speed << endl;
    // Mindblowing lines right here
    // Move the robot
    chassis->driveFromDiff(speed, -rightExtra, fwd);
    lastPos = botPos();
    // Sleep (WOW, HE'S A GENIUS)

    s(sleepTime);
#ifdef TEST
    chassis->runSimStep();
#endif

#ifdef DEBUG
    realTime.add(speed, 0, path[nearestIndex].targetSpeed, botPos(), botAngle(), normAngle, input.dist, -rightExtra, 0, pursuit);
#endif
  }
  moving = false;
  // Stop drawing the path
  // De-init code
  {
    // Set the last target for external stuff
    lastTarget = bezier.last();
    // Stop the bot
    switch (BrakeMode)
    {
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
    // Print postion and target position
    cout << botPos() << ", " << bezier.last() << endl;
    exitDist = 0.0;
  }
  stopExitPrev = false;
  controller->deInit();
// Print all the lists
#ifdef DEBUG
  // s(20000);
  // cout << endl
  //      << endl;
  // cout << "p.frameRate(" << 1.0 / (double)sleepTime * 1000 << ");\n";
  // cout << "main.inputData([";
  // int line = 0;
  // for (auto i : bezier)
  // {
  //   cout << "p.createVector(" << i << "), " << (line++ % 3 == 0 ? "\n" : "");
  //   s(20);
  // }
  // s(100);
  // cout << "]);";
  // cout << "\nmain.auxiliaryData.pos = [";
  // for (auto i : realTime.pos)
  // {
  //   cout << "p.createVector(" << i << "), " << (line++ % 3 == 0 ? "\n" : "");
  //   s(20);
  // }
  // s(100);
  // cout << "];\n\nmain.auxiliaryData.angle = [";
  // for (auto i : realTime.angles)
  // {
  //   cout << i << ", " << (line++ % 3 == 0 ? "\n" : "");
  //   s(10);
  // }
  // s(100);
  // cout << "];\n\nmain.auxiliaryData.pursuit = [";
  // for (auto i : realTime.pursuit)
  // {
  //   cout << "p.createVector(" << i << "), " << (line++ % 3 == 0 ? "\n" : "");
  //   s(20);
  // }
  // s(100);
  // cout << "];\n";
  // cout << "main.resetI();main.setHighlight(0);\n";
  // cout << "outputVel.inputData([" << flush;
  // s(100);
  // for (auto i : realTime.outSpeeds)
  // {
  //   cout << i << ", " << (line++ % 3 == 0 ? "\n" : "");
  //   s(20);
  // }
  // s(100);
  // cout << "]);\n";
  // cout << "encVel.inputData([";
  // for (auto i : realTime.encSpeeds)
  // {
  //   cout << i << ", " << (line++ % 3 == 0 ? "\n" : "");
  //   s(20);
  // }
  // s(100);
  // cout << "]);\n";
  // cout << "targetVel.inputData([";
  // for (auto i : realTime.targSpeeds)
  // {
  //   cout << i << ", " << (line++ % 3 == 0 ? "\n" : "");
  //   s(10);
  // }
  // s(100);
  // cout << "]);\n";
  // cout << "slaveP.inputData([";
  // for (auto i : realTime.sp)
  // {
  //   cout << i << ", " << (line++ % 3 == 0 ? "\n" : "");
  //   s(10);
  // }
  // s(100);
  // cout << "]);\n";
  // cout << "slaveD.inputData([";
  // for (auto i : realTime.sd)
  // {
  //   cout << i << ", " << (line++ % 3 == 0 ? "\n" : "");
  //   s(10);
  // }
  // s(100);
  // cout << "]);\n";
  // cout << "ctrlP.inputData([";
  // for (auto i : realTime.cp)
  // {
  //   cout << i << ", " << (line++ % 3 == 0 ? "\n" : "");
  //   s(10);
  // }
  // s(100);
  // cout << "]);\n";
  // cout << "ctrlD.inputData([";
  // for (auto i : realTime.cd)
  // {
  //   cout << i << ", " << (line++ % 3 == 0 ? "\n" : "");
  //   s(10);
  // }
  // s(100);
  // cout << "]);\n";
  // cout << "main.xRange = [48, -48]; main.yRange = [60, -60]; \nctrlD.customizeRange();\nctrlP.customizeRange();\nslaveP.customizeRange();\nslaveD.customizeRange();\ntargetVel.customizeRange();\nencVel.customizeRange();\noutputVel.customizeRange();";
  // cout << endl;
#endif
}
void WheelController::generalDriveDistance(double targetDist, bool isNeg, BasicPidController *pid)
{
  PVector startPos = botPos();
  double startAngle = botAngle();
  int timeIn = 0;
  int maxTimeIn = pid->settings.maxTimeIn;
  double maxDist = pid->settings.followPathDist;
  int sleepTime = 10;
  setOldDistFns();
  moving = true;
  double dist = 0.0;
  timer t = timer();
  int timesStopped = 0;
  pid->init();
  while (1)
  {
    // Basic exit conditions
    if (timeIn * sleepTime > maxTimeIn)
    {
      break;
    }
    // 50 ms not moving -> exit
    if (timesStopped * sleepTime > 50 && !stopExitPrev)
    {
      cout << "Stop Exit" << endl;
      break;
    }
    // If the bot's not moving, and it's not currently accelerating
    if (chassis->pos.velocity() < 0.1 && t.time(timeUnits::msec) > 1000)
    {
      timesStopped++;
    }
    else
    {
      timesStopped = 0;
    }
    if (exitEarly)
    {
      cout << "Exit due to external thread request" << endl;
      exitEarly = false;
      break;
    }
    // If near target, start the timer
    if (dist < pid->settings.exitDist)
    {
      timeIn++;
    }
    else
    {
      timeIn = 0;
    }
    double angle = botAngle();
    PVector pos = botPos();
    double dist = targetDist - pos.dist(startPos);
    Controller::Input input = Controller::Input();
    // Construct the inpur
    input.angleTarget = startAngle;
    input.currentAngle = angle;
    input.chassis = chassis;
    input.dist = dist > maxDist ? maxDist : dist;
    auto speeds = pid->followTo(input);
    double speed = 0;
    // Convert the speed to a percentage
    switch (speeds.first.second)
    {
    case Controller::ForwardVel::inps:
      speed = chassis->realToPct(speeds.first.first);
      break;
    case Controller::ForwardVel::pct:
      speed = speeds.first.first;
      break;
    }

    double rightExtra;
    // get the turn speed
    {
      double targetRobotVel = chassis->pctToReal(speed);
      switch (speeds.second.second)
      {
      case Controller::AngularVel::curvature:
        rightExtra = chassis->realToPct(speeds.second.first * (chassis->trackWidth + 3.0) * targetRobotVel / 2.0);
        break;
      case Controller::AngularVel::pctDiff:
        rightExtra = speeds.second.first;
        break;
      case Controller::AngularVel::radps:
        rightExtra = chassis->realToPct(speeds.second.first * chassis->trackWidth / -2.0);
        break;
      }
    }
    if (isNeg)
    {
      speed *= -1.0;
    }
    chassis->driveFromDiff(speed, -rightExtra, fwd);

    s(sleepTime);
  }
  moving = false;
  // Stop drawing the path
  // De-init code
  {
    // Stop the bot
    switch (BrakeMode)
    {
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
    // Print postion and target position
    cout << dist << ", " << targetDist << endl;
    exitDist = 0.0;
  }
  stopExitPrev = false;
  pid->deInit();
}
void WheelController::driveDistance(double dist, BasicPidController *pid)
{
  generalDriveDistance(dist, false, pid);
}
void WheelController::backwardsDriveDistance(double dist, BasicPidController *pid)
{
  generalDriveDistance(dist, true, pid);
}
// The beefiest function in this file
void WheelController::purePursuitFollow(VectorArr arr, bool isNeg)
{
  generalFollow(arr, defaultPurePursuit, isNeg);
}
bool WheelController::isRed()
{
  return !reversed;
}
void WheelController::setRed()
{
  reversed = false;
}
void WheelController::setBlue()
{
  reversed = true;
}
bool WheelController::isBlue()
{
  return reversed;
}

void MechWheelController::moveAt(double angle, double speed, double turnSpeed)
{
  // There are actual equations governing this
  //  double Y1 = cos(angle * DEG_TO_RAD) * speed;
  //  double Y2 = cos(angle * DEG_TO_RAD) * speed;
  //  double X1 = sin(angle * DEG_TO_RAD) * speed;
  //  double X2 = sin(angle * DEG_TO_RAD) * speed;
  //  double FLS = Y1 + X1;
  //  double BLS = Y1 - X1;
  //  double FRS = Y2 - X2;
  //  double BRS = Y2 + X2;
  //  FLS += turnSpeed;
  //  BLS += turnSpeed;
  //  FRS -= turnSpeed;
  //  BRS -= turnSpeed;
  //  //Spin da motors
  //  BL->spin(vex::forward, BLS, velocityUnits::pct);
  //  BR->spin(vex::forward, BRS, velocityUnits::pct);
  //  FL->spin(vex::forward, FLS, velocityUnits::pct);
  //  FR->spin(vex::forward, FRS, velocityUnits::pct);
}
void MechWheelController::followPath(VectorArr arr, double targetAngle)
{
}
void MechWheelController::driveTo(PVector pos, double finalAngle)
{
  followPath({pos}, finalAngle);
}
void MechWheelController::driveTo(double x, double y, double finalAngle)
{
  driveTo(PVector(x, y), finalAngle);
}
void MechWheelController::backInto(double x, double y, double finalAngle)
{
  backInto(PVector(x, y), finalAngle);
}
void MechWheelController::backInto(PVector pos, double finalAngle)
{
  backwardsFollow({pos}, finalAngle);
}
void MechWheelController::backwardsFollow(VectorArr arr, double targetAngle)
{
}