#include "EMA_Filter.h"
#include "FieldObject.h"
struct BasicObject {
  BasicEMA<PVector> origin = BasicEMA<PVector>(0.9);
  EMA width = EMA(0.95); // Give 95% trust to old measurements in case of object splitting
  EMA height = EMA(0.95);
  void addData(vision::object& object){
    origin.update({static_cast<double>(object.originX), static_cast<double>(object.originY)});
    width.update(object.width);
    height.update(object.height);
  }
};
struct ObjectMap {
  CircFieldObject object;
  vision& sensor;
  safearray<BasicObject, VISION_MAX_OBJECTS> objects;
  //Merge two objects if their detections are very close and the sizes are similar
  //Will require some pointer hacks to get around the const& types 
  void mergeClose(){
    
  }
  void update(){
    sensor.takeSnapshot(*object.color);
    safearray<vision::object, VISION_MAX_OBJECTS>& arr = sensor.objects;
    for(int i = 0; i < objects.getLength(); i++){
      vision::object* closestObj = NULL;
    }
  }

};