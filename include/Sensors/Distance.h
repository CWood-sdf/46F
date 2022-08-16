#ifndef DISTANCE_H
#define DISTANCE_H
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
  //Get the variance in sensor measurement
  double sensorVariance();
  //Restart the testing
  void initLoop();
  //Run a test
  void test();
  //Ends the filtering
  void endFiltering();
  //The distance sensor value
  double rawDist(distanceUnits d);
  //What the filter thinks the value is
  double objectDistance(distanceUnits d);
  //Sleep
  void sleep();
  //Is there an object?
  bool isObjectDetected();
  //ok
  double systemEstimate();
  //Sensor installed?
  bool installed();
};
#endif