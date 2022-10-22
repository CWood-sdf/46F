#define NO_MAKE
#include "Odometry/Controllers.h"
#include <cstdint>
#include <iomanip> 
#include <algorithm>
//That should be a sufficient path generator
Path::chain_method Path::setK(double s){
  kConst = s;
  CHAIN
}
double clamp(double val, double high, double low){
  if(val > high){
    return high;
  }
  else if(val < low){
    return low;
  }
  return val;
}
void Path::make(VectorArr arr, Chassis* chassis){
  this->path.clear();
  this->arr = arr;
  auto arrCopy = arr;
  arrCopy.push_front(chassis->botPos());
  
  auto bezier = bezierCurve(arrCopy);
  vector<double> targetAngles;
  vector<double> curvatures;
  vector<double> targetSpeeds;
  auto deriv = bezierDerivative(arrCopy);
  for(auto i : deriv){
    targetAngles.push_back(i.heading2D());
  }
  curvatures = bezierCurvature(arrCopy);
  targetSpeeds = vector<double>();
  
  for(int i = 0; i < bezier.size(); i++){
    targetSpeeds.push_back(abs(min(chassis->speedLimit, kConst / curvatures[i])));
    if(targetSpeeds.back() > 100){
      targetSpeeds.back() = 100;
    }
  }
  //Smooth targetSpeeds
  //vf^2 = vi^2 + 2ad
  double startVel = 30;
  targetSpeeds[0] = startVel;
  targetSpeeds.back() = 40;
  for(int i = 1; i < bezier.size(); i++){
    //I think this math of converting inches to percent works
    double d = bezier[i].dist2D(bezier[i - 1]);
    double a = chassis->maxAcc;
    if(startVel < targetSpeeds[i]){
      startVel = targetSpeeds[i] = clamp(sqrt(pow(startVel * 3.75 * M_PI / 9.0, 2) + 2.0 * a * d) * (9.0 / (3.75 * M_PI)), -abs(targetSpeeds[i]), abs(targetSpeeds[i]));
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
        double a = chassis->maxDAcc;
        double d = bezier[i].dist2D(bezier[i + 1]);
        startVel = targetSpeeds[i] = clamp(sqrt(pow(startVel * 3.75 * M_PI / 9.0, 2) + 2.0 * a * d) * (9.0 / (3.75 * M_PI)), -abs(targetSpeeds[i]), abs(targetSpeeds[i]));
      } while(startVel < targetSpeeds[i - 1]);
      i = startI;
      startVel = targetSpeeds[i];
    }
  }
  if(curvatures.size() != bezier.size() || targetSpeeds.size() != bezier.size() || targetAngles.size() != bezier.size()){
    cout << "Err in making path\ncurvatures: " << curvatures.size() 
          << "\nbezier: " << bezier.size() 
          << "\ntargetSpeeds: " << targetSpeeds.size()
          << "\ntargetAngles: " << targetAngles.size() << endl;
    
  }
  for(int i = 0; i < bezier.size(); i++){
    path.push_back(El{bezier[i], targetSpeeds[i], targetAngles[i], curvatures[i]});
  }
}
void Path::remake(Chassis* chassis){
  make(arr, chassis);
}
VectorArr Path::getBezier(){
  VectorArr ret = {};
  for(auto i : path){
    ret.push(i.bezierPt);
  }
  return ret;
}
vector<Ref<PVector>> Path::getBezierRef(){
  vector<Ref<PVector>> ret = {};
  for(auto i : path){
    ret.push_back(i.bezierPt);
  }
  return ret;
}
int Path::size(){
  return path.size();
}
Path::El& Path::last(){
  return path.back();
}
Path::El& Path::operator[](int index){
  return path[index];
}
void Controller::init(){

}
void Controller::deInit(){
  turnAtStart = defaultTurn;
}
PurePursuitController::followToRet PurePursuitController::followTo(Input& input) {
  double travelCurvature;
  {
    auto rPos = input.position;
    auto lPos = input.target;
    double moddedAngle = input.currentAngle * DEG_TO_RAD;
    double 
      side = 
        sign(
          -cos(moddedAngle) * (lPos.x - rPos.x)
          +sin(moddedAngle) * (lPos.y - rPos.y)
        ),
      a = -tan(moddedAngle),
      b = 1.0,
      c = tan(moddedAngle) * rPos.y - rPos.x;
    
    double x = abs(a * lPos.y + b * lPos.x + c) * sqrt(pow(a, 2) + pow(b, 2)) * side;
    travelCurvature = 2.0 * x / pow(input.dist, 2);
  }
  //Get the target speed of the robot
  double speed = ctrl.getVal(abs(input.dist));
  return {{speed, ForwardVel::pct}, {travelCurvature, AngularVel::curvature}};
}
void PurePursuitController::init(){
  ctrl.setTarget(0);
}
PurePursuitController::PurePursuitController(PID input){
  ctrl = input;
}
RamseteController::followToRet RamseteController::followTo(Input &input)  {
  double theta = posNeg180(input.currentAngle);//deg
  double targetAngle = posNeg180(input.angleTarget);//deg
  auto pursuit = input.target;
  auto pos = input.position;
  double tRad = theta * DEG_TO_RAD;
  Matrix<double, 3, 3> transformation = {
    { cos(tRad), -sin(tRad), 0},
    { sin(tRad),  cos(tRad), 0},
    {    0,            0,     1}
  };
  Matrix<double, 3, 1> globalErr = {
    {pursuit.x - pos.x /*in*/},
    {pursuit.y - pos.y /*in*/},
    {posNeg180(targetAngle - theta)*DEG_TO_RAD /*rad*/}
  };
  double vd = input.chassis->pctToReal(input.targetPt.targetSpeed); //inps
  double Wd = vd * input.targetPt.curvature;
  Matrix<double, 3, 1> error = transformation * globalErr;
  double k = 2.0 * zeta * sqrt(Wd * Wd/*rad^2/s^2*/ + beta * vd * vd/*rad^2/s^2*/);//1/s^2
  double eTheta = error(2, 0)/*rad*/;
  double speed = vd * cos(eTheta) + k * error(1, 0);//inps
  double turnVel = Wd + k * eTheta + beta * vd * sin(eTheta) / eTheta * error(0, 0);
  return {{speed, ForwardVel::inps}, {turnVel, AngularVel::radps}};
}
RamseteController::RamseteController(double beta, double zeta) : Controller() {
  this->beta = beta;
  this->zeta = zeta;
}
BasicPidController::BasicPidController(PID ctrl, PID slave){
  this->ctrl = ctrl;
  this->slave = slave;
}

BasicPidController::followToRet BasicPidController::followTo(Input &input){
  double dist = input.dist;
  double normAngle = posNeg180(input.angleTarget - input.currentAngle);
  double fwdVel = ctrl.getVal(dist);
  double turnVel = slave.getVal(normAngle);
  return {{fwdVel, Controller::ForwardVel::pct}, {turnVel, Controller::AngularVel::pctDiff}};
}
void BasicPidController::init(){
  ctrl.setTarget(0);
  slave.setTarget(0);
}