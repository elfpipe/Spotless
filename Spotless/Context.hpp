#ifndef SPOTLESS_VARIABLES_H
#define SPOTLESS_VARIABLES_H

#include "../ReAction/classes.h"
#include "Spotless.hpp"

#include "../SimpleDebug/Strings.hpp"

class Context : public Widget {
private:
    Spotless *spotless;
    Listbrowser *listbrowser;

public:
    Context(Spotless *parent) : Widget((Widget *)parent) { setName("Variables"); spotless = parent; }
    void createGuiObject(Layout *layout) {
        listbrowser = layout->createListbrowser();
        listbrowser->setHierachical(true);
    }
    void update() {
        // listbrowser->addNode("Hello", 0, true, 1);
        // listbrowser->addNode("and", 0, false, 2);
        // listbrowser->addNode("welcome", 0, false, 2);
        // listbrowser->addNode("I", 0, true, 1);
        // listbrowser->addNode("am", 0, true, 2);
        // listbrowser->addNode("your grandson", 0, false, 3);
        // listbrowser->addNode("Hierymies", 0, false, 3);
        // listbrowser->addNode("Isn't it awesome?", 0, false, 0);
        clear();
        vector<string> context = spotless->debugger.context();
        int generation = 1;
        for(int i = 0; i < context.size(); i++) {
            astream str(context[i]);
            int nextGeneration = generation;
            if(str.endsWith('{')) nextGeneration++;
            if(str.endsWith('}')) nextGeneration--;
            else listbrowser->addNode(context[i], 0, nextGeneration > generation, generation);
            generation = nextGeneration;
        }
    }
    void clear() {
        listbrowser->clear();
    }
};
#endif