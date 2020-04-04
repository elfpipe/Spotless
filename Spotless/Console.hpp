#ifndef SPOTLESS_CONSOLE_H
#define SPOTLESS_CONSOLE_H

#include "../ReAction/classes.h"
#include "Spotless.hpp"

class Console : public Widget {
private:
    Spotless *spotless;
    static Listbrowser *listbrowser;

public:
    Console(Spotless *parent) : Widget(dynamic_cast<Widget *>(parent)) { setName("Console"); spotless = parent; }
    void createGuiObject(Layout *layout) {
        listbrowser = layout->createListbrowser();
    }
    static void write(PublicScreen::PenType pen, string text) {
        if(listbrowser) {
            listbrowser->setPen(pen);
            listbrowser->addNode(text);
        }
    }
    void clear() {
        listbrowser->clear();
    }
};
#endif