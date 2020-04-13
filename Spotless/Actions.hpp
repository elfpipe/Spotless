#ifndef SPOTLESS_ACTIONS_H
#define SPOTLESS_ACTIONS_H

#include "../ReAction/classes.h"
#include "Spotless.hpp"

class Actions : public Widget {
public:
    typedef enum {
        Load = 1,
        Start,
        StepOver,
        StepInto,
        StepOut,
        Quit
    } ButtonIds;

private:
    Spotless *spotless;
    Speedbar *actions;

public:
    Actions(Spotless *parent) : Widget(dynamic_cast<Widget *>(parent)) { spotless = parent; setName("Actions"); }
    void createGuiObject(Layout *layout) {
        actions = layout->createSpeedbar();
        actions->addButton(1, "Load");
        actions->addButton(2, "Start");
        actions->addButton(3, "Step over");
        actions->addButton(4, "Step into");
        actions->addButton(5, "Step out");
        actions->addButton(6, "Quit");
        clear();
    }

    void update() {
        actions->enableButton(Load, false);
        for(int i = 2; i < 6+1; i++)
            actions->enableButton(i, true);        
    }

    void clear() {
        actions->enableButton(Load, true);
        for(int i = 2; i < 6+1; i++)
            actions->enableButton(i, false);
    }
};
#endif