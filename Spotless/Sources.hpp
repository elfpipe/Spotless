#ifndef SPOTLESS_SOURCES_H
#define SPOTLESS_SOURCES_H

#include "../ReAction/classes.h"
#include "Spotless.hpp"

#include <string>

using namespace std;

class Sources : public Widget {
private:
    Spotless *spotless;
    Listbrowser *listbrowser;
public:
    Sources(Spotless *parent) : Widget((Widget *)parent) { setName("Source files"); spotless = parent; }
    void createGuiObject(Layout *layout) {
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
};
#endif