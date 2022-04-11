#include "EMA_Filter.h"
struct VisObject : public vision::object {

};
struct Map {
  safearray<VisObject, VISION_MAX_OBJECTS> objects;
  safearray<VisObject, VISION_MAX_OBJECTS> lastObjects;
  //Merge two objects if their detections are very close and the sizes are similar
  void mergeClose(){
    
  }
  
};