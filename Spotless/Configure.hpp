#ifndef SPOTLESS_SOURCEROOTS_H
#define SPOTLESS_SOURCEROOTS_H

#include "../ReAction/classes.h"
#include "Spotless.hpp"

#include <string>

using namespace std;

class Configure: public Widget {
private:
    Spotless *spotless;
    Listbrowser *listbrowser;
    RButton *add, *remove, *done;

public:
    Configure(Spotless *spotless) : Widget(spotless) { setName("Configure"); this->spotless = spotless; }
    void createGuiObject(Layout *layout) {
        Layout *rootsLayout = layout->createLabeledLayout("Source roots");
        listbrowser = rootsLayout->createListbrowser();
        Layout *hl = rootsLayout->createHorizontalLayout();
        add = hl->createButton("+");
        remove = hl->createButton("-");
        done = hl->createButton("Done");
        // Layout *entryLayout = layout->createLabeledLayout("Entry point");
        // entryLayout->createString(spotless->debugger.getEntryPoint().c_str());
        update();
    }
    bool handleEvent (Event *event) {
        if(event->eventClass() == Event::CLASS_ButtonPress) {
            if(event->elementId() == getAddId()) {
                string newRoot = Requesters::path(Requesters::REQUESTER_MODULE, "Choose path to add to list of source code roots...");
                string unixRoot = Requesters::convertToUnixRelative(newRoot);
                spotless->debugger.addSourceRoot(unixRoot);
                update();
            }
            if(event->elementId() == getRemoveId()) {
                spotless->debugger.removeSourceRoot(listbrowser->getNode(listbrowser->getSelectedLineNumber()));
                update();
            }
            if(event->elementId() == getDoneId()) {                
                return true;
            }
        }
        return false;
    }
    void update() {
        clear();
        list<string> roots = spotless->debugger.getSourceRoots();
        listbrowser->detach();
        for(list<string>::iterator it = roots.begin(); it != roots.end(); it++) {
            listbrowser->addNode(*it);
        }
        listbrowser->attach();
    }
    void clear() {
        listbrowser->clear();
    }
    unsigned int getAddId() {
        return add->getId();
    }
    unsigned int getRemoveId() {
        return remove->getId();
    }
    unsigned int getDoneId() {
        return done->getId();
    }
};
#endif