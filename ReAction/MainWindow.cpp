#include "reaction.h"

#include <string.h>

#include "MainWindow.hpp"
#include "Screen.hpp"
#include "Layout.hpp"
#include "Menubar.hpp"

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

	object = WindowObject,
		WA_ScreenTitle,         "Spotless",
		WA_Title,              	"Spotless",
		WA_PubScreen,           PublicScreen::instance()->screenPointer(),
		WA_Top,					PublicScreen::instance()->screenBarHeight(),
		WA_Width,               windowWidth,
		WA_Height,              windowHeight,

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

    mainLayout = new Layout(this, Layout::LAYOUT_Vertical);

    if(topBar) {
        topBar->setParent(0);
        mainLayout->addEmbeddedWidget(topBar);
    }

    if(mainView) {
        mainView->setParent(0);
        mainLayout->addEmbeddedWidget(mainView);
    }

	object = WindowObject,
		WA_ScreenTitle,         "Spotless",
		WA_Title,              	"Spotless",
		WA_PubScreen,           PublicScreen::instance()->screenPointer(),
		WA_Top,					PublicScreen::instance()->screenBarHeight(),
		// WA_Width,               windowWidth,
		// WA_Height,              windowHeight,

        WA_DepthGadget,		    true,
		WA_SizeGadget,		    true,
		WA_DragBar,			    true,
		WA_CloseGadget,			true,
        
		//WA_Backdrop,            backdropWindow,
		//WA_Borderless,          backdropWindow,
		WA_Activate,			TRUE,
		WA_ReportMouse,			TRUE,
				        
		WINDOW_ParentLayout,	mainLayout->systemObject(),
        WINDOW_MenuStrip,       mainMenu ? mainMenu->systemObject() : 0,
        WINDOW_GadgetHelp,      TRUE,
        WINDOW_IconifyGadget,   TRUE,
	  WINDOW_IconTitle, "Spotless",
	   WINDOW_AppPort, appPort,
EndWindow;
	
	if (object) window = (struct Window *) RA_OpenWindow(object); 

	openedWindows.push_back(this);


    if(leftPanel) {
        for(vector<Widget *>::iterator it = leftPanel->widgets.begin(); it != leftPanel->widgets.end(); it++) {
            (*it)->setParent(0);
            openNewWindow(*it);
        }
    }
    if(bottomPanel) {
        for(vector<Widget *>::iterator it = bottomPanel->widgets.begin(); it != bottomPanel->widgets.end(); it++) {
            (*it)->setParent(0);
            openNewWindow(*it);
        }
    }
    if(rightPanel) {
        for(vector<Widget *>::iterator it = rightPanel->widgets.begin(); it != rightPanel->widgets.end(); it++) {
            (*it)->setParent(0);
            openNewWindow(*it);
        }
    }
    split = true;
}

Object *MainWindow::createContent() {
    mainLayout = new Layout(this, Layout::LAYOUT_Vertical);

    if(topBar) {
        topBar->setParent(this);
        mainLayout->addEmbeddedWidget(topBar);
    }

    Layout *layoutA = mainLayout->createHorizontalLayout();

    if (leftPanel && leftPanel->count()) {
        for(vector<Widget *>::iterator it = leftPanel->widgets.begin(); it != leftPanel->widgets.end(); it++)
            (*it)->setParent(this);
        layoutA->addTabbedPanel(leftPanel, 30);
        layoutA->addWeightBar();
    }

    Layout *layoutB = layoutA->createVerticalLayout();

    if(mainView) {
        mainView->setParent(this);
        layoutB->addEmbeddedWidget(mainView);
    }

    if(bottomPanel && bottomPanel->count()) {
        for(vector<Widget *>::iterator it = bottomPanel->widgets.begin(); it != bottomPanel->widgets.end(); it++)
            (*it)->setParent(this);
        layoutB->addWeightBar();
        layoutB->addTabbedPanel(bottomPanel, 30);
    }

    //and last: The right panel added to layoutA
    if (rightPanel && rightPanel->count()) {
        for(vector<Widget *>::iterator it = rightPanel->widgets.begin(); it != rightPanel->widgets.end(); it++)
            (*it)->setParent(this);
        layoutA->addTabbedPanel(rightPanel, 30);
        layoutA->addWeightBar();
    }

    return mainLayout->systemObject();
}

void MainWindow::destroyContent()
{
    delete mainLayout;
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
    if(!leftPanel) leftPanel = new Panel(this);
    leftPanel->addWidget(widget);
}

void MainWindow::addBottomPanelWidget(Widget *widget)
{
    if(!bottomPanel) bottomPanel = new Panel(this);
    bottomPanel->addWidget(widget);
}

void MainWindow::addRightPanelWidget(Widget *widget)
{
    if(!rightPanel) rightPanel = new Panel(this);
    rightPanel->addWidget(widget);
}