#define NO_MAKE
#include "Odometry/Bezier.h"
#include "Odometry/GPS_Share.h"

//Constructor
VectorArr::VectorArr(std::initializer_list<PVector> list){
  for(PVector v : list){
    arr.push_back(v);
  }
}
//Use decltype because I don't care enough to find out the type

//Iterators:
decltype(VectorArr::arr.begin()) VectorArr::begin(){
  return arr.begin();
}
decltype(VectorArr::arr.end()) VectorArr::end(){
  return arr.end();
}
//Bracket access
PVector& VectorArr::operator[](int i){
  return arr[i];
}
void VectorArr::pop(int i){
  arr.erase(arr.cbegin() + i, arr.cbegin()+ i+1);
}
// void popCurrentNext(){
//   arr.popCurrentNext();
// }
void VectorArr::popBase(){
  arr.erase(arr.cbegin(), arr.cbegin() + 1);
}
void VectorArr::popEnd(){
  arr.erase(arr.cbegin() + arr.size() - 1, arr.cbegin() + arr.size());
}
//Get the size
size_t VectorArr::size(){
  return arr.size();
}
//Add elements
void VectorArr::push(PVector v){
  arr.push_back(v);
}
void VectorArr::push_front(PVector v){
  arr.insert(arr.cbegin(), v);
}

//Get last / first elements
PVector& VectorArr::last(){
  return arr.back();
}
PVector& VectorArr::first(){
  return arr.front();
}

//operator=
VectorArr& VectorArr::operator=(VectorArr& v){
  
  arr = v.arr;
  return *this;
}
//operator=
VectorArr& VectorArr::operator=(VectorArr&& v){
  arr = v.arr;
  return *this;
}
//Find a single point on a bezier curve with parameter t (t goes from 0 -> 1)
PVector bezierInterpolate(VectorArr ptArr, double t){
  //The array of the interpolated points
  VectorArr newPts = {};
  for(int i = 0; i < ptArr.size() - 1; i++){
    //Interpolate between current point and next
    PVector newPt = ptArr[i + 1] - ptArr[i];
    newPt.mult(t);
    newPt += ptArr[i];
    newPts.push(newPt);
  }
  //If interpolated point array still has multiple elements
  if(newPts.size() >= 2){
    //YAYYY RECURSION!!!!
    return bezierInterpolate(newPts, t);
  } else {
    //Otherwise return the only element
    return newPts.first();
  }
}
//Create a bezier curve
VectorArr bezierCurve(VectorArr ptArr, double inc){
  double spacing = 1.0;
  //Return value
  VectorArr ret;
  //Go through multiple rounds of interpolation
  for(double i = 0; i <= 1; i += inc){
    PVector pt = bezierInterpolate(ptArr, i);
    ret.push(pt);
  }
  ret.push(ptArr.last());
  return ret;
}
VectorArr bezierDerivative(VectorArr ptArr, double inc){
  VectorArr bezier = bezierCurve(ptArr, inc);
  VectorArr derivative;
  for(int i = 0; i < bezier.size() - 1; i++){
    derivative.push(bezier[i + 1] - bezier[i]);
  }
  derivative.push(derivative.last());
  return derivative;
}
VectorArr bezierAcc(VectorArr ptArr, double inc){
  VectorArr derivative = bezierDerivative(ptArr, inc);
  VectorArr acc;
  for(int i = 0; i < derivative.size() - 1; i++){
    acc.push(derivative[i + 1] - derivative[i]);
  }
  acc.push(acc.last());
  return acc;
}
pair<VectorArr, VectorArr> bezierCurveNormalLR(VectorArr ptArr, double dist, double inc){
  //Return value
  pair<VectorArr, VectorArr> ret;
  VectorArr bezier = bezierCurve(ptArr, inc);
  
  VectorArr derivative = bezierDerivative(ptArr, inc);
  for(auto& i : derivative){
    i.normalize();
  }
  for(int i = 0; i < derivative.size(); i++){
    ret.first.push(derivative[i].get().rotate(-90).mult(dist).add(bezier[i]));
    ret.second.push(derivative[i].get().rotate(90).mult(dist).add(bezier[i]));
  }
  return ret;
}
vector<double> bezierCurvature(VectorArr ptArr, double inc){
  vector<double> ret;
  VectorArr derivative = bezierDerivative(ptArr, inc);
  VectorArr acc = bezierAcc(ptArr, inc);
  for(int i = 0; i < derivative.size(); i++){
    Matrix<double, 2, 2> combo;
    combo[0][0] = derivative[i].x;
    combo[0][1] = derivative[i].y;
    combo[1][0] = acc[i].x;
    combo[1][1] = acc[i].y;
    double denominator = pow(derivative[i].mag(), 3);
    if(denominator != 0)
      ret.push_back(combo.determinant() / denominator);
    else
      ret.push_back(1e99);
  }
  return ret;
}