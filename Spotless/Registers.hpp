#ifndef SPOTLESS_REGISTERS_H
#define SPOTLESS_REGISTERS_H

#include "../ReAction/classes.h"
#include "Spotless.hpp"

#include "../SimpleDebug/Strings.hpp"

class Registers : public Widget {
private:
    Spotless *spotless;
    Listbrowser *listbrowser;

public:
    Registers(Spotless *spotless) : Widget(spotless) { setName("Registers"); this->spotless = spotless; }
    void createGuiObject(Layout *layout) {
                layout->setParent(this);
        listbrowser = layout->createListbrowser();
        listbrowser->setHierachical(true);
    }
    void add(vector<string> context) {
        listbrowser->detach();
        for(int i = 0; i < context.size(); i++) {
            astream str(context[i]);
            listbrowser->addNode(context[i]);
        }
        listbrowser->attach();
    }
    void update() {
        clear();
        add(spotless->debugger.registersDump());
    }
    void clear() {
        listbrowser->clear();
    }
};
#endif