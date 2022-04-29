#include "EMA_Filter.h"
#include "FieldObject.h"
bool toBool(double v){
  return (bool)(int)(v + 0.5);
}
struct BasicObject {
  typedef BasicEMA<bool, double, toBool> BoolEMA;
  BasicEMA<PVector> origin = BasicEMA<PVector>(0.9);
  double& originX = origin.valPtr()->x;
  double& originY = origin.valPtr()->y;
  EMA width = EMA(0.95); // Give 95% trust to old measurements in case of object splitting
  EMA height = EMA(0.95);
  BoolEMA exist = BoolEMA(0.7); // Object should delete self pretty quickly
  BoolEMA split = BoolEMA(0.7);
  void addData(vision::object& object){
    origin.update({static_cast<double>(object.originX), static_cast<double>(object.originY)});
    width.update(object.width);
    height.update(object.height);
  }
  bool contains(vision::object& object){
    return false;
  }
};
struct ObjectMap {
  CircFieldObject object;
  vision& sensor;
  safearray<BasicObject, VISION_MAX_OBJECTS> objects;
  void update(){
    //Need non-existent object popping (prolly build new safearray)
    sensor.takeSnapshot(*object.color);
    safearray<vision::object, VISION_MAX_OBJECTS>& arr = sensor.objects;
    for(int i = 0; i < objects.getLength(); i++){
      vision::object* closestObj = NULL;
      PVector topLeft = objects[i].origin.value() - PVector(10, 10);
      PVector btmRight = objects[i].origin.value() + PVector(objects[i].width, objects[i].height) + PVector(10, 10);
      
      PVector objOrigin;
      PVector objWH;
      double shortestDist = 400.0; 
      for(int i = 0; i < arr.getLength(); i++){
        PVector origin = PVector(arr[i].originX, arr[i].originY);
        double oldShortest = shortestDist;
        if(origin.dist2D(topLeft) < shortestDist + 3){
          shortestDist = origin.dist2D(topLeft);
        }
        if (
          abs(shortestDist - oldShortest) < 5 &&
          arr[i].originX > topLeft.x && 
          arr[i].originY > topLeft.y && 
          arr[i].originX + arr[i].width < btmRight.x && 
          arr[i].originY + arr[i].height < btmRight.y
        ){
          objOrigin = origin;
          objWH = PVector(arr[i].width, arr[i].height);
        }
        else {
          shortestDist = oldShortest;
        }
      }
      if(shortestDist > 10){
        objects[i].exist.update(false);
        objects[i].origin.update(objOrigin);
        objects[i].height.update(objWH.y);
        objects[i].width.update(objWH.x);
        
      }
      else {
        objects[i].exist.update(true);
      }
    }
  }

};