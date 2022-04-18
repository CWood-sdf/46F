#include "EMA_Filter.h"
#include "FieldObject.h"
bool toBool(double v){
  return (bool)(int)(v + 0.5);
}
struct BasicObject {
  typedef BasicEMA<bool, double, toBool> BoolEMA;
  BasicEMA<PVector> origin = BasicEMA<PVector>(0.9);
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
    sensor.takeSnapshot(*object.color);
    safearray<vision::object, VISION_MAX_OBJECTS>& arr = sensor.objects;
    for(int i = 0; i < objects.getLength(); i++){
      vision::object* closestObj = NULL;
      PVector topLeft = objects[i].origin.value() - PVector(10, 10);
      PVector btmRight = objects[i].origin.value() + PVector(objects[i].width, objects[i].height) + PVector(10, 10);
      int objectCount = 0;
      //Need detection for if object goes out bounds, but origin is in other obj, or vice-versa
      PVector objOrigin;
      PVector objWH;
      for(int i = 0; i < arr.getLength(); i++){
        if (
          arr[i].originX > topLeft.x && 
          arr[i].originY > topLeft.y && 
          arr[i].originX + arr[i].width < btmRight.x && 
          arr[i].originY + arr[i].height < btmRight.y
        ){
          objectCount++;
        }
        
      }
      if(objectCount > 1){
        objects[i].split.update(1);
      }
      else if (objectCount == 1) {
        objects[i].split.update(0);
      }
      
      if(objectCount == 0){
        objects[i].exist.update(0);
      }
      else {
        objects[i].exist.update(1);
      }
    }
  }

};