#ifndef SPOTLESS_CONSOLE_H
#define SPOTLESS_CONSOLE_H

#include "../ReAction/classes.h"
#include "Spotless.hpp"

class Console : public Widget {
private:
    Spotless *spotless;
    Listbrowser *listbrowser;

    class Line {
        public:
        Line(string text, PublicScreen::PenType pen) {this->text=text;this->pen=pen;}
        string text;
        PublicScreen::PenType pen;
    };

    vector<Line *> buffer;

public:
    Console(Spotless *spotless) : Widget() { setName("Console"); this->spotless = spotless; }
    void createGuiObject(Layout *layout) {
                layout->setParent(this);
        listbrowser = layout->createListbrowser();
    }
    void write(PublicScreen::PenType pen, string text) {
        if(!spotless->console->open()) return;

        static int n = 0;
        if(n++ == 100) { clear()  ; n = 0; }

        if(listbrowser) {
            listbrowser->setPen(pen);
            listbrowser->detach();
            listbrowser->addNode(text);
            listbrowser->attach();
        }
        buffer.push_back(new Line(text,pen));
    }
    void clear() {
        if(!spotless->console->open()) return;
        listbrowser->clear();
        for(vector<Line *>::iterator it = buffer.begin(); it != buffer.end(); it++)
            delete *it;
        buffer.clear();
    }
    void update() {
        if(!spotless->console->open()) return;
        listbrowser->clear();
        for(vector<Line *>::iterator it = buffer.begin(); it != buffer.end(); it++) {
            listbrowser->setPen((*it)->pen);
            listbrowser->detach();
            listbrowser->addNode((*it)->text);
            listbrowser->attach();
        }
    }
};
#endif