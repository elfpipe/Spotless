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
    RButton *asmStep, *asmSkip;
    
public:
    Disassembler(Spotless *parent) : Widget(dynamic_cast<Widget *>(parent)) { setName("Disassembler"); spotless = parent; }
    void createGuiObject(Layout *layout) {
        listbrowser = layout->createListbrowser();
        Layout *buttonLayout = layout->createVerticalLayout(100, 0);
        asmStep = buttonLayout->createButton("Step", "scrolldown");
        asmSkip = buttonLayout->createButton("Skip", "scrollend");
        buttonLayout->createSpace();
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
    unsigned int getAsmStepId() {
        return asmStep->getId();
    }
    unsigned int getAsmSkipId() {
        return asmSkip->getId();
    }
};
#endif