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
        vector<string> context = spotless->debugger.context();
        int generation = 1;
        for(int i = 0; i < context.size(); i++) {
            astream str(context[i]);
            int nextGeneration = generation;
            if(str.endsWith('{')) nextGeneration++;
            if(str.endsWith('}')) nextGeneration--;
            else {
                vector<string> data;
                data.push_back(context[i]);
                listbrowser->addNode(data, 0, nextGeneration > generation, generation);
                generation = nextGeneration;
            }
        }
    }
    void clear() {
        listbrowser->clear();
    }
};
#endif