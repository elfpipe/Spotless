#ifndef SPOTLESS_SOURCES_H
#define SPOTLESS_SOURCES_H

#include "../ReAction/classes.h"
#include "Spotless.hpp"

class Sources : public Widget {
private:
    Spotless *spotless;
    Listbrowser *listbrowser;

public:
    Sources(Spotless *parent) : Widget((Widget *)parent) { spotless = parent; }
    void createGuiObject(Layout *layout) {
        listbrowser = layout->createListbrowser();
    }
    void update() {
        
    }
};
#endif