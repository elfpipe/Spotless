#ifndef SPOTLESS_STACKTRACE_H
#define SPOTLESS_STACKTRACE_H

#include "../ReAction/classes.h"
#include "Spotless.hpp"

class Stacktrace : public Widget {
private:
    Spotless *spotless;
    Listbrowser *listbrowser;

public:
    Stacktrace(Spotless *parent) : Widget((Widget *)parent) { setName("Stacktrace"); spotless = parent; }
    void createGuiObject(Layout *layout) {
        listbrowser = layout->createListbrowser();
        listbrowser->setStriping(true);
    }
    void update() {
        clear();
        if(!spotless->childLives) return;
        vector<string> trace = spotless->debugger.stacktrace();
        for(int i = 0; i < trace.size(); i++)
            listbrowser->addNode(trace[i]);
    }
    void clear() {
        listbrowser->clear();
    }
};
#endif