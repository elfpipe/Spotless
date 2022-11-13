#ifndef SPOTLESS_CONSOLE_H
#define SPOTLESS_CONSOLE_H

#include "../ReAction/classes.h"
#include "Spotless.hpp"

class Console : public Widget {
private:
    static Spotless *spotless;
    static Listbrowser *listbrowser;

public:
    Console(Spotless *spotless) : Widget(spotless) { setName("Console"); this->spotless = spotless; }
    void createGuiObject(Layout *layout) {
                layout->setParent(this);

        listbrowser = layout->createListbrowser();
    }
    static void write(PublicScreen::PenType pen, string text) {
        if(!spotless->console->open()) return;

        static int n = 0;
        if(n++ == 100) { clear()  ; n = 0; }

        if(listbrowser) {
            listbrowser->setPen(pen);
            listbrowser->detach();
            listbrowser->addNode(text);
            listbrowser->attach();
        }
    }
    static void clear() {
        listbrowser->clear();
    }
};
#endif