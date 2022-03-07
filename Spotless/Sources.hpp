#ifndef SPOTLESS_SOURCES_H
#define SPOTLESS_SOURCES_H

#include "../ReAction/classes.h"
#include "Spotless.hpp"
#include "Context.hpp"
#include "Code.hpp"
#include "Console.hpp"
#include <string>

using namespace std;

class Sources : public Widget {
private:
    Spotless *spotless;
    Listbrowser *listbrowser;
public:
    Sources(Spotless *spotless) : Widget(spotless) { setName("Source files"); this->spotless = spotless; }
    void createGuiObject(Layout *layout) {
                layout->setParent(this);

        Layout *vl = layout->createVerticalLayout();
        // vl->createButton("Source roots -->");
        listbrowser = vl->createListbrowser();
    }
    string getSelectedElement() {
        return listbrowser->getNode(listbrowser->getSelectedLineNumber());
    }
    void update() {
        vector<string> sources = spotless->debugger.sourceFiles();
        listbrowser->clear();
        listbrowser->detach();
        for(int i = 0; i < sources.size(); i++)
            listbrowser->addNode(sources[i]);
        listbrowser->attach();
    }
    void clear() {
        listbrowser->clear();
    }
    bool handleEvent(Event *event) {
        if(event->eventClass() == Event::CLASS_SelectNode) {
            string file = spotless->sources->getSelectedElement();
            spotless->console->write(PublicScreen::PENTYPE_EVENT, "Source file selected : " + file);
            string fullPath = spotless->debugger.searchSourcePath(file);
            spotless->code->show(file, fullPath);
        }
        if(event->eventClass() == Event::CLASS_CheckboxPress) {
            if(event->elementId() == spotless->context->getGlobalsId()) {
                spotless->context->globals();
            }
        }
        return false;
    }
};
#endif