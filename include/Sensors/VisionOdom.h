#include "Odometry/GPS_Share.h"
#include "Sensors/Vision/FieldObject.h"
#include "Sensors/Vision/ObjectMap.h"

extern GPS_Share share;
class VisionOdom {
  vision* sensor;
  double mountHeight, mountAngle, mountRotation;
  struct Refine {
  private:
    static double getDistEst(double sensorHeight, double distEst, PVector pt, CircFieldObject& obj){
      auto& groundToMid = obj.groundToMid;
      double dFront = distEst / sensorHeight * (pt.y + groundToMid);
      return distEst - dFront + pt.x;
    }
    static double getCenterObjectVertEstimateTop(double topEst, double sensorHeight, CircFieldObject& obj){
      auto& topLeftBound = obj.topLeftBound;

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
        return getDistEst(sensorHeight, topEst, pt, obj);
      }
      else {
        auto pt = topLeftBound[0];
        return getDistEst(sensorHeight, topEst, pt, obj);
      }
    }
    static double getCenterObjectVertEstimateBtm(double btmEst, double sensorHeight, CircFieldObject& obj){
      auto& btmRghtBound = obj.btmRghtBound;

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
        return getDistEst(sensorHeight, btmEst, pt, obj);
      }
      else {
        auto pt = btmRghtBound[0];
        return getDistEst(sensorHeight, btmEst, pt, obj);
      }
    }
    static PVector getCenterObjectHorEstimateLeft(double est, double sensorHeight, double distEst, double angleFromSensor, CircFieldObject& obj){
      auto& topLeftBound = obj.topLeftBound;
      auto& groundToMid = obj.groundToMid;
      PVector farthestPt = {0, 0};
      for(auto i : topLeftBound){
        if(abs(i.x) > abs(farthestPt.x)){
          farthestPt = i;
        }
      }
      farthestPt.rotateXZ(-angleFromSensor);
      double dist = PVector(est, distEst).mag();
      double hypotenuse = PVector(dist, sensorHeight).mag();
      double dFront = dist / sensorHeight * (farthestPt.y + groundToMid);
      PVector endPt = {est, distEst};
      PVector linePt = PVector(dFront, 0).rotate(endPt.heading2D() + 180);
      endPt += linePt;
      PVector posEst = endPt + PVector(farthestPt.x, farthestPt.z);
      return posEst;
    }
    static PVector getCenterObjectHorEstimateRight(double est, double sensorHeight, double distEst, double angleFromSensor, CircFieldObject& obj){
      auto& btmRghtBound = obj.btmRghtBound;
      auto& groundToMid = obj.groundToMid;
      PVector farthestPt = {0, 0};
      for(auto i : btmRghtBound){
        if(abs(i.x) > abs(farthestPt.x)){
          farthestPt = i;
        }
      }
      farthestPt.rotateXZ(angleFromSensor);
      double dist = PVector(est, distEst).mag();
      double hypotenuse = PVector(dist, sensorHeight).mag();
      double dFront = dist / sensorHeight * (farthestPt.y + groundToMid);
      PVector endPt = {est, distEst};
      PVector linePt = PVector(dFront, 0).rotate(endPt.heading2D() + 180);
      endPt += linePt;
      PVector posEst = endPt + PVector(farthestPt.x, farthestPt.z);
      return posEst;
    }
  public:
    static PVector getRelPosLocation(PVector topLeft, PVector btmRight, double sensorHeight, double leftAngle, double rightAngle, CircFieldObject& obj){
      double yEstTop = getCenterObjectVertEstimateTop(topLeft.y, sensorHeight, obj);
      double yEstBtm = getCenterObjectVertEstimateBtm(btmRight.y, sensorHeight, obj);
      double yEst = (yEstBtm + yEstTop) / 2.0;
      PVector leftEst = getCenterObjectHorEstimateLeft(topLeft.x, sensorHeight, yEst, leftAngle, obj);
      PVector rightEst = getCenterObjectHorEstimateRight(btmRight.x, sensorHeight, yEst, rightAngle, obj);
      PVector estTotal = PVector(
        (leftEst.x + rightEst.x) / 2.0, 
        (leftEst.y + rightEst.y + yEst) / 3.0 // May want to weight yEst: 
                                              //    ... + yEst * 2.0) / 4.0
      );
      return estTotal;
    }
  };
  PVector relPos;
  // static inline BasicWheelController* wc;
  static constexpr double 
    screenWidth = 310,
    screenHeight = 210,
    widthAngle = 56 * DEG_TO_RAD,
    heightAngle = 46 * DEG_TO_RAD, 
    backDist = 0.5 * screenWidth / _tan(widthAngle / 2.0);
  pair<PVector, double> estimateRelPos(int pixelX, int pixelY){
    // cout << object.centerX << ", " << object.centerY << endl;
    PVector ret;
    // cout << (screenHeight / 2.0 - object.centerY) << endl;
    double angleY = atan(-(screenHeight / 2.0 - pixelY) / backDist);
    double angleX = atan((screenWidth / 2.0 - pixelX) / backDist);
    // cout << angleY * RAD_TO_DEG << ", " << angleX * RAD_TO_DEG << endl;
    double internalAngleY = M_PI_2 - mountAngle - angleY;
    // cout << internalAngleY * RAD_TO_DEG << endl;
    double dy = tan(internalAngleY) * mountHeight;
    double h2 = sqrt(dy*dy + mountHeight * mountHeight);
    double dx = h2 * tan(angleX);
    ret = PVector(dx, dy);
    
    return {ret, angleX};
  }
  PVector estimatePos(vision::object& object){
    PVector ret = estimateRelPos(object.originX, object.originY).first;
    PVector rCopy = relPos;
    ret
      .rotate(mountRotation + share.heading())
      .add(share.position())
      .add(rCopy.rotate(share.heading()));
    return ret;
  }
  PVector estimateRelTopMid(vision::object& object){
    
    PVector ret = estimateRelPos(object.centerX, object.originY).first;
    return ret;
  }
  PVector estimateRelBtmMid(vision::object& object){
    PVector ret = estimateRelPos(object.centerX, object.originY + object.height).first;
    return ret;
  }
