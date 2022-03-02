#include "vex.h"
struct KillThread {
  static inline vector<thread*> instances;
  thread* t;
  
  KillThread(void(*callback)(void) ) : t(new thread(callback)) {
    instances.push_back(t);
  }
  KillThread(thread&& t) : t(&t){
    instances.push_back(this->t);
  }
  KillThread& operator=(thread&& t){
    this->t = &t;
    instances.push_back(&t);
    return *this;
  }
  static void killAll(){
    cout << "KAT " << instances.size() << endl;
    for(auto i : instances){
      i->interrupt();
      i->detach();
    }
    instances.clear();
  }
};