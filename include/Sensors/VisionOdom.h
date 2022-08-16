#include "Odometry/GPS_Share.h"
#include "Sensors/Vision/FieldObject.h"
#include "Sensors/Vision/ObjectMap.h"
class Angle {
public:
  enum class Type {
    Deg,
    Rad,
    Rev
  };
private:
  double value;//In rad
  Type type;
  static map<Type, double> toRad;
  static void init(){
    static bool done = false;
    if(!done){
      done = true;
      toRad[Type::Rad] = 1;
      toRad[Type::Deg] = M_PI / 180;
      toRad[Type::Rev] = 2.0 * M_PI;
    }
  }
public:
  Angle(double v, Type t){
    from(v, t);
  }
  Angle(double v){
    from(v, Type::Rad);
  }
  double to(Type t){
    if(t == Type::Rad){
      return value * toRad[type];
    }
    return value * toRad[type] / toRad[t];
  }
  Angle& changeType(Type t){
    value = to(t);
    type = t;
    return *this;
  }
  Angle& from(double v, Type t){
    value = v;
    type = t;
    return *this;
  }
  operator double(){
    return to(Type::Rad);
  }
};
Angle operator "" _deg(long double);
Angle operator "" _rev(long double);
Angle operator "" _rad(long double);
extern GPS_Share share;
class VisionOdom {
protected:
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
  PVector estimateSensorRelativePos(int pixelLeftX, int pixelBottomY, int pixelRightX, int pixelTopY, PVector bottomPoint, PVector topPoint, PVector fixedPoint) {

    if (pixelLeftX == 0 || pixelTopY == 0 || pixelBottomY >= screenHeight || pixelRightX >= screenWidth) {
      return PVector(1e10, 1e10);
    }
    
    //The angle to the top and bottom points
    Angle angleYBottom = getAngleY(pixelBottomY) - mountAngle;
    Angle angleYTop = getAngleY(pixelTopY) - mountAngle;
    Angle angleXBottom = getAngleX(pixelLeftX);
    Angle angleXTop = getAngleX(pixelRightX);
    Angle angleXAvg = (angleXBottom + angleXTop) / 2.0 - share.heading() - mountRotation;

    double topDiff = topPoint.z - mountHeight;
    double bottomDiff = bottomPoint.z - mountHeight;

    double distFromBottom = bottomDiff / tan(angleYBottom);
    double distFromTop = topDiff / tan(angleYTop);
    bottomPoint = PVector(0, bottomPoint.y).rotateXY(-angleXAvg);
    topPoint = PVector(0, topPoint.y).rotateXY(-angleXAvg);
    PVector bottomEst = PVector(0, distFromBottom).rotateXY(-angleXAvg);
    bottomEst.add(bottomPoint);
    PVector topEst = PVector(0, distFromTop).rotateXY(-angleXAvg);
    topEst.add(topPoint);
    PVector ret = (bottomEst + topEst) / 2.0;
    ret.z += mountHeight;
    fixedPoint.z = 0;
    ret.add(fixedPoint);
    return ret;
  }
  PVector estimateSensorRelativePos(vision::signature* sig, PVector bottomPoint, PVector topPoint, PVector fixedPoint) {
    sensor->takeSnapshot(*sig);
    auto& largestObj = sensor->largestObject;
    if (!largestObj.exists) {
      //Return an absolutely large estimate if no object is found
      return { 1e10, 1e10 };
    }
    return estimateSensorRelativePos(
      largestObj.originX, 
      largestObj.originY + largestObj.height, 
      largestObj.originX + largestObj.width, 
      largestObj.originY, 
      bottomPoint, 
      topPoint, 
      fixedPoint);
  }
  PVector estimateRelativePos(int pixelLeft, int pixelRight, double radius, double objHeight){
    Angle angleXLeft = getAngleX(pixelLeft);
    Angle angleXRight = getAngleX(pixelRight);
    //The internal angle is 0.5 times the difference
    Angle internalAngle = (angleXLeft - angleXRight) / 2.0;
    //The local offset is the avg
    Angle localOffset = (angleXLeft + angleXRight) / 2.0;
    //The distance to the object is the radius * the csc of internalAngle
    //The distance between the camera and the center point
    double dist = radius / sin(internalAngle);

    double heightDiff = mountHeight - objHeight;
    //The angle to rotateYZ down by is the 90 - acos(heightDiff / dist)
    Angle angleYZ = 90.0_deg - Angle(acos(heightDiff / dist), Angle::Type::Rad);
    
    PVector ret = PVector(0, dist, 0);
    ret.rotateYZ(-angleYZ);
    ret.rotateXY(localOffset);
    ret.z += mountHeight;
    //ret.pt.z = 0;
    return ret;
  }
  Angle getAngleY(int pixelY){
    return Angle(-atan(-(screenHeight / 2.0 - pixelY) / backDist), Angle::Type::Rad);
  }
  Angle getAngleX(int pixelX){
    return Angle( atan(-(screenHeight / 2.0 - pixelX) / backDist), Angle::Type::Rad);
  }
public:
  
