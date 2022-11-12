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

MainWindow::MainWindow()
    :   Widget(0),
        topBar(0),
        mainView(0),
        leftPanel(0),
        bottomPanel(0),
        rightPanel(0),
        split(false)
{
    appPort = (struct MsgPort *)IExec->AllocSysObjectTags(ASOT_PORT, TAG_DONE);
}

MainWindow::~MainWindow()
{
    if(appPort) IExec->FreeSysObject(ASOT_PORT, appPort);
}

bool MainWindow::openWindow() {
    closeAllWindows();

    int windowWidth = PublicScreen::instance()->screenWidth();
    int windowHeight = PublicScreen::instance()->screenHeight() - PublicScreen::instance()->screenBarHeight();

	bool backdropWindow = PublicScreen::usingPublicScreen();

	Config config("config.prefs");
    setName("Spotless");

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

	openedWindows.push_back(this);
    split = false;

    return window != 0;
}

void MainWindow::showSplit()
{
    closeAllWindows();
    window = 0;
    object = 0;

    parentLayout = new Layout(this, Layout::LAYOUT_Vertical);

    if(topBar) {
        topBar->setParent(0);
        parentLayout->addEmbeddedWidget(topBar);
    }

    if(mainView) {
        mainView->setParent(0);
        parentLayout->addEmbeddedWidget(mainView);
    }

    Config config("config.prefs");
    setName("SpotlessMini");

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

	openedWindows.push_back(this);


    if(leftPanel.size()) {
        for(list<Widget *>::iterator it = leftPanel.begin(); it != leftPanel.end(); it++) {
            (*it)->setParent(0);
            openNewWindow(*it);
        }
    }
    if(bottomPanel.size()) {
        for(list<Widget *>::iterator it = bottomPanel.begin(); it != bottomPanel.end(); it++) {
            (*it)->setParent(0);
            openNewWindow(*it);
        }
    }
    if(rightPanel.size()) {
        for(list<Widget *>::iterator it = rightPanel.begin(); it != rightPanel.end(); it++) {
            (*it)->setParent(0);
            openNewWindow(*it);
        }
    }
    split = true;
}

Object *MainWindow::createContent() {
    parentLayout = new Layout(this, Layout::LAYOUT_Vertical);

    if(topBar) {
        topBar->setParent(this);
        parentLayout->addEmbeddedWidget(topBar);
    }

    Layout *layoutA = parentLayout->createHorizontalLayout();

    if (leftPanel.size()) {
        for(list<Widget *>::iterator it = leftPanel.begin(); it != leftPanel.end(); it++)
            (*it)->setParent(this);
        // layoutA->addTabbedPanel(leftPanel, 30);
        layoutA->createTabbedPanel(this, leftPanel, 30);
        layoutA->addWeightBar();
    }

    Layout *layoutB = layoutA->createVerticalLayout();

    if(mainView) {
        mainView->setParent(this);
        layoutB->addEmbeddedWidget(mainView);
    }

    if(bottomPanel.size()) {
        for(list<Widget *>::iterator it = bottomPanel.begin(); it != bottomPanel.end(); it++)
            (*it)->setParent(this);
        layoutB->addWeightBar();
        layoutB->createTabbedPanel(this, bottomPanel, 30);
    }

    //and last: The right panel added to layoutA
    if (rightPanel.size()) {
        for(list<Widget *>::iterator it = rightPanel.begin(); it != rightPanel.end(); it++)
            (*it)->setParent(this);
        layoutA->createTabbedPanel(this, rightPanel, 30);
        layoutA->addWeightBar();
    }

    return parentLayout->systemObject();
}

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