#include "EMA_Filter.h"
#include "FieldObject.h"

struct ObjectMap {
  CircFieldObject searchObject;
  safearray<vision::object, VISION_MAX_OBJECTS> objects;
  safearray<vision::object, VISION_MAX_OBJECTS> lastObjects;
  //Merge two objects if their detections are very close and the sizes are similar
  //Will require some pointer hacks to get around the const& types 
  void mergeClose(){
    
  }
  void update();

};