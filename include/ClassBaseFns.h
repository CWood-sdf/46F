#include "fns.h"
//ClassBaseFns.h -- use this file for all auton functions
list<function<void()>> threadFns;

//Sleeps the current thread
void ts (int time){
  this_thread::sleep_for(time);
}
#define toStr(a)  #a

//This macro does a lot
//First it makes a prototype function with the passed name and accepting 2 arguments:
//        an int and a boolean that defaults to true
//It then defines the function with a beginning body that writes to the APA 
//      if the 'write' arg is true
string parseInt(vision::signature& c){
  return "";
}
#define FN_WITH_APA_SIG(name, tp1) \
  void name (tp1, bool = true);       \
  void name (tp1 amnt, bool write) {       \
    string msg = #name;       \
    msg += "(";                 \
    if(write)                             \
    cout << msg << parseInt(amnt) << string(");\n");

#define FN_WITH_APA_SIG_ARG(name, tp1, arg, str) \
  void name (tp1, arg, bool = true);       \
  void name (tp1 amnt, arg, bool write) {       \
    string msg = #name;       \
    msg += "(";                 \
    string msg2 = parseInt(amnt) + ", ";             \
    msg2 += str;                  \
    if(write)                             \
    cout << msg << msg2 << string(");\n");
#define FN_WITH_APA_SIG_NO_ARG(name) \
  void name (bool = true);       \
  void name (bool write) {       \
    string msg = #name;       \
    if(write)                             \
    cout << msg << string("();\n");
//


//All the functions that are based on classes
namespace ClassFns {
  

}
