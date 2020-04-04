#include "reaction.h"

#include <string.h>

#include "MainWindow.hpp"
#include "Screen.hpp"
#include "Layout.hpp"
#include "Menubar.hpp"

/* ----------------------------------------------------- */

MainWindow::MainWindow()
    :   topBar(0),
        mainView(0),
        leftPanel(0),
        bottomPanel(0),
        rightPanel(0)
{
}

MainWindow::~MainWindow()
{
}

void MainWindow::openWindow() {
	if(window) {
		IIntuition->CloseWindow(window);
		window = 0;
	}

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
		WA_CloseGadget,			true,
		WA_Backdrop,            backdropWindow,
		//WA_Borderless,          backdropWindow,
		WA_Activate,			TRUE,
		WA_ReportMouse,			TRUE,
				
		WINDOW_ParentLayout,	createContent(),
        WINDOW_MenuStrip,       mainMenu ? mainMenu->systemObject() : 0,
	EndWindow;
	
	if (object) window = (struct Window *) RA_OpenWindow(object); 
}

Object *MainWindow::createContent() {
    Layout *mainLayout = new Layout(this, Layout::LAYOUT_Vertical);

    if(topBar) {
        mainLayout->addEmbeddedWidget(topBar);
    }

    Layout *layoutA = mainLayout->createHorizontalLayout();

    if (leftPanel && leftPanel->count()) {
        layoutA->addTabbedPanel(leftPanel, 30);
        layoutA->addWeightBar();
    }

    Layout *layoutB = layoutA->createVerticalLayout();

    if(mainView) {
        layoutB->addEmbeddedWidget(mainView);
    }

    if(bottomPanel && bottomPanel->count()) {
        layoutB->addWeightBar();
        layoutB->addTabbedPanel(bottomPanel, 30);
    }

    //and last: The right panel added to layoutA
    if (rightPanel && rightPanel->count()) {
        layoutA->addTabbedPanel(rightPanel, 30);
        layoutA->addWeightBar();
    }

    return mainLayout->systemObject();
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