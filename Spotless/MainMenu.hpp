#ifndef SPOTLESS_MENU_H
#define SPOTLESS_MENU_H

#include "../ReAction/classes.h"
#include "Spotless.hpp"

class MainMenu : public Menubar {
private:
    Spotless *spotless;
public:
    MainMenu(Spotless *parent) : Menubar(dynamic_cast<Widget *>(parent)) { spotless = parent; }
    void createMenu() {
        MenuReference panel1 = addCreateMenu("Spotless");

        addCreateMenuItem (panel1, "About", "", 1);
        addCreateMenuItem (panel1, "Switch public screen", "", 2);
    }
    bool handleMenuPick(int id) {
        switch(id) {
            case 1: //show about
            case 2: //switch public screen
        }
    }
};
#endif