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
        actions->addButton(1, "Load", "open");
        actions->addSpacer();
        actions->addButton(2, "Start", "debug");
        actions->addButton(3, "Stop", "stop");
        actions->addSpacer();
        actions->addButton(4, "Step over", "stepover");
        actions->addButton(5, "Step into", "stepinto");
        actions->addButton(6, "Step out", "stepout");
        actions->addSpacer();
        actions->addButton(7, "Quit", "quit");
        clear();
    }

    void update() {
        actions->enableButton(Load, false);
        if(spotless->debugger.isRunning() == false) {
            actions->enableButton(2, true);
            actions->enableButton(3, false);
        } else {
            actions->enableButton(2, false);
            actions->enableButton(3, true);
        } 
        for(int i = 4; i <= 6; i++) {
            if(spotless->debugger.hasFunction())
                actions->enableButton(i, true);        
            else
                actions->enableButton(i, false);
        }
        actions->enableButton(7, false);
    }

    void clear() {
        actions->enableButton(1, true);
        for(int i = 2; i <= 6; i++)
            actions->enableButton(i, false);
        actions->enableButton(7, true);
    }
};
#endif