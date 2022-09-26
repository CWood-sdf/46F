#include "vex.h"
#include "Sensors/PotDial.h"
#include "BrainOS/VariableConfig.h"
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
  static void configCallback(int i){
    auton = refList[i];
    autonSelected = true;
    ready = true;
    cout << "Select auton" << endl;
  }
  static inline VariableConfig conf = VariableConfig({}, "Auton", configCallback);
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
    conf.setOptionName(index, n);
    name = n;
    CHAIN;
  }
  Auton(){
    conf.addOption("");
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
      auton->call();
    }
    else {
      SelectorArr::getVal()();
    }
  }
  static inline std::vector<Auton*> getRefs(){
    return refList;
  }
  //Returns true when done
  static inline bool selectAuton(bool remake){
    //Create button list 
    if(buttons.size() == 0 && refList.size() != 0 && !autonSelected){
      //The number of buttons in the horizontal direction
      //Uses integer division to implicitly round down
      int buttonWidthCount = refList.size() - refList.size() / 2;
      //The width of a button
      double buttonWidth = (double)(BRAIN_WIDTH - 80) / (double)buttonWidthCount;
      //The height of a button
      double buttonHeight = (double)(BRAIN_HEIGHT + 20) / 2.0;
      //Make new buttons
      for(int i = 0; i < refList.size(); i++){
        buttons.push_back(new Button(Brain, (i / 2) * buttonWidth + 40, (i % 2) * buttonHeight, buttonWidth, buttonHeight, vex::blue, vex::purple, refList[i]->name, -30, -30));
      }
    }
    //Background
    Brain.Screen.clearScreen(black);
    //If the user has not selected an auton
    if(!autonSelected){
      for(int i = 0; i < buttons.size(); i++){
        buttons[i]->draw();
        if(buttons[i]->clicked()){
          autonSelected = true;
          //Get the auton from the stored list of autons
          auton = refList[i];
        }
      }
    }
    else if(!ready){
      confirm.draw();
      deny.draw();
      Brain.Screen.setFillColor(black);
      Brain.Screen.printAt(BRAIN_WIDTH / 2 - 20, BRAIN_HEIGHT / 2, auton->name.data());
      //Deny first as an extra layer of safety
      if(deny.clicked()){
        //Send to top again
        autonSelected = false;
      }
      else if(confirm.clicked()){
        ready = true;
        //Delete buttons
        for(auto b : buttons){
          delete b;
        }
        buttons.clear();
        return true;
      }

    }
    return ready;
  }
  Auton& operator+(std::function<void()> fn){
    setFn(fn);
    potGet.attachFn(fn);
    return *this;
  }
  static bool isDone(){
    return ready;
  }
};
Auton operator "" _afn(const char* s){
  return Auton(s, [](){});
}
Auton operator+(const char* n, std::function<void()> f){
  return Auton(n, f);
}