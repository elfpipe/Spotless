#ifndef SPOTLESS_STACKTRACE_H
#define SPOTLESS_STACKTRACE_H

#include "../ReAction/classes.h"
#include "Spotless.hpp"

class Stacktrace : public Widget {
private:
    Spotless *spotless;
    Listbrowser *listbrowser;
    // RButton *button;

public:
    Stacktrace(Spotless *spotless) : Widget() { setName("Stacktrace"); this->spotless = spotless; }
    void createGuiObject(Layout *layout) {
                layout->setParent(this);
        Layout *vLayout = layout->createVerticalLayout();
        listbrowser = vLayout->createListbrowser();
        listbrowser->setStriping(true);
        // button = vLayout->createButton("Generate stacktrace...");
    }
    void update() {
        if(!open()) return;
        clear();
        // if(!spotless->childLives) return;
        vector<string> trace = spotless->debugger.stacktrace();
        listbrowser->detach();
        for(int i = 0; i < trace.size(); i++)
            listbrowser->addNode(trace[i]);
        listbrowser->attach();
    }
    void clear() {
        if(!open()) return;
        listbrowser->clear();
    }
    // unsigned int getButtonId() {
    //     return button->getId();
    // }
    // bool handleEvent(Event *event, bool *exit) {

    //     if(event->eventClass() == Event::CLASS_ButtonPress) {
    //         if(event->elementId() == getButtonId()) {
    //             update();
    //         }
    //     }
    //     return false;
    // }
};
#endif