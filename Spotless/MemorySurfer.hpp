#ifndef SPOTLESS_MEMORYSURFER_H
#define SPOTLESS_MEMORYSURFER_H

#include <proto/exec.h>

#include "../ReAction/classes.h"
#include "../SimpleDebug/Strings.hpp"
#include "Spotless.hpp"

class MemorySurfer : public Widget {
private:
    Spotless *spotless;
    Listbrowser *disassembly, *hex;
    RButton *disassemble, *readHex, *done, *run;
    RString *symbolName, *addressString, *breakpointString;
    RButton *asmBackSkip, *asmStep, *asmSkip;

    char buffer1[4096], buffer2[4096];

public:
    MemorySurfer(Spotless *parent) : Widget(0) { setName("Memory surfer"); spotless = parent; }
    void createGuiObject(Layout *layout) {
        Layout *disassemblyLayout = layout->createLabeledLayout("Disassemble symbol");
        Layout *disassemblyInputLayout = layout->createHorizontalLayout();
        symbolName = disassemblyInputLayout->createString("");
        disassemble = disassemblyInputLayout->createButton("Disassemble");
        disassembly = disassemblyLayout->createListbrowser();

        Layout *hexLayout = layout->createLabeledLayout("Hex view");
        Layout *hexInputLayout = hexLayout->createHorizontalLayout();
        addressString = hexInputLayout->createString("");
        readHex = hexInputLayout->createButton("Read hex");
        hex = hexLayout->createListbrowser();

        Layout *runnerLayout = layout->createLabeledLayout("Blind runner");
        Layout *runnerInputLayout = runnerLayout->createHorizontalLayout();
        breakpointString = runnerInputLayout->createString("<enter breakpoint symbol>");
        run = runnerInputLayout->createButton("Run");
        asmBackSkip = runnerInputLayout->createButton("Back skip", "scrollstart");
        asmStep = runnerInputLayout->createButton("Asm step", "scrolldown");
        asmSkip = runnerInputLayout->createButton("Asm skip", "scrollend");

        done = layout->createButton("Done");
    }
    bool handleEvent (Event *event) {
        cout << "handleEvent : elementId() == " << event->elementId() << "\n";
        if(event->eventClass() == Event::CLASS_ButtonPress) {
            if(event->elementId() == getDisassembleId()) {
                updateDisassembly();
            }
            if(event->elementId() == getReadHexId()) {
                updateHex();
            }
            if(event->elementId() == getRunId()) {
                blindRunner();
            }
            if(event->elementId() == getAsmBackSkipId()) {
                // this is inherently unsafe
                spotless->debugger.backSkip();
                updateDisassembly();
                updateHex();
            }
            if(event->elementId() == getAsmStepId()) {
                spotless->debugger.step();
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
                spotless->debugger.skip();
                updateDisassembly();
                updateHex();
            }
            if(event->elementId() == getDoneId()) {
                return true;
            }
        }
        return false;
    }

    void updateDisassembly() {
        disassembly->clear();
        string symbol = symbolName->getContent();
        cout << "updateDisassembly : symbol = " << symbol << "\n";
        vector<string> result = spotless->debugger.disassembleSymbol(symbol);
        for(vector<string>::iterator it = result.begin(); it != result.end(); it++)
            disassembly->addNode((*it));
        int line = spotless->debugger.getDisassebmlyLine();
        cout << "focus of dissasmbly : line = " << line << "\n";
        disassembly->focus(line);
    }
    void updateHex() {
        hex->clear();
        string hexString = addressString->getContent();
        cout << "updateHex : hexString = " << hexString << "\n";
        vector<string> result = spotless->debugger.hexDump(hexString);
        for(vector<string>::iterator it = result.begin(); it != result.end(); it++)
            hex->addNode((*it));
        hex->focus(spotless->debugger.getHexLine());
    }
    void blindRunner() {
        string breakpoint = breakpointString->getContent();
        if(breakpoint.size()) {
            spotless->debugger.breakpointSymbol(breakpoint, true);
        }
        spotless->debugger.activateBreaks();
        spotless->debugger.start();
        spotless->debugger.wait();
        spotless->debugger.suspendBreaks();
        spotless->debugger.breakpointSymbol(breakpoint, false);

        sprintf(buffer1, "%s", breakpoint.c_str());
        symbolName->setContent(buffer1);
        updateDisassembly();
        sprintf(buffer2, "0x%x", spotless->debugger.getSymbolValue(breakpoint));
        addressString->setContent(buffer2);
        updateHex();
    }
    unsigned int getDisassembleId() {
        return disassemble->getId();
    }
    unsigned int getReadHexId() {
        return readHex->getId();
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

};
#endif