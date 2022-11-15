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
class Registers;
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
    Registers *registers;

public:
    Debugger debugger;
    
private:
    static Spotless *spotless;
        
public:
    Spotless () : MainWindow() { create(); }
    ~Spotless() { destroy(); }

    void create();
    void destroy();
    int unfold();

    // static void deathHandler();
    // static void trapHandler();
    static void portHandler();
    // static void pipeHandler();
    bool handleEvent(Event *event, bool *exit);
    
    void updateAll(bool doSources = false);
    void clearAll();
    
    vector<Widget *> getAllPanelWidgets();

    friend class MainMenu;
    friend class Actions;
    friend class Code;
    friend class Sources;
    friend class Context;
    friend class Stacktrace;
    friend class Console;
    friend class Disassembler;
    friend class MemorySurfer;
    friend class Configure;
};
#endif