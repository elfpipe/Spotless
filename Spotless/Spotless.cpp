#include "Actions.hpp"
#include "Code.hpp"
#include "Console.hpp"
#include "Context.hpp"
#include "MainMenu.hpp"
#include "Sources.hpp"
#include "Stacktrace.hpp"
#include "Spotless.hpp"

#include "../SimpleDebug/Strings.hpp"

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

    setMenubar(menu);
    setTopBar(actions);
    setMainView(code);
    addLeftPanelWidget(sources);
    addLeftPanelWidget(context);
    addLeftPanelWidget(stacktrace);
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
            Console::write(PublicScreen::PENTYPE_CRITICAL, "--] " + formatRawString(output[i]));
    }
}

void Spotless::deathHandler() {
    if(spotless) {
        spotless->clearAll();
        spotless->childLives = false;
    }
}
bool Spotless::handleEvent(Event *event) {
    if(event->eventClass() == Event::CLASS_ButtonPress) {
        switch(event->elementId()) {
            case Actions::Load: {
                string path;
                string file = Requesters::file(Requesters::REQUESTER_EXECUTABLE, "", path, "Select executable...");
                childLives = debugger.load(patch::fullPath(path, file), "");
                updateAll();
                break;
            }
            case Actions::Start:
                debugger.start();
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
                break;
            case Actions::Quit:
                return true;
                break;
        }
    }
    if(event->eventClass() == Event::CLASS_SelectNode) {
        string file = sources->getSelectedElement();
        console->write(PublicScreen::PENTYPE_EVENT, "Source file selected : " + file);
        code->show(file);
    }
    if(event->eventClass() == Event::CLASS_CheckboxCheck) {
        code->checkboxSelected(sources->getSelectedElement(), true);
    }
    if(event->eventClass() == Event::CLASS_CheckboxUncheck) {
        code->checkboxSelected(sources->getSelectedElement(), false);
    }
    if(event->eventClass() == Event::CLASS_GoButtonPress) {
        if(!event->elementDescription().compare("Globals")) {
            context->globals();
        }
    }
    return false;
}

void Spotless::updateAll() {
    actions->update();
    code->update();
    sources->update();
    context->update();
    stacktrace->update();
    console->clear();
}

void Spotless::clearAll() {
    actions->clear();
    code->clear();
    context->clear();
    sources->clear();
    stacktrace->clear();
    debugger.clear();
}