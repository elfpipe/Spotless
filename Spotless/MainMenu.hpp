#ifndef SPOTLESS_MENU_H
#define SPOTLESS_MENU_H

#include "../ReAction/classes.h"
#include "Spotless.hpp"

class MainMenu : public Menubar {
private:
    Spotless *spotless;
    PublicScreen *screen;
    bool usingPublicScreen = false;
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
                Requesters::showAboutWindow();
                break;
            case 2: //switch public screen
                spotless->closeWindow();
                if(usingPublicScreen) {
                    screen->closePublicScreen();
                    usingPublicScreen = false;
                } else {
                    screen->openPublicScreen("Spotless", "Spotless"); //name, title
                }
                spotless->openWindow();
                spotless->updateAll();
                break;
        }
        return false;
    }
};
#endif