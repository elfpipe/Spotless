#ifndef SPOTLESS_SOURCECODE_H
#define SPOTLESS_SOURCECODE_H

#include "../ReAction/classes.h"
#include "Spotless.hpp"

class Code : public Widget {
private:
    Spotless *spotless;
    Listbrowser *listbrowser;

public:
    Code(Spotless *parent) : Widget((Widget *)parent) { spotless = parent; }
    void createGuiObject(Layout *layout) {
        listbrowser = layout->createListbrowser();
    }
};
#endif