  VisionOdom(vision& sensor, PVector relPos, double height, double mAngle, double mRotation){
    this->relPos = relPos;
    this->sensor = &sensor;
    mountHeight = height;
    mountAngle = mAngle * DEG_TO_RAD;
    mountRotation = mRotation;
  }
  // template<class Sig>
  // PVector estimatePos(Sig& sig){
  //   sensor->takeSnapshot(sig);
  //   return estimatePos(sensor->largestObject);
  // }
  // template<class Sig>
  // vector<PVector> estimateAllPos(Sig& sig){
  //   sensor->takeSnapshot(sig);
  //   vector<PVector> ret;
  //   for(int i = 0; i < sensor->objects.getLength(); i++){
  //     if(sensor->objects[i].exists){
  //       ret.push_back(estimatePos(sensor->objects[i]));
  //     }
  //   }
  //   return ret;
  // }
  // template<class Sig>
  // PVector closest(Sig& sig){
  //   sensor->takeSnapshot(sig);
  //   vector<PVector> arr;
  //   for(int i = 0; i < sensor->objects.getLength(); i++){
  //     if(sensor->objects[i].exists){
  //       //auto pos = estimatePos(sensor->objects[i]);
  //       arr.push_back(estimatePos(sensor->objects[i]));
  //     }
  //   }
  //   PVector c;
  //   double dist = 100000;
  //   for(auto i : arr){
  //     if(i.dist2D(share.position()) < dist){
  //       dist = i.dist2D(share.position());
  //       c = i;
  //     }
  //   }
  //   return c;
  // }
  // PVector getEstimatePos(vision::object& object, CircFieldObject& obj){
  //   auto topLeft = estimateRelPos(object.originX, object.originY);
  //   auto btmRight = estimateRelPos(object.originX + object.width, object.originY + object.height);
  //   PVector ret = Refine::getRelPosLocation(topLeft.first, btmRight.first, mountHeight, topLeft.second, btmRight.second, obj);
  //   return ret;
  // }
  safearray<PVector, VISION_MAX_OBJECTS> fromArr(safearray<vision::object, VISION_MAX_OBJECTS>& arr);
  safearray<PVector, VISION_MAX_OBJECTS> fromMap(ObjectMap& objects);
};

class VisionBasicOdometry : private VisionOdom, public BasicConfirmationOdom {
  PVector currentEstimate;
  vector<tuple<vision::signature*, PVector, PVector, PVector>> fixedPoints;
  PVector posOffCenter = PVector(0, 0, 0);
  int currentQuality = 0;
  double variance() override {
    return 16.0;
  }
  void updateEstimate() override {
    //Update the current estimate from the fixed points, if the estimate is outside the 12' by 12' field, set quality = 0
    for (auto& tuple : fixedPoints) {
      auto& sig = get<0>(tuple);
      auto& bottomPoint = get<1>(tuple);
      auto& topPoint = get<2>(tuple);
      auto& fixedPoint = get<3>(tuple);
      PVector estimate = estimateSensorRelativePos(sig, bottomPoint, topPoint, fixedPoint);
      currentEstimate = estimate;
      PVector posOffCopy = posOffCenter;
      posOffCopy.rotateXY(-share.heading());
      currentEstimate += posOffCopy;
      if (currentEstimate.dist2D({0, 0}) >= 12.0 * 6.0) {
        currentQuality = 0;
      }
      else {
        currentQuality = 100;
        break;
      }
    }
  }
  double xPosition(distanceUnits) override {
    return currentEstimate.x;
  }
  double yPosition(distanceUnits) override {
    return currentEstimate.y;
  }
  int32_t quality() override {
    return currentQuality;
  }
  bool installed() override {
    return sensor->installed();
  }
};