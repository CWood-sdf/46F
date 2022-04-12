#include "Odometry/GPS_Share.h"
#include "Sensors/Vision/FieldObject.h"
#include "Sensors/Vision/ObjectMap.h"

extern GPS_Share share;
class VisionOdom {
  vision* sensor;
  double mountHeight, mountAngle, mountRotation;
  PVector relPos;
  // static inline BasicWheelController* wc;
  static constexpr double 
    screenWidth = 310,
    screenHeight = 210,
    widthAngle = 56 * DEG_TO_RAD,
    heightAngle = 46 * DEG_TO_RAD, 
    backDist = 0.5 * screenWidth / _tan(widthAngle / 2.0);
  PVector estimateRelPos(int pixelX, int pixelY){
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
    
    return ret;
  }
  PVector estimatePos(vision::object& object){
    
    // cout << object.centerX << ", " << object.centerY << endl;
    PVector ret = estimateRelPos(object.centerX, object.centerY);
    PVector rCopy = relPos;
    ret
      .rotate(mountRotation + share.heading())
      .add(share.position())
      .add(rCopy.rotate(share.heading()));
    return ret;
  }
  PVector estimateRelTopMid(vision::object& object){
    
    PVector ret = estimateRelPos(object.centerX, object.originY);
    return ret;
  }
  PVector estimateRelBtmMid(vision::object& object){
    PVector ret = estimateRelPos(object.centerX, object.originY + object.height);
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
  safearray<PVector, VISION_MAX_OBJECTS> fromArr(safearray<vision::object, VISION_MAX_OBJECTS>& arr);
  safearray<PVector, VISION_MAX_OBJECTS> fromMap(ObjectMap& objects);
};