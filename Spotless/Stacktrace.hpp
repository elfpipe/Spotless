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
        
    }
};
#endif