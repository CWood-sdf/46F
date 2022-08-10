#include "vex.h"
#include "Sensors/PotDial.h"
class Auton {
  friend class Init;
  static inline size_t idSet = 0;
  static inline std::vector<Auton*> refList;
  static inline std::vector<Button*> buttons;
  static inline Auton* auton;
  static inline bool autonSelected = false;
  static inline bool ready = false;
  static inline Button confirm = Button(Brain, 0, 0, BRAIN_WIDTH / 2, BRAIN_HEIGHT, green, "Confirm");
  static inline Button deny = Button(Brain, BRAIN_WIDTH / 2, 0, BRAIN_WIDTH / 2, BRAIN_HEIGHT, red, "Deny");
  string name;
  size_t index;
  function<void()> fn;
  SelectorArr potGet;
  typedef Auton& chain_method;
  chain_method setFn(std::function<void()> a){
    this->fn = a;
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
  Auton(string name, vector<int> v) : Auton(){
    setName(name);
    potGet = SelectorArr(v);
  }
  Auton(const Auton&) = default;
  Auton(Auton&&) = default;
  chain_method call(){
    fn();
    CHAIN;
  }
  void attachArr(vector<int> v){
    potGet = SelectorArr(v, fn);
  }
  string getName(){
    return name;
  }
  static inline void callAuton(){
    if(refList.size() == 0){
      cout << "No Auton" << endl;
      return;
    }
    if(ready){
      //Call selected auton
      auton->call();
    }
    else {
      //Call potentiometer auton
      SelectorArr::getVal()();
    }
  }
  static inline std::vector<Auton*> getRefs(){
    return refList;
  }
  //Returns true when done
  //This represents a pretty simple state machine
  static inline bool selectAuton(bool remake){
    //Create button list 
    if(buttons.size() == 0 && refList.size() != 0 && !autonSelected){
      //Kinda hacky because we're using integer division
      //If there is an odd number it will refList.size() / 2 will round down
      //This is intended
      int buttonWidthCount = refList.size() - refList.size() / 2;
      bool isOddCount = refList.size() % 2 == 1;
      double buttonWidth = (double)(BRAIN_WIDTH) / (double)buttonWidthCount;
      double buttonHeight = (double)(BRAIN_HEIGHT + 20) / 2.0;
      //Just fill buttons
      for(int i = 0; i < refList.size(); i++){
        //Once again integer division is intended
        buttons.push_back(new Button(Brain, (i / 2) * buttonWidth, (i % 2) * buttonHeight, buttonWidth, buttonHeight, vex::blue, vex::purple, refList[i]->name, -30, -30));
      }
    }
    Brain.Screen.clearScreen(black);
    //This is first stage
    //Just draw the buttons and see if they're clicked
    if(!autonSelected){
      for(int i = 0; i < buttons.size(); i++){
        buttons[i]->draw();
        if(buttons[i]->clicked()){
          autonSelected = true;
          auton = refList[i];
        }
      }
    }
    //If auton is not confirmed
    else if(!ready){
      confirm.draw();
      deny.draw();
      Brain.Screen.setFillColor(black);
      Brain.Screen.printAt(BRAIN_WIDTH / 2 - 20, BRAIN_HEIGHT / 2, auton->name.data());
      //Deny first as an extra layer of safety
      //Idk if this safety really helps at all, but whatevs
      if(deny.clicked()){
        //Send to top again
        autonSelected = false;
      }
      else if(confirm.clicked()){
        //Lock in current choice
        ready = true;
        //Delete buttons
        for(auto b : buttons){
          delete b;
        }
        buttons.clear();
        //Return true, this line is technically unnecessary, but whatevs
        return true;
      }

    }
    return ready;
  }
  //Easily make an auton function
  //Works more like +=
  Auton& operator+(std::function<void()> fn){
    setFn(fn);
    potGet.attachFn(fn);
    return *this;
  }
  //Are we ready?
  static bool isDone(){
    return ready;
  }
};
//Operators for easily making auton functions
Auton operator "" _afn(const char* s){
  return Auton(s, [](){});
}
Auton operator+(const char* n, std::function<void()> f){
  return Auton(n, f);
}