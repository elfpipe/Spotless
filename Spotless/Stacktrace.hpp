#ifndef SPOTLESS_STACKTRACE_H
#define SPOTLESS_STACKTRACE_H

#include "../ReAction/classes.h"
#include "Spotless.hpp"

class Stacktrace : public Widget {
private:
    Spotless *spotless;
    Listbrowser *listbrowser;

public:
    Stacktrace(Spotless *spotless) : Widget(spotless) { setName("Stacktrace"); this->spotless = spotless; }
    void createGuiObject(Layout *layout) {
                layout->setParent(this);

        listbrowser = layout->createListbrowser();
        listbrowser->setStriping(true);
    }
    void update() {
        clear();
        if(!spotless->childLives) return;
        vector<string> trace = spotless->debugger.stacktrace();
        listbrowser->detach();
        for(int i = 0; i < trace.size(); i++)
            listbrowser->addNode(trace[i]);
        listbrowser->attach();
    }
    void clear() {
        listbrowser->clear();
    }
};
#endif