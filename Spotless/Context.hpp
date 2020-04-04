#ifndef SPOTLESS_VARIABLES_H
#define SPOTLESS_VARIABLES_H

#include "../ReAction/classes.h"
#include "Spotless.hpp"

class Context : public Widget {
private:
    Spotless *spotless;
    Listbrowser *listbrowser;

public:
    Context(Spotless *parent) : Widget((Widget *)parent) { setName("Variables"); spotless = parent; }
    void createGuiObject(Layout *layout) {
        listbrowser = layout->createListbrowser();
        listbrowser->setHierachical(true);
    }
    void update() {
        
    }
};
#endif