#define NO_MAKE
#define DISTANCE_CPP
#include "Sensors/Distance.h"
double Distance::sensorVariance(){
  if(sensor.objectDistance(mm) > 200){
    //Over 200 mm, accuracy is +/- 5%
    return sensor.objectDistance(mm) * sensor.objectDistance(mm) * 0.05 * 0.05;
  }
  else {
    return sensorSigma * sensorSigma;
  }
}
void Distance::initLoop(){
  prediction = estimate = {{sensor.objectDistance(mm)}, {0}, {0}};
  uncertainty = {
    {sensorVariance(), 0,   0},
    {               0, 100, 0},
    {               0, 0, 200}
  };
  predictionUncertainty = uncertainty;
  
}
void Distance::test(){
  if(sensor.isObjectDetected() && sensor.objectDistance(inches) < 20){
    if(!lastOff){
      initLoop();
    }
    TOP:
    Rn[0][0] = sensorVariance();
    auto Kn = predictionUncertainty * H.transpose() * (H * predictionUncertainty * H.transpose() + Rn).inverse();
    Matrix<double, 1, 1> zn;
    zn[0][0] = sensor.objectDistance(mm);
    //Reset filter there is a high velocity maneuver
    if(abs(sensor.objectDistance(mm) - prediction[0][0]) > 80 /*(about 2 inches)*/){
      double prevEst = prediction[0][0];
      initLoop();

      //Give the new filter the best guess for velocity and accelleration
      estimate[1][0] = prediction[1][0] = (prevEst - zn[0][0]) / deltaT;
      estimate[2][0] = prediction[2][0] = 1.0 / 2.0 * (prevEst - zn[0][0]) / (deltaT * deltaT);
      goto TOP;
    }
    estimate = prediction + Kn * (zn - H * prediction);
    uncertainty = (Id3 - Kn * H) * predictionUncertainty * (Id3 - Kn * H).transpose() + Kn * Rn * Kn.transpose();
    //Use robot velocity as control input, bc we are assuming that the goals aren't moving during auton
    //Making the assumption that we are moving roughly towards the goal +/- 5 degrees
    //Could have a negative process noise for auton (because the goals are being taken away)
    double ctrlInput = -share.velocity();
    prediction = F * estimate + Gn * ctrlInput;
    predictionUncertainty = F * uncertainty * F.transpose() + Q;
    lastOff = true;
  }
  else {
    //Force re-initialization next time object detected
    lastOff = false;
  }
}
void Distance::endFiltering(){
  lastOff = false;
}
double Distance::rawDist(distanceUnits d){
  return sensor.objectDistance(d);
}
double Distance::objectDistance(distanceUnits d){
  if(!lastOff){
    return 0;
  }
  if(d == distanceUnits::in){
    return systemEstimate() / 25.4;
  }
  if(d == distanceUnits::cm){
    return systemEstimate() / 10;
  }
  else {
    return systemEstimate();
  }
}
void Distance::sleep(){
  s(deltaT * 1000.0);
}
bool Distance::isObjectDetected(){
  return lastOff;
}
double Distance::systemEstimate(){
  if(!lastOff){
    return 0;
  }
  return estimate(0, 0);
}
bool Distance::installed(){
  return sensor.installed();
}