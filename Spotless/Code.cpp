#include "Sources.hpp"
#include "Code.hpp"

    bool Code::handleEvent(Event *event, bool *exit) {
        if(event->eventClass() == Event::CLASS_CheckboxCheck) {
            checkboxSelected(spotless->sources->getSelectedElement(), true);
        }
        if(event->eventClass() == Event::CLASS_CheckboxUncheck) {
            checkboxSelected(spotless->sources->getSelectedElement(), false);
        }
        return false;
    }
