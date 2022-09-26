#ifndef VARIABLE_CONFIG_H
#define VARIABLE_CONFIG_H
#include "vex.h"
class VariableConfig {
    static inline Button confirm = Button(Brain, 0, 0, BRAIN_WIDTH / 2, BRAIN_HEIGHT, green, "Confirm");
    static inline Button deny = Button(Brain, BRAIN_WIDTH / 2, 0, BRAIN_WIDTH / 2, BRAIN_HEIGHT, red, "Deny");
    static inline LinkedList<VariableConfig*> variables = LinkedList<VariableConfig*>();
    int index = -1;
    vector<string> optionNames;
    bool selected = false;
    bool ready = false;
    vector<Button*> buttons = vector<Button*>();
    //A callback function for when the variable is finally selected
    function<void(int)> callback = [](int) {};
    string title = "";
public:
    //Constructor that takes a list of options
    VariableConfig(vector<string> options, string title) {
        optionNames = options;
        index = -1;
        variables.push_back(this);
        this->title = title;
    }
    //Constructor that takes a list of options and a default option
    VariableConfig(vector<string> options, string title, int defaultOption) {
        optionNames = options;
        index = defaultOption;
        variables.push_back(this);
        this->title = title;
    }
    //Make all the constructors above but have them take a callback
    VariableConfig(vector<string> options, string title, function<void(int)> callback) : VariableConfig(options, title) {
        this->callback = callback;
    }
    VariableConfig(vector<string> options, string title, int defaultOption, function<void(int)> callback) : VariableConfig(options, title, defaultOption) {
        this->callback = callback;
    }
    //A function that sets the name of an index
    void setOptionName(int index, string name) {
        optionNames[index] = name;
    }
    //A function that changes the callback
    void setCallback(function<void(int)> callback) {
        this->callback = callback;
    }
    //A function that adds an option to the list of options
    void addOption(string option) {
        optionNames.push_back(option);
    }
    //A function that adds a list of options to the list of options
    void addOptions(vector<string> options) {
        for (int i = 0; i < options.size(); i++) {
            optionNames.push_back(options[i]);
        }
    }
    //A function that returns the current option
    string getOption() {
        //If the index is -1, return an empty string
        if (index == -1) {
            return "";
        }
        return optionNames[index];
    }
    //Returns if the variable is selected
    bool isSelected() {
        return selected;
    }
    //Returns if the variable is defaulted but not selected
    bool isDefaulted() {
        return index != -1 && !ready;
    }
    //A function that draws a list of buttons to select the options
    bool draw() {
        //Make the buttons if they don't exist and if the options list isn't empty
        if (buttons.size() == 0 && optionNames.size() != 0 && !selected) {
            //The number of buttons in the horizontal direction
            //Uses integer division to implicitly round down
            int buttonWidthCount = optionNames.size() - optionNames.size() / 2;
            //The width of a button
            double buttonWidth = (double)(BRAIN_WIDTH - 80) / (double)buttonWidthCount;
            //The height of a button
            double buttonHeight = (double)(BRAIN_HEIGHT + 20) / 2.0;
            //Make new buttons
            for (int i = 0; i < optionNames.size(); i++) {
                buttons.push_back(new Button(Brain, (i / 2) * buttonWidth + 40, (i % 2) * buttonHeight, buttonWidth, buttonHeight, vex::blue, vex::purple, optionNames[i], -30, -30));
            }
        }
        //Background
        Brain.Screen.clearScreen(black);
        //If the user has not selected an option
        if (!selected) {
            for (int i = 0; i < buttons.size(); i++) {
                buttons[i]->draw();
                if (buttons[i]->clicked()) {
                    selected = true;
                    //Get the option from the stored list of options
                    index = i;
                }
            }
        }
        else if (!ready) {
            confirm.draw();
            deny.draw();
            Brain.Screen.setFillColor(black);
            Brain.Screen.printAt(BRAIN_WIDTH / 2 - 20, BRAIN_HEIGHT / 2, optionNames[index].data());
            //Deny first as an extra layer of safety
            if (deny.clicked()) {
                //Send to top again
                selected = false;
            }
            else if (confirm.clicked()) {
                ready = true;
                //Delete buttons
                for (auto b : buttons) {
                    delete b;
                }
                buttons.clear();
                callback(index);
                return true;
            }
        }
        return ready;
    }
    //A function that draws each of the variable screens in order
    static bool drawAll(bool) {
        //If there are no variables, return true
        if (variables.empty()) {
            return true;
        }
        //If the first variable is ready, remove it and draw the next one
        if (variables.getBase()->draw()) {
            variables.popBase();
        }
        //If the first variable is not ready, return false
        return false;
    }
};
#endif