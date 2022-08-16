#include "Odometry/GPS_Share.h"
//Makes an array of Vectors that can be efficiently modified at both ends
struct VectorArr {
  //Underlying storage type
  //Leave it public just because
  vector<PVector> arr = vector<PVector>();

  //Constructor
  VectorArr(std::initializer_list<PVector> list);

  //Default the other constructors and destructor
  VectorArr() = default;
  VectorArr(const VectorArr&) = default;
  VectorArr(VectorArr&&) = default;
  ~VectorArr() = default;

  //Use decltype because I don't care enough to find out the type

  //Iterators:
  decltype(arr.begin()) begin();
  decltype(arr.end()) end();
  //Bracket access
  PVector& operator[](int i);
  void pop(int i);
  // void popCurrentNext(){
  //   arr.popCurrentNext();
  // }
  void popBase();
  void popEnd();
  //Get the size
  size_t size();
  //Add elements
  void push(PVector v);
  void push_front(PVector v);

  //Get last / first elements
  PVector& last();
  PVector& first();
  //operator=
  VectorArr& operator=(VectorArr& v);
  //operator=
  VectorArr& operator=(VectorArr&& v);
};

//Find a single point on a bezier curve with parameter t (t goes from 0 -> 1)
PVector bezierInterpolate(VectorArr ptArr, double t);
//Create a bezier curve
VectorArr bezierCurve(VectorArr ptArr, double inc = 1.0 / 50.0);
VectorArr bezierDerivative(VectorArr ptArr, double inc = 1.0 / 50.0);
pair<VectorArr, VectorArr> bezierCurveNormalLR(VectorArr ptArr, double dist = 1.0, double inc = 1.0 / 50.0);
vector<double> bezierCurvature(VectorArr ptArr, double inc = 1.0 / 50.0);