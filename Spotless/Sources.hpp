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

    vector<string> buffer;

public:
    Sources(Spotless *spotless) : Widget() { setName("Source files"); this->spotless = spotless; }
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
        if(!open()) return;
        if(!buffer.size())
            buffer = spotless->debugger.sourceFiles();
        listbrowser->clear();
        listbrowser->detach();
        for(int i = 0; i < buffer.size(); i++)
            listbrowser->addNode(buffer[i]);
        listbrowser->attach();
    }
    void showCurrent() {
        if(!open()) return;
        string current = spotless->debugger.getSourceFile();
        if(current.size()) listbrowser->showSelected(current);
    }
    void clear() {
        if(!open()) return;
        listbrowser->clear();
        buffer.clear();
    }
    bool handleEvent(Event *event, bool *exit) {
        if(event->eventClass() == Event::CLASS_SelectNode) {
            string file = spotless->sources->getSelectedElement();
            spotless->console->write(PublicScreen::PENTYPE_EVENT, "Source file selected : " + file);
            string fullPath = spotless->debugger.searchSourcePath(file);
            spotless->code->show(file, fullPath);
        }
        return false;
    }
};
#endif