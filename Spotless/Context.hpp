#ifndef SPOTLESS_VARIABLES_H
#define SPOTLESS_VARIABLES_H

#include "../ReAction/classes.h"
#include "Spotless.hpp"

#include "../SimpleDebug/Strings.hpp"

class Context : public Widget {
private:
    Spotless *spotless;
    Listbrowser *listbrowser;
    Checkbox *globalsCheckbox;

    bool showGlobals;
public:
    Context(Spotless *parent) : Widget((Widget *)parent), showGlobals(false) { setName("Variables"); spotless = parent; }
    void createGuiObject(Layout *layout) {
        Layout *vertical = layout->createVerticalLayout();
        listbrowser = vertical->createListbrowser();
        listbrowser->setHierachical(true);
        globalsCheckbox = vertical->createCheckbox("Show globals", showGlobals);
    }
    void add(vector<string> context, int generation) {
        for(int i = 0; i < context.size(); i++) {
            astream str(context[i]);
            int nextGeneration = generation;
            if(str.endsWith('{')) nextGeneration++;
            if(str.endsWith('}')) nextGeneration--;
            else listbrowser->addNode(context[i], 0, nextGeneration > generation, generation);
            generation = nextGeneration;
        }
    }
    void update() {
        clear();
        add(spotless->debugger.context(), 1);
        if(showGlobals) add(spotless->debugger.globals(), 1);
    }
    void globals() {
        showGlobals = globalsCheckbox->getChecked();
        cout << "showGlobals : " << (showGlobals ? "true" : "false") << "\n";
        // listbrowser->addNode("Globals", 0, true, 1);
        // add(spotless->debugger.globals(), 2);
        update();
    }
    void clear() {
        listbrowser->clear();
    }
    unsigned int getGlobalsId() {
        return globalsCheckbox->getId();
    }
};
#endif