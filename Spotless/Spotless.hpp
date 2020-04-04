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
};
#endif