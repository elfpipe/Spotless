#ifndef SPOTLESS_MEMORYSURFER_H
#define SPOTLESS_MEMORYSURFER_H

#include <proto/exec.h>

#include "../ReAction/classes.h"
#include "../SimpleDebug/Strings.hpp"
#include "Spotless.hpp"
#include "../SimpleDebug/Breaks.hpp"

class MemorySurfer : public Widget {
private:
    Spotless *spotless;
    Listbrowser *disassembly, *hex;
    RButton /**disassemble,*/ /**readHex,*/ *done, *run;
    RString *symbolName, *addressString; //, *breakpointString;
    RButton *asmBackSkip, *asmStep, *asmSkip;

    Breaks breaks;

    char buffer1[4096], buffer2[4096];

public:
    MemorySurfer(Spotless *parent) : Widget(0) { setName("Memory surfer"); spotless = parent; }
    void createGuiObject(Layout *layout) {
        run = layout->createButton("Run", "debug");

        Layout *disassemblyLayout = layout->createLabeledLayout("Disassemble symbol");
        symbolName = disassemblyLayout->createString("");
        // disassemble = disassemblyInputLayout->createButton("Disassemble");
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
        // readHex = hexInputLayout->createButton("Read hex");
        hex = hexLayout->createListbrowser();

        // Layout *runnerInputLayout = runnerLayout->createHorizontalLayout();
        // breakpointString = runnerInputLayout->createString("<enter breakpoint symbol>");

        done = layout->createButton("Done");
    }
    bool handleEvent (Event *event) {
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
            breaks.insert((uint32_t)disassembly->getUserData (disassembly->getSelectedLineNumber()));
            updateDisassembly();
        }
        if(event->eventClass() == Event::CLASS_CheckboxUncheck) {
            breaks.remove((uint32_t)disassembly->getUserData (disassembly->getSelectedLineNumber()));
            updateDisassembly();
        }
        if(event->eventClass() == Event::CLASS_ButtonPress) {
            if(event->elementId() == getRunId()) {
                blindRunner();
            }
            if(event->elementId() == getAsmBackSkipId()) {
                // this is inherently unsafe
                spotless->debugger.unsafeBackSkip();
                updateDisassembly();
                updateHex();
            }
            if(event->elementId() == getAsmStepId()) {
                spotless->debugger.unsafeStep();
                string newSymbol = spotless->debugger.getSymbolFromAddress(spotless->debugger.getIp());
                if(newSymbol.size()) {
                    sprintf(buffer1, "%s", newSymbol.c_str());
                    symbolName->setContent(buffer1);
                }
                updateDisassembly();
                sprintf(buffer2, "0x%x", spotless->debugger.getIp());
                addressString->setContent(buffer2);
                updateHex();
            }
            if(event->elementId() == getAsmSkipId()) {
                // this is inherently unsafe
                spotless->debugger.unsafeSkip();
                updateDisassembly();
                updateHex();
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
        disassembly->clear();
        string symbol = symbolName->getContent();
        vector<string> result = spotless->debugger.disassembleSymbol(symbol);
        if(result.size()) {
            uint32_t address = spotless->debugger.getSymbolValue(symbol);
            for(vector<string>::iterator it = result.begin(); it != result.end(); it++) {
                vector<string> data;
                data.push_back("");
                data.push_back(*it);
                disassembly->addCheckboxNode(data, true, breaks.isBreak(address), (void *)address);
                address += 4;
            }
        }
        int line = spotless->debugger.getDisassebmlyLine();
        disassembly->focus(line);
    }
    void updateHex() {
        hex->clear();
        string hexString = addressString->getContent();
        vector<string> result = spotless->debugger.hexDump(hexString);
        for(vector<string>::iterator it = result.begin(); it != result.end(); it++)
            hex->addNode((*it));
        hex->focus(spotless->debugger.getHexLine());
    }
    void blindRunner() {
        breaks.activate();
        spotless->debugger.justGo();
        spotless->debugger.wait();
        breaks.deactivate();

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
    unsigned int getDoneId() {
        return done->getId();
    }
    unsigned int getRunId() {
        return run->getId();
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
    }
};
#endif