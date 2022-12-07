#define NO_MAKE
#include "Controllers.h"
#include <cstdint>
#include <iomanip>
#include <algorithm>
// That should be a sufficient path generator
Path::chain_method Path::setK(double s)
{
  kConst = s;
  CHAIN
}
double clamp(double val, double high, double low)
{
  if (val > high)
  {
    return high;
  }
  else if (val < low)
  {
    return low;
  }
  return val;
}
void Path::make(VectorArr &arr, Chassis *chassis)
{
  // cout << "Making path" << endl; s(100);
  this->path.clear();
  this->arr = arr;
  auto arrCopy = VectorArr(arr);
  // cout << "Path size: " << arr.size() << endl; s(100);
  arrCopy.push_front(chassis->botPos());

  auto bezier = VectorArr(bezierCurve(arrCopy));
  // cout << "Bezier size: " << bezier.size() << endl; s(100);
  vector<double> targetAngles;
  vector<double> curvatures;
  vector<double> targetSpeeds;
  auto deriv = VectorArr(bezierDerivative(arrCopy));
  // cout << "Derivative size: " << deriv.size() << endl; s(100);
  for (auto i : deriv)
  {
    targetAngles.push_back(i.heading2D());
  }
  // cout << "Target angles size: " << targetAngles.size() << endl; s(100);
  curvatures = bezierCurvature(arrCopy);
  targetSpeeds = vector<double>();
  for (int i = 0; i < bezier.size(); i++)
  {
    targetSpeeds.push_back(abs(min(chassis->speedLimit, kConst / curvatures[i])));
    if (targetSpeeds.back() > 100)
    {
      targetSpeeds.back() = 100;
    }
  }

  // cout << "Target speeds size: " << targetSpeeds.size() << endl; s(100);
  // Smooth targetSpeeds
  // vf^2 = vi^2 + 2ad
  double startVel = 50;
  targetSpeeds[0] = startVel;
  targetSpeeds[targetSpeeds.size() - 1] = 40;

  for (int i = 1; i < bezier.size(); i++)
  {
    // I think this math of converting inches to percent works
    double d = (bezier)[i].dist2D((bezier)[i - 1]);
    double a = chassis->maxAcc;
    targetSpeeds[i] = abs(targetSpeeds[i]);
    if (startVel < targetSpeeds[i] && i != bezier.size() - 1)
    {
      startVel = targetSpeeds[i] = clamp(chassis->realToPct(sqrt(pow(chassis->pctToReal(startVel), 2) + 2.0 * a * d)), abs(targetSpeeds[i]), 0);
      // if(startVel > targetSpeeds[i + 1]){
      //   targetSpeeds[i] = startVel = targetSpeeds[i + 1];
      // }
    }
    else if (startVel > targetSpeeds[i] && i != 0)
    {
      int startI = i;
      // Go backwards until reached speed
      do
      {
        startVel = targetSpeeds[i];
        i--;
        double a = chassis->maxDAcc;
        double d = (bezier)[i].dist2D((bezier)[i + 1]);
        startVel = targetSpeeds[i] = clamp(chassis->realToPct(sqrt(pow(chassis->pctToReal(startVel), 2) + 2.0 * a * d)), abs(targetSpeeds[i]), 0);
      } while (i != 0 && startVel < targetSpeeds[i - 1]);
      i = startI;
      startVel = targetSpeeds[i];
    }
  }
  // cout << "Target speeds size: " << targetSpeeds.size() << endl; s(100);
  for (auto &i : targetSpeeds)
  {
    i = abs(i);
  }
  // cout << "Target speeds size: " << targetSpeeds.size() << endl; s(100);
  if (curvatures.size() != bezier.size() || targetSpeeds.size() != bezier.size() || targetAngles.size() != bezier.size())
  {
    cout << "Err in making path\ncurvatures: " << curvatures.size()
         << "\nbezier: " << bezier.size()
         << "\ntargetSpeeds: " << targetSpeeds.size()
         << "\ntargetAngles: " << targetAngles.size() << endl;
  }
  // cout << "Path made" << endl; s(100);
  for (int i = 0; i < bezier.size(); i++)
  {
    path.push_back(El{(bezier)[i], targetSpeeds[i], targetAngles[i], curvatures[i]});
  }
  // s(300000);
  // cout << "Path size: " << path.size() << endl; s(100);
}
void Path::remake(Chassis *chassis)
{
  make(arr, chassis);
}
VectorArr Path::getBezier()
{
  VectorArr ret = {};
  for (auto i : path)
  {
    ret.push(i.bezierPt);
  }
  return ret;
}
vector<Ref<PVector>> Path::getBezierRef()
{
  vector<Ref<PVector>> ret = {};
  for (auto i : path)
  {
    ret.push_back(i.bezierPt);
  }
  return ret;
}
int Path::size()
{
  return path.size();
}
Path::El &Path::last()
{
  return path.back();
}
Path::El &Path::operator[](int index)
{
  return path[index];
}
void Controller::init()
{
}
void Controller::deInit()
{
  turnAtStart = defaultTurn;
}
PurePursuitController::followToRet PurePursuitController::followTo(Input &input)
{
  double travelCurvature;
  {
    auto rPos = input.position;
    auto lPos = input.targetPt.bezierPt;
    double moddedAngle = input.currentAngle * DEG_TO_RAD;
    double
        side =
            sign(
                -cos(moddedAngle) * (lPos.x - rPos.x) + sin(moddedAngle) * (lPos.y - rPos.y)),
        a = -tan(moddedAngle),
        b = 1.0,
        c = tan(moddedAngle) * rPos.y - rPos.x;
    double x = abs(a * lPos.y + b * lPos.x + c) * sqrt(pow(a, 2) + pow(b, 2)) * side;
    travelCurvature = 2.0 * x / pow(input.dist, 2);
  }
  double normAngle = posNeg180(input.angleTarget - input.currentAngle);
  if (abs(normAngle) < 10)
  {
    travelCurvature /= 2.0;
  }
  if (input.dist < 10)
  {
    travelCurvature = 0;
  }
  // Get the target speed of the robot
  double speed = -ctrl.getVal(input.dist);

  return {{speed, ForwardVel::pct}, {travelCurvature, AngularVel::curvature}};
}
void PurePursuitController::init()
{
  ctrl.setTarget(0);
}
PurePursuitController::PurePursuitController(PID input) : PurePursuitController(input, BasicWheelController::PathFollowSettings())
{
}
PurePursuitController::PurePursuitController(PID input, BasicWheelController::PathFollowSettings settings)
{
  ctrl = input;
  this->settings = settings;
}
RamseteController::followToRet RamseteController::followTo(Input &input)
{
  double theta = posNeg180(input.currentAngle);      // deg
  double targetAngle = posNeg180(input.angleTarget); // deg
  auto pursuit = input.target;
  auto pos = input.position;
  double tRad = theta * DEG_TO_RAD;
  Matrix<double, 3, 3> transformation = {
      {cos(tRad), -sin(tRad), 0},
      {sin(tRad), cos(tRad), 0},
      {0, 0, 1}};
  Matrix<double, 3, 1> globalErr = {
      {pursuit.x - pos.x /*in*/},
      {pursuit.y - pos.y /*in*/},
      {posNeg180(targetAngle - theta) * DEG_TO_RAD /*rad*/}};
  double vd = input.chassis->pctToReal(input.targetPt.targetSpeed); // inps
  double Wd = vd * input.targetPt.curvature;
  Matrix<double, 3, 1> error = transformation * globalErr;
  double k = 2.0 * zeta * sqrt(Wd * Wd /*rad^2/s^2*/ + beta * vd * vd /*rad^2/s^2*/); // 1/s^2
  double eTheta = error(2, 0) /*rad*/;
  double speed = vd * cos(eTheta) + k * error(1, 0); // inps
  cout << Wd << endl;
  double turnVel = Wd + k * eTheta + beta * vd * sin(eTheta) / (eTheta + eTheta == 0 ? 0.00001 : 0) * error(0, 0);
  return {{speed, ForwardVel::inps}, {turnVel, AngularVel::radps}};
}
RamseteController::RamseteController(double beta, double zeta, BasicWheelController::PathFollowSettings settings) : Controller()
{
  this->beta = beta;
  this->zeta = zeta;
  this->settings = settings;
}
RamseteController::RamseteController(double beta, double zeta) : RamseteController(beta, zeta, BasicWheelController::PathFollowSettings())
{
}
BasicPidController::BasicPidController(PID ctrl, PID slave, BasicWheelController::PathFollowSettings settings) : Controller()
{
  this->ctrl = ctrl;
  this->slave = slave;
  this->settings = settings;
}
BasicPidController::BasicPidController(PID ctrl, PID slave) : BasicPidController(ctrl, slave, BasicWheelController::PathFollowSettings())
{
}

