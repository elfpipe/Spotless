#ifndef SPOTLESS_GUI_H
#define SPOTLESS_GUI_H

#include "../ReAction/classes.h"
#include "../SimpleDebug/Debugger.hpp"

class MainMenu;
class Actions;
class Code;
class Sources;
class Stacktrace;
class Context;
class Console;
class Spotless : public MainWindow {
private:
    MainMenu *menu;
    Actions *actions;
    Code * code;
    Sources *sources;
    Context *context;
    Stacktrace *stacktrace;
    Console *console;

    Debugger debugger;

public:
    Spotless () : MainWindow() { create(); }
    ~Spotless() {}

    void create();
    int unfold();

    Debugger *debugger() { return &debugger; }


    void trapHandler() {

    }
    void portHandler() {

    }
    bool handleEvent(Event *event) {
        if(event->eventClass == Event::CLASS_ButtonPress) {
            switch(event->elementId()) {
                case Actions::Load:
                case Actions::Start:
                case Actions::StepOver:
                case Actions::StepInto:
                case Actions::StepOut:
                case Actions::Quit:
            }
        }
        if(event->eventClass == Event::CLASS_CheckboxCheck) {

        }
        if(event->eventClass == Event::CLASS_CheckboxUncheck) {
            
        }
    }
};
#endif