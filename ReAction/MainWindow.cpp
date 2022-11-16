#include "reaction.h"

#include <string.h>

#include "MainWindow.hpp"
#include "Screen.hpp"
#include "Layout.hpp"
#include "Menubar.hpp"
#include "Config.hpp"

#include <iostream>
using namespace std;
/* ----------------------------------------------------- */

bool MainWindow::split = false;

MainWindow::MainWindow()
    :   Widget(),
        topBar(0),
        mainView(0),
        leftPanel(0),
        bottomPanel(0),
        rightPanel(0)
        // ,split(false)
{
    appPort = (struct MsgPort *)IExec->AllocSysObjectTags(ASOT_PORT, TAG_DONE);
}

MainWindow::~MainWindow()
{
    if(appPort) IExec->FreeSysObject(ASOT_PORT, appPort);
}

bool MainWindow::openWindow() {
    closeAllWindows();

	Config config("config.prefs");
    setName("Spotless");

    int windowWidth = PublicScreen::instance()->screenWidth();
    int windowHeight = PublicScreen::instance()->screenHeight() - PublicScreen::instance()->screenBarHeight();

	bool backdropWindow = PublicScreen::usingPublicScreen();

    split = false;

    if(mainMenu) mainMenu->createMenu();

	object = WindowObject,
		WA_ScreenTitle,         "Spotless",
		WA_Title,              	"Spotless",
		WA_PubScreen,           PublicScreen::instance()->screenPointer(),

		WA_Top,				config.getValue(widgetName, "Top", PublicScreen::instance()->screenBarHeight()),
		WA_Left,			config.getValue(widgetName, "Left", 0),
		WA_Width,			config.getValue(widgetName, "Width", windowWidth),
		WA_Height,			config.getValue(widgetName, "Height", windowHeight),

		// WA_Top,				    PublicScreen::instance()->screenBarHeight(),
		// WA_Width,			    windowWidth,
		// WA_Height,			    windowHeight,

        WA_DepthGadget,		    true,
		WA_SizeGadget,		    true,
		WA_DragBar,			    true,
		WA_CloseGadget,			true,
        
		// WA_Backdrop,            backdropWindow,
		// WA_Borderless,          backdropWindow,
		WA_Activate,			TRUE,
		WA_ReportMouse,			TRUE,
				        
		WINDOW_ParentLayout,	createContent(),
        WINDOW_MenuStrip,       mainMenu ? mainMenu->systemObject() : 0,
        WINDOW_GadgetHelp,      TRUE,
        WINDOW_IconifyGadget,   TRUE,
	  WINDOW_IconTitle, "Spotless",
	   WINDOW_AppPort, appPort,
EndWindow;
	
	if (object) window = (struct Window *) RA_OpenWindow(object); 

    // open extra windows again :
    vector<Widget *> allWidgets;
    vector<bool> windowSelected;

    for(list<Widget *>::iterator it = extraWindows.begin(); it != extraWindows.end(); it++)
        allWidgets.push_back(*it);
    {
        Config config("config.prefs");
        for(int i = 0; i < allWidgets.size(); i++) {
            bool result = config.getBool(allWidgets[i]->name(), "Window selected", false);
            windowSelected.push_back(result);
            mainMenu->setWindowSelected(allWidgets[i]->name(), result);
        }
    }

    for(int i = 0; i < allWidgets.size(); i++)
        if(windowSelected[i]) {
            allWidgets[i]->setParent(0);
            allWidgets[i]->setMenubar(mainMenu);
            allWidgets[i]->openWindow();
        }


    if(window) isOpen = true;

    // mainMenu->update();

	openedWindows.push_back(this);

    return window != 0;
}

