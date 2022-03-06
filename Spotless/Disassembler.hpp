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
    Disassembler(Spotless *spotless) : Widget(spotless) { setName("Disassembler"); this->spotless = spotless; }
    void createGuiObject(Layout *layout) {
                layout->setParent(this);

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

    bool handleEvent(Event *event) {

        if(event->eventClass() == Event::CLASS_ButtonPress) {
            if(event->elementId() == spotless->disassembler->getAsmBackSkipId()) {
                // this is inherently unsafe
                spotless->debugger.backSkip();
                spotless->disassembler->update();
            }
            if(event->elementId() == spotless->disassembler->getAsmStepId()) {
                spotless->debugger.safeStep();
                spotless->disassembler->update();
                spotless->sources->update();
                spotless->context->update();
            }
            if(event->elementId() == spotless->disassembler->getAsmSkipId()) {
                // this is inherently unsafe
                spotless->debugger.skip();
                spotless->disassembler->update();
            }
            if(event->elementId() == spotless->disassembler->getMemSurfId()) {
                if(spotless->memorySurfer) {
                    spotless->openNewWindow((Widget *)spotless->memorySurfer);
                }
            }
        }
        return false;
    }

};
#endif