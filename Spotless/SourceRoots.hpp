#ifndef SPOTLESS_SOURCEROOTS_H
#define SPOTLESS_SOURCEROOTS_H

#include "../ReAction/classes.h"
#include "Spotless.hpp"

#include <string>

using namespace std;

class SourceRoots: public Widget {
private:
    Spotless *spotless;
    Listbrowser *listbrowser;

public:
    SourceRoots(Spotless *parent) : Widget(0) { setName("Source roots"); spotless = parent; }
    void createGuiObject(Layout *layout) {
        listbrowser = layout->createListbrowser();
        Layout *hl = layout->createHorizontalLayout();
        hl->createButton("+");
        hl->createButton("-");
        hl->createButton("Done");
        update();
    }
    bool handleEvent (Event *event) {
        if(event->eventClass() == Event::CLASS_GoButtonPress) {
            if(!event->elementDescription().compare("+")) {
                string newRoot = Requesters::path(Requesters::REQUESTER_MODULE, "Choose path to add to list of source code roots...");
                spotless->debugger.addSourceRoot(newRoot);
                update();
            }
            if(!event->elementDescription().compare("-")) {
                spotless->debugger.removeSourceRoot(listbrowser->getNode(listbrowser->getSelectedLineNumber()));
                update();
            }
            if(!event->elementDescription().compare("Done")) {                
                return true;
            }
        }
        return false;
    }
    void update() {
        clear();
        list<string> roots = spotless->debugger.getSourceRoots();
        for(list<string>::iterator it = roots.begin(); it != roots.end(); it++) {
            listbrowser->addNode(*it);
        }
    }
    void clear() {
        listbrowser->clear();
    }
};
#endif