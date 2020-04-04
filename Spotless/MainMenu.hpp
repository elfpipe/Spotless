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
        MenuReference panel1 = addCreateMenu ("Panel 1");
        MenuReference panel2 = addCreateMenu ("Panel 2");

        addCreateMenuItem (panel1, "Item 1", "CTRL+1", 1);
        addCreateMenuItem (panel2, "Item 2", "CTRL+2", 2);
        addCreateMenuItem (panel2, "Item 2", "CTRL+3", 3);
    }
};
#endif