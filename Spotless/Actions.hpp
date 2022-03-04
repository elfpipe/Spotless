#ifndef SPOTLESS_ACTIONS_H
#define SPOTLESS_ACTIONS_H

#include "../ReAction/classes.h"
#include "Spotless.hpp"

class Actions : public Widget {
public:
    typedef enum {
        Load = 1,
        Start = 2,
        Stop = 3,
        StepOver = 4,
        StepInto = 5,
        StepOut = 6,
        Quit = 7
    } ButtonIds;

private:
    Spotless *spotless;
    Speedbar *actions;

public:
    Actions(Spotless *parent) : Widget(dynamic_cast<Widget *>(parent)) { spotless = parent; setName("Actions"); }
    void createGuiObject(Layout *layout) {
        actions = layout->createSpeedbar();
        actions->addButton(Load, "Load", "open");
        actions->addSpacer();
        actions->addButton(Start, "Start", "debug");
        actions->addButton(Stop, "Stop", "stop");
        actions->addSpacer();
        actions->addButton(StepOver, "Step over", "stepover");
        actions->addButton(StepInto, "Step into", "stepinto");
        actions->addButton(StepOut, "Step out", "stepout");
        actions->addSpacer();
        actions->addButton(Quit, "Quit", "quit");
        clear();
    }

    void update() {
        if(spotless->debugger.isDead()) {
            cout << "spotless->debugger.isDead()\n";
            actions->enableButton(Load, true);
            actions->enableButton(Quit, true);
        } else {
            actions->enableButton(Load, false);
            actions->enableButton(Quit, false);
        }
        
        if(spotless->debugger.isRunning()) {
            actions->enableButton(Start, false);
            actions->enableButton(Stop, true);
        } else if(spotless->debugger.isDead()) { //doesn't exist
            actions->enableButton(Start, false);
            actions->enableButton(Stop, false);
        } else {
            actions->enableButton(Start, true);
            actions->enableButton(Stop, false);
        }

        if(spotless->debugger.hasFunction()) {
            actions->enableButton(StepOver, true);
            actions->enableButton(StepInto, true);
            actions->enableButton(StepOut, true);
        } else {
            actions->enableButton(StepOver, false);
            actions->enableButton(StepInto, false);
            actions->enableButton(StepOut, false);
        }
    }

    void clear() {
        actions->enableButton(1, true);
        for(int i = 2; i <= 6; i++)
            actions->enableButton(i, false);
        actions->enableButton(7, true);
    }
};
#endif