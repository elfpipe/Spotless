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
class Disassembler;
class MemorySurfer;
class Configure;
class Spotless : public MainWindow {
private:
    MainMenu *menu;
    Actions *actions;
    Code * code;
    Sources *sources;
    Context *context;
    Stacktrace *stacktrace;
    Console *console;
    Disassembler *disassembler;
    MemorySurfer *memorySurfer;
    Configure *configure;

public:
    Debugger debugger;
    
private:
    static Spotless *spotless;
    
    bool childLives = false;
    
public:
    Spotless () : MainWindow() { create(); }
    ~Spotless() {}

    void create();
    int unfold();

    // static void trapHandler();
    static void portHandler();
    // static void pipeHandler();
    static void deathHandler();
    bool handleEvent(Event *event);
    
    void updateAll();
    void clearAll();
    
    friend class MainMenu;
    friend class Actions;
    friend class Code;
    friend class Sources;
    friend class Context;
    friend class Stacktrace;
    friend class Console;
    friend class Disassembler;
    friend class MemorySurfer;
};
#endif