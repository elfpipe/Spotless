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

#include <libraries/keymap.h>

#include <proto/exec.h>

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
    // addSignalHandler(pipeHandler, debugger.getPipeSignal());
}

int Spotless::unfold() {
    openWindow();
    return waitForClose();
}

void Spotless::trapHandler() {
    if(spotless) {
        spotless->debugger.suspendBreaks();
        if(spotless) spotless->updateAll();
        Console::write(PublicScreen::PENTYPE_EVENT, "At break : " + spotless->debugger.printLocation());
    }
}

void Spotless::portHandler() {
    if(spotless) {
        // vector<string> messages = spotless->debugger.getMessages();
        // for(int i = 0; i < messages.size(); i++)
        //     Console::write(PublicScreen::PENTYPE_INFO, messages[i]);
        // if(spotless->debugger.isDead()) 
        spotless->debugger.handleMessages();
        // if(!spotless->debugger.lives()) {
        //     spotless->clearAll();
        //     spotless->childLives = false;
        // }
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
    cout << "deathHandler()\n";
    if(spotless) {
        spotless->clearAll();
        spotless->childLives = false;
    }
}

bool Spotless::handleEvent(Event *event) {
    if(event->eventClass() == Event::CLASS_KeyPress) {
        if(event->elementId() == RAWKEY_F1) {
                            debugger.justGo();

        }
        if(event->elementId() == RAWKEY_F2) {
                            debugger.stop();

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
    // console->clear();
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