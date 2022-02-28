#include "../Listbrowser.hpp"
#include "../MainWindow.hpp"
#include "../Requester.hpp"
#include "../Progress.hpp"
#include "../Widget.hpp"
#include "../Layout.hpp"
#include "../Screen.hpp"
#include "../Panel.hpp"
#include "../Event.hpp"
#include "../Menubar.hpp"
#include "../Button.hpp"
#include "../Speedbar.hpp"
#include "../../SimpleDebug/Strings.hpp"
#include "../../SimpleDebug/Strings.cpp"
#include <iostream>
#include <unistd.h>
using namespace std;

#include "classes.h"

class Window1 : public Widget {
public:
    Window1() : Widget() {}
    void createGuiObject(Layout *layout) {
        layout->createButton("PublicScreen1");
        layout->createButton("Requesters1-5");
        layout->createButton("Listbrowser1");
        layout->createButton("Listbrowser2");
        layout->createButton("Listbrowser3");
        layout->createButton("Speedbar1");
        layout->createButton("Progress1");
        layout->createButton("Panel1");
        layout->createButton("Layout1");
        layout->createButton("Menu1");
        layout->createButton("MainWindow1");
    }

    void progress() {
        ProgressWindow progress;
        progress.open("Time is ticking like a clock", 100, 0);
        for (int i = 0; i < 100; i += 10) {
            progress.updateLevel(i);
            sleep(1);
        }
        progress.close();
    }

    void publicScreen() {
        closeWindow();
        if (!PublicScreen::usingPublicScreen())
            PublicScreen::instance()->openPublicScreen("Spotless", "Spotless - Copyright © 2020 by Alpha Kilimanjaro");
        else
            PublicScreen::instance()->closePublicScreen();
        openWindow();
    }

    void runRequesters() {
        string pathResult;

        Requesters reqTools;
        string result = reqTools.file (Requesters::REQUESTER_EXECUTABLE, "hello.exe", pathResult, "Select an executable (%s, %d)..", "from the list", 12345);
        cout << "(requesterFile) Selected file: " << result << " Path result: " << pathResult << "\n";

        result = reqTools.path (Requesters::REQUESTER_EXECUTABLE, "Select a path... (%s, %d)", "from the path list", 12345);
        cout << "(requesterPath) Selected path: " << result << "\n";

        int choice =  reqTools.choice ("Make a choice", "Yes|No|Don't know", "Are you a fan of %s?", "fishslapping");
        cout << "(requesterChoice) Result: " << choice << "\n";

        result = reqTools.requestString ("Please enter a string", "What is your favorite %s?", "food");
        cout << "(requesterString) Result: " << result << "\n";

        reqTools.showAboutWindow();
    }

    bool handleEvent(Event *event) {
        if(event->eventClass() == Event::CLASS_ButtonPress) {
            closeWindow();
            Widget *widget = 0;
            if(!event->elementDescription().compare("Listbrowser1"))
                widget = new Listbrowser1;
            if(!event->elementDescription().compare("Listbrowser2"))
                widget = new Listbrowser2;
            if(!event->elementDescription().compare("Listbrowser3"))
                widget = new Listbrowser3;
            if(!event->elementDescription().compare("MainWindow1"))
                widget = new MainWindow1;
            if(!event->elementDescription().compare("Speedbar1"))
                widget = new Speedbar1;
            if(!event->elementDescription().compare("Layout1"))
                widget = new Layout1;
            if(!event->elementDescription().compare("Panel1"))
                widget = new Panel1;
            if(!event->elementDescription().compare("Menu1"))
                widget = new Menu1;
            if(!event->elementDescription().compare("Requesters1-5"))
                runRequesters();
            if(!event->elementDescription().compare("PublicScreen1"))
                publicScreen();
            if(!event->elementDescription().compare("Progress1"))
                progress();
            if(widget) {
                widget->openWindow();
                widget->waitForClose();
            }
            openWindow();
        }
        return false;
    }
};

int main() {
    cout << "ReActionTests.\n";
    Window1 window1;
    window1.openWindow();
    window1.waitForClose();

    cout << "All OK.\n";
    return 0;
}
