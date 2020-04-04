#ifndef SPOTLESS_CONSOLE_H
#define SPOTLESS_CONSOLE_H

#include "../ReAction/classes.h"
#include "Spotless.hpp"

class Console : public Widget {
private:
    Spotless *spotless;
    Listbrowser *listbrowser;

public:
    Console(Spotless *parent) : Widget(dynamic_cast<Widget *>(parent)) { spotless = parent; }
    void createGuiObject(Layout *layout) {
        listbrowser = layout->createListbrowser();
    }
};
#endif