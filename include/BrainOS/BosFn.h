#include "vex.h"
namespace bos {
  class BosFn {
    std::function<bool()> popFn;
    std::function<void()> mainFn;
    bool isPop = false;
  public:
    BosFn(){
      
    }
    BosFn(std::function<bool()> p){
      popFn = p;
      isPop = true;
    }
    BosFn(std::function<void()> m){
      mainFn = m;
      isPop = false;
    }
    BosFn(bool(*p)()){
      popFn = p;
      isPop = true;
    }
    BosFn(void(*m)()){
      mainFn = m;
      isPop = false;
    }
    bool call(){
      if(isPop){
        return popFn();
      }
      else {
        mainFn();
      }
      return false;
    }
  };
}