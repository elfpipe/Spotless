#ifndef SPOTLESS_MENU_H
#define SPOTLESS_MENU_H

#include "../ReAction/classes.h"
#include "Spotless.hpp"
#include "Configure.hpp"

class MainMenu : public Menubar {
private:
    Spotless *spotless;
    PublicScreen *screen;
    bool usingPublicScreen = false;
public:
    MainMenu(Spotless *spotless) : Menubar(0) { this->spotless = spotless; }
    void createMenu() {
        MenuReference panel1 = addCreateMenu("Spotless");

        addCreateMenuItem (panel1, "About", "", 1);
        addCreateMenuItem (panel1, "Switch public screen", "", 2);
        addCreateMenuItem (panel1, "Exit", "", 3);

        MenuReference panel2 = addCreateMenu("Project");

        addCreateMenuItem (panel2, "Configure ...", "", 4);
    }
    bool handleMenuPick(int id, bool *closeAll) {
        bool done = false;
        switch(id) {
            case 1: //show about
                Requesters::showAboutWindow();
                break;
            case 2: //switch public screen
                spotless->closeAllWindows();
                if (!PublicScreen::usingPublicScreen())
                    PublicScreen::instance()->openPublicScreen("Spotless", "Spotless - Copyright © 2020, 2022 by Alpha Kilimanjaro");
                else
                    PublicScreen::instance()->closePublicScreen();
                spotless->openWindow();
                spotless->updateAll();
                done = true;
                // this->doScreenSwitch = true;
                break;
            case 3:
                *closeAll = true;
                break;
            case 4: {
                if(spotless->configure) {
                    spotless->openNewWindow((Widget *)spotless->configure);
                }
            } break;
        }
        return done;
    }
};
#endif