BasicPidController::followToRet BasicPidController::followTo(Input &input)
{
  double dist = input.dist;
  double normAngle = posNeg180(input.angleTarget - input.currentAngle);
  // cout << normAngle << endl;
  double fwdVel = -ctrl.getVal(dist);
  if (dist < 0)
  {
    normAngle = posNeg180(normAngle + 180);
  }
  double turnVel = slave.getVal(posNeg180(normAngle));

  return {{fwdVel * 0.8, Controller::ForwardVel::pct}, {turnVel * 3.0, Controller::AngularVel::pctDiff}};
}
void BasicPidController::init()
{
  ctrl.setTarget(0);
  slave.setTarget(0);
}
DebugController::DebugController(BasicWheelController::PathFollowSettings settings) : Controller()
{
  this->settings = settings;
}
DebugController::followToRet DebugController::followTo(Input &input)
{
  static int i = 0;
  // Print all the input data
  cout << "Point: " << i++ << "\n";
  cout << "Target: " << input.target << "\n";
  cout << "Position: " << input.position << "\n";
  cout << "Current Angle: " << input.currentAngle << "\n";
  cout << "Target Angle: " << input.angleTarget << "\n";
  cout << "Distance: " << input.dist << "\n";
  cout << "Target Speed: " << input.targetPt.targetSpeed << "\n";
  cout << "Curvature: " << input.targetPt.curvature << "\n\n";
  if (Greg.ButtonA.pressing())
    return {{0, Controller::ForwardVel::pct}, {0, Controller::AngularVel::pctDiff}};
  return {{30, Controller::ForwardVel::pct}, {0, Controller::AngularVel::pctDiff}};
}