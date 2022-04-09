#include "vex.h"
struct CircFieldObject {
  vision::signature* color;
  double groundToMid = 0;
  vector<PVector> topLeftBound;
  vector<PVector> btmRghtBound;
  double getDistEst(double sensorHeight, double distEst, PVector pt){
    double dFront = distEst / sensorHeight * (pt.y + groundToMid);
    return distEst - dFront + pt.x;
  }
  double getCenterObjectDistEstimateTop(double topEst, double sensorHeight){
    double angle = atan(sensorHeight / topEst); // in rad
    if(topLeftBound.size() != 1){
      int i = 0;
      while(90 - topLeftBound[i].angleTo(topLeftBound[i + 1]) > angle){
        if(i == topLeftBound.size() - 1){
          break;
        }
      }
      if(i != 0 && abs(90 - topLeftBound[i].angleTo(topLeftBound[i + 1]) - angle) > abs(90 - topLeftBound[i - 1].angleTo(topLeftBound[i]) - angle)){
        //Use btm
        i--;
      }
      if(90 - topLeftBound[i].angleTo(topLeftBound[i + 1]) < angle){
        //Change to top
        i++;
      }
      auto pt = topLeftBound[i];
      return getDistEst(sensorHeight, topEst, pt);
    }
    else {
      auto pt = topLeftBound[0];
      return getDistEst(sensorHeight, topEst, pt);
    }
  }
  double getCenterObjectDistEstimateBtm(double btmEst, double sensorHeight){
    double angle = atan(sensorHeight / btmEst); // in rad
    if(btmRghtBound.size() != 1){
      int i = 0;
      while(90 - btmRghtBound[i].angleTo(btmRghtBound[i + 1]) < angle){
        if(i == btmRghtBound.size() - 1){
          break;
        }
      }
      if(i != 0 && abs(90 - btmRghtBound[i].angleTo(btmRghtBound[i + 1]) - angle) < abs(90 - btmRghtBound[i - 1].angleTo(btmRghtBound[i]) - angle)){
        //Use btm
        i--;
      }
      if(90 - btmRghtBound[i].angleTo(btmRghtBound[i + 1]) > angle){
        //Change to top
        i++;
      }
      auto pt = btmRghtBound[i];
      return getDistEst(sensorHeight, btmEst, pt);
    }
    else {
      auto pt = btmRghtBound[0];
      return getDistEst(sensorHeight, btmEst, pt);
    }
  }
  double getCenterObjectHorEstimateLeft(double est, double sensorHeight, double distEst){
    return 0;
  }
  double getCenterObjectHorEstimateRight(double est, double sensorHeight, double distEst){
    return 0;
  }
  PVector getRelPosLocation(PVector top, PVector btm){
    return {0, 0};
  }
};

