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

        MenuReference panel3 = addCreateMenu("Windows");
        addCreateMenuItem (panel3, "Switch split window mode", "", 5);

        created = true;
    }
    bool handleMenuPick(int id, bool *openClose, bool *exit) {
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
                spotless->sources->update();
                spotless->code->update();
                spotless->updateAll();
                *openClose = true;
                break;
            case 3:
                *openClose = true;
                *exit = true;
                break;
            case 4: {
                if(spotless->configure) {
                    spotless->openNewWindow((Widget *)spotless->configure);
                }
            } break;
            case 5:
                // cout << "isSplit() : " << spotless->isSplit();
                if(spotless->isSplit()) {
                    spotless->openWindow();
                } else {
                    spotless->showSplit();
                }
                *openClose = true;
                break;
        }
        return done;
    }
};
#endif