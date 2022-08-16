#include "vex.h"
struct Pose{
  PVector pos = PVector();
  double angle = 0.0;
public:
  Pose(){
    pos = PVector();
    angle = 0.0;
  }
  Pose(PVector& v, double& a){
    pos = v;
    angle = a;
  }
  Pose(PVector&& v, double&& a){
    pos = v;
    angle = a;
  }
  Pose& set(PVector v, double a){
    pos = v;
    angle = a;
    CHAIN
  }
  bool operator!=(Pose& f){
    return f.pos != pos || f.angle != angle;
  }
  bool operator==(Pose& f){
    return f.pos == pos && f.angle == angle;
  }
  bool en = false;
  Pose& operator+= (Pose& f){
    if(en){
      cout << pos << "\n" << f.pos << endl;
    }
    this->pos += f.pos;
    if(en){
      cout << pos << "\n" << f.pos << endl << endl;
    }
    this->angle += f.angle;
    CHAIN
  }
  Pose operator-(Pose& p){
    return Pose(pos - p.pos, angle - p.angle);
  }
  Pose operator-(){
    return Pose(-pos, angle + 180);
  }
};
#ifndef NO_MAKE
ostream& operator<< (ostream& cout, Pose& v){
  cout << VECT_OUT(v.pos) << ", " << v.angle;
  return cout;
}
ostream& operator<<(ostream& cout, Pose&& v){
  return operator<<(cout, v);
}

#else 
ostream& operator<< (ostream& cout, Pose& v);
ostream& operator<<(ostream& cout, Pose&& v);

#endif