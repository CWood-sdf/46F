#include "vex.h"
class Init {
  function<void()> init;
public:
  class Auton;
  vector<Auton*> autons;
  
};
class Init::Auton {
  friend class Init;
  static inline size_t idSet = 0;
  static inline std::vector<Auton*> refList;
  string name;
  size_t index;
  function<void()> auton;
  typedef Auton& chain_method;
  chain_method setFn(std::function<void()> a){
    auton = a;
    CHAIN;
  }
  chain_method setName(string n){
    name = n;
    CHAIN;
  }
  Auton(){
    index = idSet++;
    refList.push_back(this);
  }
public:
  Auton(string name, std::function<void()> fn) : Auton() {
    setName(name);
    setFn(fn);
  }
  chain_method call(){
    auton();
    CHAIN;
  }
  string getName(){
    return name;
  }
  static inline std::vector<Auton*> getRefs(){
    return refList;
  }
};