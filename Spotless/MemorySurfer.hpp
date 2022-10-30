#ifndef SPOTLESS_MEMORYSURFER_H
#define SPOTLESS_MEMORYSURFER_H

#include <proto/exec.h>

#include "../ReAction/classes.h"
#include "../SimpleDebug/Strings.hpp"
#include "Spotless.hpp"
#include "../SimpleDebug/Breaks.hpp"

#include <list>
using namespace std;

class MemorySurfer : public Widget {
private:
    Spotless *spotless;
    Listbrowser *disassembly, *hex;
    RButton *done, *run, *stepOver, *stepInto, *stepOut;
    RString *symbolName, *addressString;
    RButton *clearBreaks;
    RButton *asmBackSkip, *asmStep, *asmSkip;

    // Breaks breaks;
    list<uint32_t> breaks;
    string lastSymbol;

    char buffer1[4096], buffer2[4096];

public:
    MemorySurfer(Spotless *spotless) : Widget(0) { setName("Memory surfer"); this->spotless = spotless; }
    void createGuiObject(Layout *layout) {
        Layout *controlLayout = layout->createHorizontalLayout(0, 0);
        run = controlLayout->createButton("Run", "debug");
        stepOver = controlLayout->createButton("Step over", "stepover");
        stepInto = controlLayout->createButton("Step into", "stepinto");
        stepOut = controlLayout->createButton("Step out", "stepout");
        controlLayout->createSpace();

        Layout *disassemblyLayout = layout->createLabeledLayout("Disassemble symbol");
        Layout *inputLayout = disassemblyLayout->createVerticalLayout(100, 0);
        symbolName = inputLayout->createString("");
        inputLayout->createSpace();
        clearBreaks = inputLayout->createButton("Clear breaks");
        Layout *hLayout = disassemblyLayout->createHorizontalLayout();
        disassembly = hLayout->createListbrowser();
        disassembly->setColumnTitles("Br|Command");
        Layout *vLayout = hLayout->createVerticalLayout(100, 0);
        asmBackSkip = vLayout->createButton("Back skip", "scrollstart");
        asmStep = vLayout->createButton("Asm step", "scrolldown");
        asmSkip = vLayout->createButton("Asm skip", "scrollend");
        vLayout->createSpace();

        Layout *hexLayout = layout->createLabeledLayout("Hex view");
        addressString = hexLayout->createString("");
        hex = hexLayout->createListbrowser();

        done = layout->createButton("Done");
    }
    bool handleEvent (Event *event, bool *exit) {
        bool result = false;

        if (event->eventClass() == Event::CLASS_StringEntry) {
            if(event->elementId() == getSymbolNameId()) {
                updateDisassembly();
            }
            if(event->elementId() == getAddressId()) {
                updateHex();
            }

        }
        if(event->eventClass() == Event::CLASS_CheckboxCheck) {
            spotless->debugger.breakpointAddress((uint32_t)disassembly->getUserData (disassembly->getSelectedLineNumber()), true);
            // updateDisassembly();
        }
        if(event->eventClass() == Event::CLASS_CheckboxUncheck) {
            spotless->debugger.breakpointAddress((uint32_t)disassembly->getUserData (disassembly->getSelectedLineNumber()), false);
            // updateDisassembly();
        }
        if(event->eventClass() == Event::CLASS_ButtonPress) {
            if(event->elementId() == getRunId()) {
                blindRunner();
            }
            if(event->elementId() == getClearBreaksId()) {
                breaks.clear();
                updateDisassembly();
            }
            if(event->elementId() == getStepOverId()) {
                spotless->debugger.asmStepOver();
                update();
            }
            if(event->elementId() == getStepIntoId()) {
                spotless->debugger.asmStepInto();
                update();
            }
            if(event->elementId() == getStepOutId()) {
                spotless->debugger.asmStepOut();
                update();
            }
            if(event->elementId() == getAsmBackSkipId()) {
                // this is inherently unsafe
                spotless->debugger.unsafeBackSkip();
                update();
            }
            if(event->elementId() == getAsmStepId()) {
                spotless->debugger.unsafeStep();
                update();
            }
            if(event->elementId() == getAsmSkipId()) {
                // this is inherently unsafe
                spotless->debugger.unsafeSkip();
                update();
            }
            if(event->elementId() == getDoneId()) {
                breaks.clear();
                result = true;
            }
            spotless->updateAll();
        }
        return result;
    }

    void updateDisassembly() {
        string symbol = symbolName->getContent();
        if(symbol.compare(lastSymbol)) disassembly->clear();
        vector<string> result = spotless->debugger.disassembleSymbol(symbol);
        if(result.size()) {
            uint32_t address = spotless->debugger.getSymbolValue(symbol);
            disassembly->detach();
            for(vector<string>::iterator it = result.begin(); it != result.end(); it++) {
                vector<string> data;
                data.push_back("");
                data.push_back(*it);
                disassembly->addCheckboxNode(data, true, spotless->debugger.isBreak(address), (void *)address);
                address += 4;
            }
            disassembly->attach();
        }
        int line = spotless->debugger.getDisassebmlyLine();
        disassembly->focus(line);
    }
    void updateHex() {
        hex->clear();
        string hexString = addressString->getContent();
        vector<string> result = spotless->debugger.hexDump(hexString);
        hex->detach();
        for(vector<string>::iterator it = result.begin(); it != result.end(); it++)
            hex->addNode((*it));
        hex->attach();
        hex->focus(spotless->debugger.getHexLine());
    }
    void blindRunner() {
        spotless->debugger.start();
        // breaks.activate();
        // spotless->debugger.setTrace();
        // spotless->debugger.justGo();
        // spotless->debugger.waitTrace();
        // breaks.deactivate();

        // spotless->updateAll();
    }
    void update() {
        string breakpoint = spotless->debugger.getSymbolFromAddress(spotless->debugger.getIp());
        if(!breakpoint.size()) breakpoint = "<not a symbol>";
        sprintf(buffer1, "%s", breakpoint.c_str());
        symbolName->setContent(buffer1);
        updateDisassembly();
        sprintf(buffer2, "0x%x", spotless->debugger.getSymbolValue(breakpoint));
        addressString->setContent(buffer2);
        updateHex();
    }
    unsigned int getSymbolNameId() {
        return symbolName->getId();
    }
    unsigned int getAddressId() {
        return addressString->getId();
    }
    unsigned int getClearBreaksId() {
        return clearBreaks->getId();
    }
    unsigned int getDoneId() {
        return done->getId();
    }
    unsigned int getRunId() {
        return run->getId();
    }
    unsigned int getStepOverId() {
        return stepOver->getId();
    }
    unsigned int getStepIntoId() {
        return stepInto->getId();
    }
    unsigned int getStepOutId() {
        return stepOut->getId();
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
    void clear() {
        //what to do?
        for(list<uint32_t>::iterator it = breaks.begin(); it != breaks.end(); it++) {
            spotless->debugger.breakpointAddress(*it, false);
        }
        breaks.clear();
    }
};
#endif