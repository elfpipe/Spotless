#ifndef SPOTLESS_DISASSEMBLER_H
#define SPOTLESS_DISASSEMBLER_H

#include <proto/exec.h>

#include "../ReAction/classes.h"
#include "../SimpleDebug/Strings.hpp"
#include "Spotless.hpp"

class Disassembler : public Widget {
private:
    Spotless *spotless;
    Listbrowser *listbrowser;

public:
    Disassembler(Spotless *parent) : Widget(dynamic_cast<Widget *>(parent)) { setName("Disassembler"); spotless = parent; }
    void createGuiObject(Layout *layout) {
        listbrowser = layout->createListbrowser();
    }
    void update() {
        clear();
        vector<string> disassembly = spotless->debugger.disassemble();
        for(int i = 0; i < disassembly.size(); i++)
            listbrowser->addNode(disassembly[i]);
        listbrowser->focus(spotless->debugger.getDisassebmlyLine());
    }
    void clear() {
        listbrowser->clear();
    }
};
#endif