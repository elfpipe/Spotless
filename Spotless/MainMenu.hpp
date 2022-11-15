#ifndef SPOTLESS_MENU_H
#define SPOTLESS_MENU_H

#include "../ReAction/classes.h"
#include "Spotless.hpp"
#include "Configure.hpp"
#include "MemorySurfer.hpp"

class MainMenu : public Menubar {
private:
    Spotless *spotless;
    PublicScreen *screen;
    bool usingPublicScreen = false;
    vector<Widget *> widgets;
    vector<Object *> windowItems;
public:
    MainMenu(Spotless *spotless) : Menubar(0) { this->spotless = spotless; }
    void createMenu() {
        createMenuStrip();

        MenuReference panel1 = addCreateMenu("Spotless");

        addCreateMenuItem (panel1, "About", "", 1);
        addCreateMenuItem (panel1, "Switch public screen", "", 2);
        addCreateMenuItem (panel1, "Exit", "", 3);

        MenuReference panel2 = addCreateMenu("Project");

        addCreateMenuItem (panel2, "Configure ...", "", 4);

        MenuReference panel3 = addCreateMenu("Windows");
        addCreateMenuItem (panel3, "Switch split window mode", "", 5);

        widgets.clear();
        windowItems.clear();

        int i = 0;
        if(spotless->isSplit()) {
            widgets = spotless->getAllPanelWidgets();
            for(vector<Widget*>::iterator it = widgets.begin(); it != widgets.end(); it++)
                windowItems.push_back(addCreateMenuItem (panel3, (*it)->name().c_str(), "", 6 + (i++), true, true));
        }
        widgets.push_back(spotless->configure);
        widgets.push_back(spotless->memorySurfer);

        addSeparator(panel3);
        windowItems.push_back(addCreateMenuItem (panel3, "Configure", "", 6 + (i++), true, false));
        windowItems.push_back(addCreateMenuItem( panel3, "Memory Surfer", "", 6 + i, true, false));
        
        created = true;
    }
    void update() {
        if(!created || !spotless->open()) return;
        for(int i = 0; i < windowItems.size(); i++) {
            setSelected(windowItems[i], widgets[i]->open());
        }
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
                    PublicScreen::instance()->openPublicScreen("Spotless", "Spotless - Copyright (c) 2020, 2022 by Alfkil Thorbjoern Wennermark");
                else
                    PublicScreen::instance()->closePublicScreen();
                if(!spotless->isSplit()) {
                    spotless->openWindow();
                } else {
                    spotless->showSplit();
                }
                spotless->sources->update();
                spotless->code->update();
                spotless->updateAll();
                *openClose = true;
                break;
            case 3:
                *openClose = true;
                *exit = true;
                break;
            case 4:
                if(spotless->configure) {
                    spotless->openExtraWindow(spotless->configure);
                    spotless->configure->update();
                }
                break;
            case 5:
                if(spotless->isSplit()) {
                    spotless->openWindow();
                } else {
                    spotless->showSplit();
                }
                spotless->sources->update();
                spotless->code->update();
                spotless->updateAll();
                *openClose = true;
                break;
            default: {
                if(id > 5+windowItems.size()-2) {
                    if(isSelected(windowItems[id-6]))
                        spotless->openExtraWindow(widgets[id-6]);
                    else
                        spotless->closeExtraWindow(widgets[id-6]);
                } else {
                    if(isSelected(windowItems[id-6]))
                        widgets[id-6]->openWindow();
                    else
                        widgets[id-6]->closeWindow();
                }
                spotless->sources->update();
                spotless->code->update();
                spotless->updateAll();
                *openClose = true;
            }
            break;
        }
        return done;
    }
    bool getWindowSelected(string name, bool def) {
        for(int i = 0; i < windowItems.size(); i++) {
            if(!name.compare(widgets[i]->name())) {
                return isSelected(windowItems[i]);
            }
        }
        return def;
    }
    void setWindowSelected(string name, bool value) {
        for(int i = 0; i < windowItems.size(); i++) {
            if(!name.compare(widgets[i]->name())) {
                setSelected(windowItems[i], value);
            }
        }
    }
};
#endif