public:
  
  VisionOdom(vision& sensor, PVector relPos, double height, double mAngle, double mRotation){
    this->relPos = relPos;
    this->sensor = &sensor;
    mountHeight = height;
    mountAngle = mAngle * DEG_TO_RAD;
    mountRotation = mRotation;
  }
  template<class Sig>
  PVector estimatePos(Sig& sig){
    sensor->takeSnapshot(sig);
    return estimatePos(sensor->largestObject);
  }
  template<class Sig>
  vector<PVector> estimateAllPos(Sig& sig){
    sensor->takeSnapshot(sig);
    vector<PVector> ret;
    for(int i = 0; i < sensor->objects.getLength(); i++){
      if(sensor->objects[i].exists){
        ret.push_back(estimatePos(sensor->objects[i]));
      }
    }
    return ret;
  }
  template<class Sig>
  PVector closest(Sig& sig){
    sensor->takeSnapshot(sig);
    vector<PVector> arr;
    for(int i = 0; i < sensor->objects.getLength(); i++){
      if(sensor->objects[i].exists){
        auto pos = estimatePos(sensor->objects[i]);
        arr.push_back(estimatePos(sensor->objects[i]));
      }
    }
    PVector c;
    double dist = 100000;
    for(auto i : arr){
      if(i.dist2D(share.position()) < dist){
        dist = i.dist2D(share.position());
        c = i;
      }
    }
    return c;
  }
  PVector getEstimatePos(vision::object& object, CircFieldObject& obj){
    auto topLeft = estimateRelPos(object.originX, object.originY);
    auto btmRight = estimateRelPos(object.originX + object.width, object.originY + object.height);
    PVector ret = Refine::getRelPosLocation(topLeft.first, btmRight.first, mountHeight, topLeft.second, btmRight.second, obj);
    return ret;
  }
  safearray<PVector, VISION_MAX_OBJECTS> fromArr(safearray<vision::object, VISION_MAX_OBJECTS>& arr);
  safearray<PVector, VISION_MAX_OBJECTS> fromMap(ObjectMap& objects);
};