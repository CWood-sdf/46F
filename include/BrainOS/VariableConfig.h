#ifndef VARIABLE_CONFIG_H
#define VARIABLE_CONFIG_H
#include "vex.h"
#include <functional>
class VariableConfig
{
    static Button confirm;
    static Button deny;
    static inline LinkedList<VariableConfig*> variables = LinkedList<VariableConfig*>();
    int index = -1;
    vector<string> optionNames;
    bool selected = false;
    bool ready = false;
    vector<Button*> buttons = vector<Button*>();
    function<bool()> bypass = []()
    { return false; };
    // A callback function for when the variable is finally selected
    function<void(int)> callback = [](int) {};
    string title = "";

public:
    // Constructor that takes a list of options
    VariableConfig(vector<string> options, string title);
    // Constructor that takes a list of options and a default option
    VariableConfig(vector<string> options, string title, int defaultOption);
    // Make all the constructors above but have them take a callback
    VariableConfig(vector<string> options, string title, function<void(int)> callback);
    VariableConfig(vector<string> options, string title, int defaultOption, function<void(int)> callback);
    VariableConfig& addBypass(function<bool()> bypass);
    // A function that sets the name of an index
    void setOptionName(int index, string name);
    // A function that changes the callback
    void setCallback(function<void(int)> callback);
    // A function that adds an option to the list of options
    void addOption(string option);
    // A function that adds a list of options to the list of options
    void addOptions(vector<string> options);
    // A function that returns the current option
    string getOption();
    // Returns if the variable is selected
    bool isSelected();
    // Returns if the variable is defaulted but not selected
    bool isDefaulted();
    // A function that draws a list of buttons to select the options
    bool draw();
    // A function that draws each of the variable screens in order
    static bool drawAll(bool);
};
#endif
