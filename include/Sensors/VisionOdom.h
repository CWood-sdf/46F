#include "Odometry/GPS_Share.h"
#include "Sensors/Vision/FieldObject.h"
#include "Sensors/Vision/ObjectMap.h"

extern GPS_Share share;
class VisionOdom {
  vision* sensor;
  double mountHeight, mountAngle, mountRotation;
  struct Refine {
  private:
    static double getDistEst(double sensorHeight, double distEst, PVector& pt, CircFieldObject& obj){
      //Pt is in the object plane
      //The half-height of the object
      auto& groundToMid = obj.groundToMid;
      /*
                    -|
             -       |
      -___|__________|
      distEst = long btm line
      dFront = short front line
      pt.y + groundToMid = left vert line
      sensorHeight = right vert line
      */
      double dFront = distEst / sensorHeight * (pt.y + groundToMid);
      //pt.x is neg so adding is actually subtracting
      //distEst - dFront gets to the object sight point
      //then subtracting pt.x gets to the center
      return distEst - dFront + pt.x;
    }
    static double getCenterObjectVertEstimateTop(double topEst, double sensorHeight, CircFieldObject& obj){
      //Get the top left bounding box for the object
      auto& topLeftBound = obj.topLeftBound;

      //The angle of the object
      //From the bottom
      double angle = atan(sensorHeight / topEst); // in rad
      if(topLeftBound.size() != 1){
        int i = 0;
        //Find the closest point that matches angle
        while(90 - topLeftBound[i].angleTo(topLeftBound[i + 1]) > angle){
          if(i == topLeftBound.size() - 1){
            break;
          }
          i++;
        }
        if(i != 0 && abs(90 - topLeftBound[i].angleTo(topLeftBound[i + 1]) - angle) > abs(90 - topLeftBound[i - 1].angleTo(topLeftBound[i]) - angle)){
          //Use the bottom point because it is the one the sensor is seeing
          i--;
        }
        //Nevermind, we're going to change it to the top if we should
        if(90 - topLeftBound[i].angleTo(topLeftBound[i + 1]) < angle){
          //Change to top
          i++;
        }
        //Store the point
        auto& pt = topLeftBound[i];
        return getDistEst(sensorHeight, topEst, pt, obj);
      }
      else {
        //Only one point to refine from
        //The first block should be robust enough to allow this code to run as well, 
        //But whatevs
        auto pt = topLeftBound[0];
        return getDistEst(sensorHeight, topEst, pt, obj);
      }
    }
    //See above function for math reference
    static double getCenterObjectVertEstimateBtm(double btmEst, double sensorHeight, CircFieldObject& obj){
      //From the bottom this time
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
    //Refine the left and right estimation
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
      //double hypotenuse = PVector(dist, sensorHeight).mag();
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
      //double hypotenuse = PVector(dist, sensorHeight).mag();
      double dFront = dist / sensorHeight * (farthestPt.y + groundToMid);
      PVector endPt = {est, distEst};
      PVector linePt = PVector(dFront, 0).rotate(endPt.heading2D() + 180);
      endPt += linePt;
      PVector posEst = endPt + PVector(farthestPt.x, farthestPt.z);
      return posEst;
    }
  public:
    //Refines the position measurements
    static PVector getRelPosLocation(PVector topLeft, PVector btmRight, double sensorHeight, double leftAngle, double rightAngle, CircFieldObject& obj){
      //Get the y-estimate from the top
      double yEstTop = getCenterObjectVertEstimateTop(topLeft.y, sensorHeight, obj);
      //Get the y-estimate from the bottom
      double yEstBtm = getCenterObjectVertEstimateBtm(btmRight.y, sensorHeight, obj);
      //^^Both those variables should be the same

      //The actual y-estimate
      double yEst = (yEstBtm + yEstTop) / 2.0;
      //These both estimate points
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

  //First estimation of the position
  pair<PVector, double> estimateRelPos(int pixelX, int pixelY){
    PVector ret;

    //The angle between the point and the vision screen
    double angleY = atan(-(screenHeight / 2.0 - pixelY) / backDist);
    double angleX = atan((screenWidth / 2.0 - pixelX) / backDist);

    //The actual y-angle
    double internalAngleY = M_PI_2 - mountAngle - angleY;

    //The distance between sensor and point
    double dy = tan(internalAngleY) * mountHeight;
    //The height of the object
    double h2 = sqrt(dy*dy + mountHeight * mountHeight);
    //The distance in x
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
        //auto pos = estimatePos(sensor->objects[i]);
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