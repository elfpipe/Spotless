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
    Configure(Spotless *parent) : Widget(0) { setName("Configure"); spotless = parent; }
    void createGuiObject(Layout *layout) {
        Layout *rootsLayout = layout->createLabeledLayout("Source roots");
        listbrowser = rootsLayout->createListbrowser();
        Layout *hl = rootsLayout->createHorizontalLayout();
        add = hl->createButton("+");
        remove = hl->createButton("-");
        done = hl->createButton("Done");
        Layout *entryLayout = layout->createLabeledLayout("Entry point");
        entryLayout->createString(spotless->debugger.getEntryPoint().c_str());
        update();
    }
    bool handleEvent (Event *event) {
        if(event->eventClass() == Event::CLASS_ButtonPress) {
            if(!event->elementDescription().compare("+")) {
                string newRoot = Requesters::path(Requesters::REQUESTER_MODULE, "Choose path to add to list of source code roots...");
                string unixRoot = Requesters::convertToUnixRelative(newRoot);
                spotless->debugger.addSourceRoot(unixRoot);
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