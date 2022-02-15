#include "Odometry/GPS_Share.h"
#define TO_4(x) x*x*x*x
#define TO_3(x) x*x*x
#define TO_2(x) x*x

extern GPS_Share share;
class Distance {
  static constexpr double deltaT = 0.005; // Update loop every 5ms
  double sensorSigma = 15; // The pathetic +/- 15mm accuracy
  double sigmaA = 10; // Random variance in accelleration
  Matrix<double, 3, 1> estimate = {{0}, {0}, {0}};
  Matrix<double, 3, 1> prediction = {{0}, {0}, {0}};
  Matrix<double, 3, 1> Gn = {{deltaT}, {1.0}, {1.0 / deltaT}};
  Matrix<double, 3, 3> F;

  Matrix<double, 3, 3> Q;
  //{ sensorSigma * sensorSigma }
  Matrix<double, 1, 1> Rn;
  // { 1, 0, 0}
  Matrix<double, 1, 3> H;
  
  //Init
  Matrix<double, 3, 3> uncertainty;
  Matrix<double, 3, 3> predictionUncertainty;
  Matrix<double, 3, 3> Id3 = Matrix<double, 3, 3>::Identity();
  vex::distance sensor;
  bool lastOff = false;
  public:
  Distance(const int32_t& port) : sensor(vex::distance(port)){
    //Construct process noise matrix
    Q = {
      {TO_4(deltaT) / 4, TO_3(deltaT) / 2, deltaT*deltaT / 2},
      {TO_3(deltaT) / 2,  deltaT * deltaT,            deltaT},
      {deltaT*deltaT/ 2,           deltaT,                 1}
    };
    Q *= sigmaA * sigmaA;

    //Construct state update equations
    F = {
      {1, deltaT, 0.5 * deltaT * deltaT},
      {0,      1,                deltaT},
      {0,      0,                     1}
    };

    H = {{1,0,0}};
  }
  double sensorVariance(){
    if(sensor.objectDistance(mm) > 200){
      //Over 200 mm, accuracy is +/- 5%
      return sensor.objectDistance(mm) * sensor.objectDistance(mm) * 0.05 * 0.05;
    }
    else {
      return sensorSigma * sensorSigma;
    }
  }
  void initLoop(){
    prediction = estimate = {{sensor.objectDistance(mm)}, {0}, {0}};
    uncertainty = {
      {sensorVariance(), 0,   0},
      {               0, 100, 0},
      {               0, 0, 200}
    };
    predictionUncertainty = uncertainty;
    
  }
  void test(){
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
  void endFiltering(){
    lastOff = false;
  }
  double rawDist(distanceUnits d){
    return sensor.objectDistance(d);
  }
  double objectDistance(distanceUnits d){
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
  void sleep(){
    s(deltaT * 1000.0);
  }
  bool isObjectDetected(){
    return lastOff;
  }
  double systemEstimate(){
    if(!lastOff){
      return 0;
    }
    return estimate(0, 0);
  }
  bool installed(){
    return sensor.installed();
  }
};