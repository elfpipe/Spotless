#ifndef SPOTLESS_SOURCECODE_H
#define SPOTLESS_SOURCECODE_H

#include "../ReAction/classes.h"
#include "Spotless.hpp"

class Code : public Widget {
private:
    Spotless *spotless;
    Listbrowser *listbrowser;

public:
    Code(Spotless *parent) : Widget((Widget *)parent) { setName("Code"); spotless = parent; }
    void createGuiObject(Layout *layout) {
        listbrowser = layout->createListbrowser();
    }
    void showSourceFile(string file) {

    }
    void highlightLine(int line) {
        
    }
    void update() {

    }
};
#endif