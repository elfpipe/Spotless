#ifndef SPOTLESS_VARIABLES_H
#define SPOTLESS_VARIABLES_H

#include "../ReAction/classes.h"
#include "Spotless.hpp"

#include "../SimpleDebug/Strings.hpp"

class Context : public Widget {
private:
    Spotless *spotless;
    Listbrowser *listbrowser;
    GoButton *button;

public:
    Context(Spotless *parent) : Widget((Widget *)parent) { setName("Variables"); spotless = parent; }
    void createGuiObject(Layout *layout) {
        Layout *vertical = layout->createVerticalLayout();
        listbrowser = vertical->createListbrowser();
        listbrowser->setHierachical(true);
        button = vertical->createButton("Globals");
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
    }
    void globals() {
        listbrowser->addNode("Globals", 0, true, 1);
        add(spotless->debugger.globals(), 2);
    }
    void clear() {
        listbrowser->clear();
    }
};
#endif