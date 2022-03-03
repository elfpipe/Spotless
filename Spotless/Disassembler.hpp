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
    RButton *asmBackSkip, *asmStep, *asmSkip, *memSurf;
    
public:
    Disassembler(Spotless *parent) : Widget(dynamic_cast<Widget *>(parent)) { setName("Disassembler"); spotless = parent; }
    void createGuiObject(Layout *layout) {
        listbrowser = layout->createListbrowser();
        Layout *buttonLayout = layout->createVerticalLayout(100, 0);
        asmBackSkip = buttonLayout->createButton("Back skip", "scrollstart");
        asmStep = buttonLayout->createButton("Asm step", "scrolldown");
        asmSkip = buttonLayout->createButton("Asm skip", "scrollend");
        buttonLayout->createSpace();
        memSurf = buttonLayout->createButton("Memory surfer", "scrollpan");
    } 
    void update() {
        clear();
        vector<string> disassembly = spotless->debugger.disassemble();
        listbrowser->detach();
        for(int i = 0; i < disassembly.size(); i++)
            listbrowser->addNode(disassembly[i]);
        listbrowser->attach();
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
    unsigned int getAsmBackSkipId() {
        return asmBackSkip->getId();
    }
    unsigned int getMemSurfId() {
        return memSurf->getId();
    }
};
#endif