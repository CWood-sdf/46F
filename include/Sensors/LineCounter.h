#include "Sensors/KillThread.h"
class LineCounter {
  
  static const KillThread updater;
  friend class LineGroup;
  line* sensor;
  bool isActive = false;
  int threshold = startThreshold;
  static const int startThreshold = 15;
  //May have to increase lowThreshold to prevent 2 balls in a row from counting as one
  static const int lowThreshold = 10;
  bool wasActiveLast = false;
  unsigned int countOut = 0;
  unsigned int countIn = 0;
  bool fHit = false;
public:
  static vector<LineCounter*> instances;
  LineCounter(line& se) : sensor(&se){
    ADD_THIS;
  }
  LineCounter(LineCounter& counter) : sensor(counter.sensor){
    isActive = counter.isActive;
    threshold = counter.threshold;
    ADD_THIS;
  }
  
  LineCounter(triport::port& p) : sensor(new line(p)) {
    ADD_THIS;
  }
  bool firstHit(){
    if(fHit){
      fHit = false;
      return true;
    }
    return false;
  }
  bool active(){
    return isActive;
  }
  void update(bool out = false){
    if(out)
    cout << sensor->reflectivity() << endl;
    
    bool isActive = pressing();
    if(wasActiveLast){
      if(!isActive){
        countOut++;
      }
    } else {
      if(isActive){
        countIn++;
        fHit = true;
      }
    }
    wasActiveLast = isActive;
  }
  void reset(){
    countIn = 0;
    countOut = 0;
    if(active()){
      countIn = 1;
      countOut = 0;
    }
  }
  int getCountOut(){
    return countOut;
  }
  int getCountIn(){
    return countIn;
  }
  bool pressing(){
    
    isActive = threshold <= sensor->reflectivity();
    if(isActive){
      threshold = lowThreshold;
    } else  {
      threshold = startThreshold;
    }
    return isActive;
  }
};
void microWait(uint time);
vector<LineCounter*> LineCounter::instances = {};
const KillThread LineCounter::updater = thread([](){
  if(LineCounter::instances.size() == 0){
    cout << "No Line Counter instances exist, exiting thread" << endl;\
    return;
  }
  while(1){
    for(auto t : LineCounter::instances){
      t->update();
    }
    microWait(2000);
  }
});