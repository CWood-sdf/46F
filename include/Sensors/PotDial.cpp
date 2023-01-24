#define NO_MAKE
#include "Sensors/PotDial.h"

void SelectorArr::addDial(PotDial* dial){
  for(auto p : boundDials){
    //Dial already contained in the array, abort
    if(p == dial){
      return ;
    }
  }
  boundDials.push_back(dial);
  if(toVal.size() > 0){
    cout << "In SelectorArr: "
          << "Adding new dial, deleting " << toVal.size() << " output item(s)" << endl;
    toVal.clear();
  }
}
SelectorArr::SelectorArr(vector<int> count, FnTp fn){
  if(count.size() != boundDials.size()){
    cout 
      << "Invalid input size, given array size (" 
      << count.size() 
      << ") does not match dial count of (" 
      << boundDials.size() 
      << "), aborting init of SelectorArr(vector<int>, FnTp)" 
      << endl;
  }
  else {
    isValid = true;
    toVal[count] = fn;
    this->count = count;
  }
}
SelectorArr::SelectorArr(vector<int> count){
  if(count.size() != boundDials.size()){
    cout 
      << "Invalid input size, given array size (" 
      << count.size() 
      << ") does not match dial count of (" 
      << boundDials.size() 
      << "), aborting init of SelectorArr(vector<int>)" 
      << endl;
  }
  else {
    isValid = true;
    toVal[count] = emptyFn;
  }
}
SelectorArr::SelectorArr() {}
void SelectorArr::attachFn(FnTp fn){
  if(isValid){
    toVal[count] = fn;
  }
}

int PotDial::getAmnt(){
  double angle = sensor->angle(deg);
  if(angle <= baseVal){
    return 1;
  } else if(angle >= baseVal + range){
    return ticks;
  } else {
    angle -= baseVal;
    angle /= (double)range;
    angle *= (double)ticks - 1;
    return round(angle) + 1;
  }
}
SelectorArr::FnTp SelectorArr::getVal(){
  vector<int> vals;
  for(auto l : boundDials){
    vals.push_back(l->getAmnt());
  }
  if(toVal.count(vals) == 1){
    return toVal[vals];
  }
  //Err msg: show inputs received by prog, show possible inputs
  cout << "In SelectorArr: ";
  cout << "Input array of {";
  for(int i = 0; i < vals.size(); i++){
    cout << vals[i];
    if(i != vals.size() - 1){
      cout << ", ";
    }
  }
  cout << "} does not match any of the declared inputs: \n";
  for(auto& [key, val] : toVal){
    cout << "  {";
    for(int i = 0; i < key.size(); i++){
    cout << key[i];
      if(i != key.size() - 1){
        cout << ", ";
      }
    }
    cout << "}\n";
  }
  cout << flush;
  return emptyFn;
}