#ifndef SPOTLESS_CONSOLE_H
#define SPOTLESS_CONSOLE_H

#include "../ReAction/classes.h"
#include "Spotless.hpp"

class Console : public Widget {
private:
    static Spotless *spotless;
    static Listbrowser *listbrowser;

    class Line {
        public:
        Line(string text, PublicScreen::PenType pen) {this->text=text;this->pen=pen;}
        string text;
        PublicScreen::PenType pen;
    };

    static vector<Line> buffer;

public:
    Console(Spotless *spotless) : Widget(spotless) { setName("Console"); this->spotless = spotless; }
    void createGuiObject(Layout *layout) {
                layout->setParent(this);

        listbrowser = layout->createListbrowser();
    }
    static void write(PublicScreen::PenType pen, string text) {
        if(!spotless->console->open()) return;

        static int n = 0;
        if(n++ == 100) { clear()  ; n = 0; }

        if(listbrowser) {
            listbrowser->setPen(pen);
            listbrowser->detach();
            listbrowser->addNode(text);
            listbrowser->attach();
        }
        buffer.push_back(Line(text,pen));
    }
    static void clear() {
        if(!spotless->console->open()) return;
        listbrowser->clear();
    }
    static void update() {
        if(!spotless->console->open()) return;
        clear();
        for(vector<Line>::iterator it = buffer.begin(); it != buffer.end(); it++)
            write((*it).pen, (*it).text);
    }
};
#endif