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

private:
    static Spotless *spotless;
    
public:
    Spotless () : MainWindow() { create(); }
    ~Spotless() {}

    void create();
    int unfold();

    static void trapHandler();
    static void portHandler();
    static void pipeHandler();
    bool handleEvent(Event *event);

    void updateAll();

    friend class MainMenu;
    friend class Actions;
    friend class Code;
    friend class Sources;
    friend class Context;
    friend class Stacktrace;
    friend class Console;
};
#endif