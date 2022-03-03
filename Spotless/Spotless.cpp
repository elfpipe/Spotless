#include "Actions.hpp"
#include "Code.hpp"
#include "Console.hpp"
#include "Context.hpp"
#include "MainMenu.hpp"
#include "Sources.hpp"
#include "Stacktrace.hpp"
#include "Disassembler.hpp"
#include "Spotless.hpp"
#include "MemorySurfer.hpp"

#include "../SimpleDebug/Strings.hpp"

string vectorToString(vector<string> v) {
	string result;
	for(int i = 0; i < v.size(); i++)
		result += v[i] + "\n";
	return result;
}

Listbrowser *Console::listbrowser = 0;
Spotless *Spotless::spotless = 0;

void Spotless::create() {
    spotless = this;

    menu = new MainMenu(spotless);
    actions = new Actions(spotless);
    sources = new Sources(spotless);
    context = new Context(spotless);
    stacktrace = new Stacktrace(spotless);
    code = new Code(spotless);
    console = new Console(spotless);
    disassembler = new Disassembler(spotless);

    memorySurfer = new MemorySurfer(spotless);
    configure = new Configure(spotless);
    
    setMenubar(menu);
    setTopBar(actions);
    setMainView(code);
    addLeftPanelWidget(sources);
    addLeftPanelWidget(context);
    addLeftPanelWidget(stacktrace);
    addBottomPanelWidget(disassembler);
    addBottomPanelWidget(console);

    addSignalHandler(deathHandler, SIGF_CHILD);
    addSignalHandler(trapHandler, debugger.getTrapSignal());
    addSignalHandler(portHandler, debugger.getPortSignal());
    addSignalHandler(pipeHandler, debugger.getPipeSignal());
}

int Spotless::unfold() {
    openWindow();
    return waitForClose();
}

void Spotless::trapHandler() {
    if(spotless) {
        spotless->debugger.suspendBreaks();
        if(spotless->childLives) spotless->updateAll();
        Console::write(PublicScreen::PENTYPE_EVENT, "At break : " + spotless->debugger.printLocation());
    }
}

void Spotless::portHandler() {
    if(spotless) {
        vector<string> messages = spotless->debugger.getMessages();
        for(int i = 0; i < messages.size(); i++)
            Console::write(PublicScreen::PENTYPE_CRITICAL, messages[i]);
    }
}

void Spotless::pipeHandler() {
    if(spotless) {
        vector<string> output = spotless->debugger.emptyPipe();
        for(int i = 0; i < output.size(); i++)
            Console::write(PublicScreen::PENTYPE_OUTPUT, "--] " + formatRawString(output[i]));
    }
}

void Spotless::deathHandler() {
    // cout << "deathHandler()\n";
    if(spotless) {
        spotless->clearAll();
        spotless->childLives = false;
    }
}
bool Spotless::handleEvent(Event *event) {
    if(event->eventClass() == Event::CLASS_ActionButtonPress) {
        switch(event->elementId()) {
            case Actions::Load: {
                spotless->debugger.clearRoots();
                string path;
                string file = Requesters::file(Requesters::REQUESTER_EXECUTABLE, "", path, "Select executable...");
                string unixPath = Requesters::convertToUnixRelative(path);
                spotless->debugger.addSourceRoot(unixPath);
                childLives = debugger.load(path, file, ""); //Use amigaos path for LoadSeg
                if(childLives) {
                    updateAll();
                    sources->update();
                } else {
                    console->write(PublicScreen::PENTYPE_CRITICAL, "Failed to load selected file.");
                }
                break;
            }
            case Actions::Start:
                debugger.start();
                break;
            case Actions::Stop:
                debugger.stop();
                updateAll();
                break;
            case Actions::StepOver:
                debugger.stepOver();
                break;
            case Actions::StepInto:
                debugger.stepInto();
                updateAll();
                break;
            case Actions::StepOut:
                debugger.stepOut();
                updateAll();
                break;
            case Actions::Quit:
                return true;
                break;
        }
        actions->update();
    }

    if(event->eventClass() == Event::CLASS_SelectNode) {
        string file = sources->getSelectedElement();
        console->write(PublicScreen::PENTYPE_EVENT, "Source file selected : " + file);
        string fullPath = spotless->debugger.searchSourcePath(file);
        code->show(file, fullPath);
    }
    if(event->eventClass() == Event::CLASS_CheckboxCheck) {
        code->checkboxSelected(sources->getSelectedElement(), true);
    }
    if(event->eventClass() == Event::CLASS_CheckboxUncheck) {
        code->checkboxSelected(sources->getSelectedElement(), false);
    }
    if(event->eventClass() == Event::CLASS_ButtonPress) {
        if(event->elementId() == context->getGlobalsId()) {
            context->globals();
        }
        if(event->elementId() == disassembler->getAsmBackSkipId()) {
            // this is inherently unsafe
            debugger.backSkip();
            disassembler->update();
        }
        if(event->elementId() == disassembler->getAsmStepId()) {
            debugger.safeStep();
            disassembler->update();
            sources->update();
            context->update();
        }
        if(event->elementId() == disassembler->getAsmSkipId()) {
            // this is inherently unsafe
            debugger.skip();
            disassembler->update();
        }
        if(event->elementId() == disassembler->getMemSurfId()) {
            if(memorySurfer) {
                openNewWindow((Widget *)memorySurfer);
            }
        }
    }
    return false;
}

void Spotless::updateAll() {
    actions->update();
    code->update();
    //sources->update();
    context->update();
    stacktrace->update();
    console->clear();
    disassembler->update();
    if(memorySurfer && memorySurfer->windowObject()) memorySurfer->updateDisassembly();
}

void Spotless::clearAll() {
    actions->clear();
    code->clear();
    context->clear();
    sources->clear();
    stacktrace->clear();
    disassembler->clear();
    debugger.clear();
    if(memorySurfer && memorySurfer->windowObject()) closeNewWindow(memorySurfer);
}