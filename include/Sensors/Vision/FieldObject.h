#include "vex.h"
struct CircFieldObject {
  vision::signature* color;
  vector<PVector> topLeftBound;
  vector<PVector> btmRghtBound;
  PVector getEst(double sensorHeight, double distEst, PVector pt){
    double dFront = distEst / sensorHeight * pt.y;
    return {0, 0};//
  }
  PVector getCenterObjectEstimateTop(double topEst, double sensorHeight){
    double angle = atan(sensorHeight / topEst); // in rad
    if(topLeftBound.size() == 1){

    }
    else {

    }
    return {0, 0};
  }
  PVector getCenterObjectEstimateBtm(double btmEst, double sensorHeigh){
    return {0, 0};
  }
};