bool MainWindow::showSplit()
{
    closeAllWindows();
    window = 0;
    object = 0;

    split = true;

    if(mainMenu) mainMenu->createMenu();

    vector<Widget *> allWidgets;
    for(list<Widget *>::iterator it = leftPanel.begin(); it != leftPanel.end(); it++)
        allWidgets.push_back(*it);
    for(list<Widget *>::iterator it = bottomPanel.begin(); it != bottomPanel.end(); it++)
        allWidgets.push_back(*it);
    for(list<Widget *>::iterator it = rightPanel.begin(); it != rightPanel.end(); it++)
        allWidgets.push_back(*it);
    for(list<Widget *>::iterator it = extraWindows.begin(); it != extraWindows.end(); it++)
        allWidgets.push_back(*it);

    vector<bool> windowSelected;
    {
        Config config("config.prefs");
        for(int i = 0; i < allWidgets.size(); i++) {
            bool result = config.getBool(allWidgets[i]->name(), "Window selected", i >= allWidgets.size() - extraWindows.size() ? false :true);
            windowSelected.push_back(result);
            mainMenu->setWindowSelected(allWidgets[i]->name(), result);
        }
    }

    for(int i = 0; i < allWidgets.size(); i++) {
        if(windowSelected[i]) {
            allWidgets[i]->setParent(0);
            allWidgets[i]->setMenubar(mainMenu);
            allWidgets[i]->openWindow();
        }
    }

    // allWidgets.clear();
    // windowSelected.clear();
    // for(list<Widget *>::iterator it = extraWindows.begin(); it != extraWindows.end(); it++)
    //     allWidgets.push_back(*it);
    // {
    //     Config config("config.prefs");
    //     for(int i = 0; i < allWidgets.size(); i++) {
    //         bool result = config.getBool(allWidgets[i]->name(), "Window selected", false);
    //         windowSelected.push_back(result);
    //         mainMenu->setWindowSelected(allWidgets[i]->name(), result);
    //     }
    // }
    // for(int i = 0; i < allWidgets.size(); i++)
    //     if(windowSelected[i]) {
    //         allWidgets[i]->setParent(0);
    //         allWidgets[i]->setMenubar(mainMenu);
    //         allWidgets[i]->openWindow();
    //     }

    // if(leftPanel.size()) {
    //     for(list<Widget *>::iterator it = leftPanel.begin(); it != leftPanel.end(); it++) {
    //         (*it)->setParent(0);
    //         (*it)->setMenubar(mainMenu);
    //         bool windowSelected;
    //         {
    //             Config config("config.prefs");
    //             windowSelected = config.getBool((*it)->name(), "Window selected", true);
    //         }
    //         if(windowSelected)
    //             (*it)->openWindow();
    //         mainMenu->setWindowSelected((*it)->name(), windowSelected);
    //     }
    // }
    // if(bottomPanel.size()) {
    //     for(list<Widget *>::iterator it = bottomPanel.begin(); it != bottomPanel.end(); it++) {
    //         (*it)->setParent(0);
    //         (*it)->setMenubar(mainMenu);
    //         bool windowSelected;
    //         {
    //             Config config("config.prefs");
    //             windowSelected = config.getBool((*it)->name(), "Window selected", true);
    //         }
    //         if(windowSelected)
    //             (*it)->openWindow();
    //         mainMenu->setWindowSelected((*it)->name(), windowSelected);
    //     }
    // }
    // if(rightPanel.size()) {
    //     for(list<Widget *>::iterator it = rightPanel.begin(); it != rightPanel.end(); it++) {
    //         (*it)->setParent(0);
    //         (*it)->setMenubar(mainMenu);
    //         bool windowSelected;
    //         {
    //             Config config("config.prefs");
    //             windowSelected = config.getBool((*it)->name(), "Window selected", true);
    //         }
    //         if(windowSelected)
    //             (*it)->openWindow();
    //         mainMenu->setWindowSelected((*it)->name(), windowSelected);
    //     }
    // }

    parentLayout = new Layout(this, Layout::LAYOUT_Vertical);

    if(topBar) {
        topBar->setParent(this);
        parentLayout->addEmbeddedWidget(topBar);
        topBar->setOpen(true);
    }

    if(mainView) {
        mainView->setParent(this);
        parentLayout->addEmbeddedWidget(mainView);
        mainView->setOpen(true);
    }

    setName("SpotlessMini");
    Config config("config.prefs");

    int windowWidth = PublicScreen::instance()->screenWidth();
    int windowHeight = PublicScreen::instance()->screenHeight() - PublicScreen::instance()->screenBarHeight();

	object = WindowObject,
		WA_ScreenTitle,         "Spotless",
		WA_Title,              	"Spotless",
		WA_PubScreen,           PublicScreen::instance()->screenPointer(),

		WA_Top,				config.getValue(widgetName, "Top", PublicScreen::instance()->screenBarHeight()),
		WA_Left,			config.getValue(widgetName, "Left", 0),
		WA_Width,			config.getValue(widgetName, "Width", windowWidth),
		WA_Height,			config.getValue(widgetName, "Height", windowHeight),

		// WA_Top,				    PublicScreen::instance()->screenBarHeight(),
		// WA_Width,			    windowWidth,
		// WA_Height,			    windowHeight,

        WA_DepthGadget,		    true,
		WA_SizeGadget,		    true,
		WA_DragBar,			    true,
		WA_CloseGadget,			true,
        
		//WA_Backdrop,            backdropWindow,
		//WA_Borderless,          backdropWindow,
		WA_Activate,			TRUE,
		WA_ReportMouse,			TRUE,
				        
		WINDOW_ParentLayout,	parentLayout->systemObject(),
        WINDOW_MenuStrip,       mainMenu ? mainMenu->systemObject() : 0,
        WINDOW_GadgetHelp,      TRUE,
        WINDOW_IconifyGadget,   TRUE,
	  WINDOW_IconTitle, "Spotless",
	   WINDOW_AppPort, appPort,
EndWindow;
	
	if (object) window = (struct Window *) RA_OpenWindow(object); 

    // open extra windows again :
    // for(list<Widget *>::iterator it = extraWindows.begin(); it != extraWindows.end(); it++)
    //     (*it)->openWindow();

    if(window) isOpen = true;
	openedWindows.push_back(this);

    // mainMenu->update();

    return window != 0;
}

