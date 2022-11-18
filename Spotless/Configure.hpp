#ifndef SPOTLESS_SOURCEROOTS_H
#define SPOTLESS_SOURCEROOTS_H

#include "../ReAction/classes.h"
#include "Spotless.hpp"
#include "../ReAction/Config.hpp"
#include "../SimpleDebug/TextFile.hpp"
#include "../ReAction/Requester.hpp"

#include <string>

using namespace std;

class Configure: public Widget {
private:
    Spotless *spotless;
    Listbrowser *listbrowser;
    RButton *add, *remove, *done;
    // Checkbox *arguments;

// state:
    // bool ask;
    string configFile;

public:
    Configure(Spotless *spotless) : Widget(0), /*ask(false),*/ configFile("spotless.conf") { setName("Configure"); this->spotless = spotless; }
    void createGuiObject(Layout *layout) {
        Layout *rootsLayout = layout->createLabeledLayout("Source roots");
        listbrowser = rootsLayout->createListbrowser();
        Layout *hl = rootsLayout->createHorizontalLayout();
        add = hl->createButton("+");
        remove = hl->createButton("-");
        done = hl->createButton("Done");

        // Layout *entryLayout = layout->createLabeledLayout("Entry");
        // arguments = entryLayout->createCheckbox("Ask for arguments", false);

        setOpen(true);
        update();
    }
    //     bool askArguments() {
    //     return ask;
    // }
    bool openConfig(string file) {
        configFile = Requesters::convertToAmigaRelative(file);
        {
            TextFile f(file);
            if(!f.exists()) return false;
        }
        Config config(configFile);
        vector<string> roots = config.getArray("Roots");
        spotless->debugger.clearRoots();
        for(vector<string>::iterator it = roots.begin(); it != roots.end(); it++)
            spotless->debugger.addSourceRoot(*it);
        // ask = config.getBool("Ask for arguments", false);
        return true;
    }
    void saveConfig() {
        if(!configFile.size()) return;
        Config config(configFile);
        list<string> roots = spotless->debugger.getSourceRoots();
        vector<string> _roots;
        for(list<string>::iterator it = roots.begin(); it != roots.end(); it++)
            _roots.push_back(*it);
        config.setArray("Roots", _roots);
        // config.setBool("Ask for arguments", ask);
    }
    bool handleEvent (Event *event, bool *exit) {
        if(event->eventClass() == Event::CLASS_ButtonPress) {
            if(event->elementId() == getAddId()) {
                string newRoot = Requesters::path(Requesters::REQUESTER_MODULE, "Choose path to add to list of source code roots...");
                if(newRoot.size()) {
                    string unixRoot = Requesters::convertToUnixRelative(newRoot);
                    spotless->debugger.addSourceRoot(unixRoot);
                    update();
                    saveConfig();
                }
            }
            if(event->elementId() == getRemoveId()) {
                spotless->debugger.removeSourceRoot(listbrowser->getNode(listbrowser->getSelectedLineNumber()));
                update();
                saveConfig();
            }
            if(event->elementId() == getDoneId()) {                
                // spotless->closeExtraWindow(this);
                closeWindow();
                saveConfig();
                return true;
                // *exit = true;
            }
        }
        // if(event->eventClass() == Event::CLASS_CheckboxPress) {
        //     ask = arguments->getChecked();
        //     saveConfig();
        // }
        return false;
    }
    void update() {
        if(!open()) return;
        listbrowser->clear();
        list<string> roots = spotless->debugger.getSourceRoots();
        listbrowser->detach();
        for(list<string>::iterator it = roots.begin(); it != roots.end(); it++) {
            listbrowser->addNode(*it);
        }
        listbrowser->attach();
        // arguments->setChecked(ask);
    }
    void clear() {
        saveConfig();
        configFile = string();
        if(!open()) return;
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