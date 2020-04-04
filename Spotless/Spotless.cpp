#include "Actions.hpp"
#include "Code.hpp"
#include "Console.hpp"
#include "Context.hpp"
#include "MainMenu.hpp"
#include "Sources.hpp"
#include "Stacktrace.hpp"
#include "Spotless.hpp"

void Spotless::create() {
    menu = new MainMenu(this);
    actions = new Actions(this);
    sources = new Sources(this);
    context = new Context(this);
    stacktrace = new Stacktrace(this);
    code = new Code(this);
    console = new Console(this);

    setMenubar(menu);
    setTopBar(actions);
    setMainView(code);
    addLeftPanelWidget(sources);
    addLeftPanelWidget(context);
    addLeftPanelWidget(stacktrace);
    addBottomPanelWidget(console);
}

int Spotless::unfold() {
    openWindow();
    return waitForClose();
}

void Spotless::trapHandler() {

}

void Spotless::portHandler() {

}

bool Spotless::handleEvent(Event *event) {
    if(event->eventClass() == Event::CLASS_ButtonPress) {
        switch(event->elementId()) {
            case Actions::Load:
            case Actions::Start:
            case Actions::StepOver:
            case Actions::StepInto:
            case Actions::StepOut:
            case Actions::Quit:
                break;
        }
    }
    if(event->eventClass() == Event::CLASS_CheckboxCheck) {

    }
    if(event->eventClass() == Event::CLASS_CheckboxUncheck) {
        
    }
    return false;
}
