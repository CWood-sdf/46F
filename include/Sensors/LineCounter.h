#include "Sensors/KillThread.h"
class LineCounter {
  
  static const KillThread updater;
  friend class LineGroup;
  line* sensor;
  bool isActive = false;
  int threshold = startThreshold;
  static const int startThreshold = 15;
  //May have to increase lowThreshold to prevent 2 balls in a row from counting as one
  static const int lowThreshold = 13;
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
  //Return true if its the first time called after a hit
  bool firstHit();
  //Return true if obj near
  bool active();
  //Update the values
  void update();
  //Reset the counts
  void reset();
  //Get the number of objs that entered field of view, then left
  int getCountOut();
  //Get the number of objs that entered field of view
  int getCountIn();
  //Same as active()
  bool pressing();
  //Returns raw line tracker value
  int rawData();
};
void microWait(uint time);