Object *MainWindow::createContent() {
    parentLayout = new Layout(this, Layout::LAYOUT_Vertical);

    if(topBar) {
        topBar->setParent(this);
        parentLayout->addEmbeddedWidget(topBar);
        topBar->setOpen(true);
    }

    Layout *layoutA = parentLayout->createHorizontalLayout();

    if (leftPanel.size()) {
        for(list<Widget *>::iterator it = leftPanel.begin(); it != leftPanel.end(); it++) {
            (*it)->setOpen(true);
            (*it)->setParent(this);
        }
        // layoutA->addTabbedPanel(leftPanel, 30);
        layoutA->createTabbedPanel(this, leftPanel, 30);
        layoutA->addWeightBar();
    }

    Layout *layoutB = layoutA->createVerticalLayout();

    if(mainView) {
        mainView->setParent(this);
        layoutB->addEmbeddedWidget(mainView);
        mainView->setOpen(true);
    }

    if(bottomPanel.size()) {
        for(list<Widget *>::iterator it = bottomPanel.begin(); it != bottomPanel.end(); it++) {
            (*it)->setOpen(true);
            (*it)->setParent(this);
        }
        layoutB->addWeightBar();
        layoutB->createTabbedPanel(this, bottomPanel, 30);
    }

    //and last: The right panel added to layoutA
    if (rightPanel.size()) {
        for(list<Widget *>::iterator it = rightPanel.begin(); it != rightPanel.end(); it++) {
            (*it)->setOpen(true);
            (*it)->setParent(this);
        }
        layoutA->createTabbedPanel(this, rightPanel, 30);
        layoutA->addWeightBar();
    }

    return parentLayout->systemObject();
}

void MainWindow::closeWindow()
{
    {
        Config config("config.prefs");
        config.setBool("Split mode", split);

        for(list<Widget *>::iterator it = leftPanel.begin(); it != leftPanel.end(); it++)
            config.setBool((*it)->name(), "Window selected", mainMenu->getWindowSelected((*it)->name(), true));
        for(list<Widget *>::iterator it = bottomPanel.begin(); it != bottomPanel.end(); it++)
            config.setBool((*it)->name(), "Window selected", mainMenu->getWindowSelected((*it)->name(), true));
        for(list<Widget *>::iterator it = rightPanel.begin(); it != rightPanel.end(); it++)
            config.setBool((*it)->name(), "Window selected", mainMenu->getWindowSelected((*it)->name(), true));
        for(list<Widget *>::iterator it = extraWindows.begin(); it != extraWindows.end(); it++)
            config.setBool((*it)->name(), "Window selected", mainMenu->getWindowSelected((*it)->name(), false));
    }
    // to not interfere with below:
    
    Widget::closeWindow();
}

// bool MainWindow::openExtraWindow(Widget *widget)
// {
//     widget->setMenubar(mainMenu);
//  	if(widget->openWindow()) {
// 		extraWindows.push_back(widget);
// 		return true;
// 	}
// 	return false;
// }
   
// bool MainWindow::closeExtraWindow(Widget *widget)
// {
//     for(list<Widget *>::iterator it = extraWindows.begin(); it != extraWindows.end(); it++) {
//         if ((*it) == widget) {
//             widget->closeWindow();
//             extraWindows.remove(widget);
//             return true;
//         }
//     }
//     return false;
// }

void MainWindow::destroyContent()
{
    if(parentLayout) delete parentLayout;
    parentLayout = 0;
}

void MainWindow::setMainView(Widget *view)
{
    mainView = view;
}

void MainWindow::setTopBar(Widget *top)
{
    topBar = top;
}

void MainWindow::addLeftPanelWidget(Widget *widget)
{
    leftPanel.push_back(widget);
    // if(!leftPanel) leftPanel = new Panel(this);
    // leftPanel->addWidget(widget);
}

void MainWindow::addBottomPanelWidget(Widget *widget)
{
    bottomPanel.push_back(widget);
    // if(!bottomPanel) bottomPanel = new Panel(this);
    // bottomPanel->addWidget(widget);
}

void MainWindow::addRightPanelWidget(Widget *widget)
{
    rightPanel.push_back(widget);
    // if(!rightPanel) rightPanel = new Panel(this);
    // rightPanel->addWidget(widget);
}

void MainWindow::addExtraWidget(Widget *widget)
{
    extraWindows.push_back(widget);
    widget->setMenubar(mainMenu);
}