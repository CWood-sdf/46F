#include "vex.h"
namespace bos {
  class BosFn {
    std::function<bool(bool)> popFn;
    std::function<void(bool)> mainFn;
    bool isLvgl = false;
    bool isPop = false;
  public:
    BosFn(){
      
    }
    BosFn(std::function<bool(bool)> p, bool isLv = false){
      isLvgl = isLv;
      popFn = p;
      isPop = true;
    }
    BosFn(std::function<void(bool)> m, bool isLv = false){
      isLvgl = isLv;
      mainFn = m;
      isPop = false;
    }
    BosFn(bool(*p)(bool)){
      popFn = p;
      isPop = true;
    }
    BosFn(void(*m)(bool)){
      mainFn = m;
      isPop = false;
    }
    bool lvgl(){
      return isLvgl;
    }
    bool call(bool remake = false){
      if(isPop){
        return popFn(remake);
      }
      else {
        mainFn(remake);
      }
      return false;
    }
  };
}