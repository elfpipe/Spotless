#include "reaction.h"

#include <string.h>

#include "mainwindow.h"
#include "screen.h"
#include "layout.h"
#include "menu.h"

/* ----------------------------------------------------- */

ReactionMainWindow::ReactionMainWindow()
    :   topBar(0),
        mainView(0),
        leftPanel(0),
        bottomPanel(0),
        rightPanel(0)
{
    unityState = MAINWINDOW_UNIFIED;
}

ReactionMainWindow::~ReactionMainWindow()
{
}

void ReactionMainWindow::openWindow() {

    if(unityState == MAINWINDOW_SEPARATED)
        closeSeparated();

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

Object *ReactionMainWindow::createContent() {
    ReactionLayout *mainLayout = new ReactionLayout(this, ReactionLayout::LAYOUT_Vertical);

    if(topBar) {
        mainLayout->addEmbeddedWidget(topBar);
    }

    ReactionLayout *layoutA = mainLayout->createHorizontalLayout();

    if (leftPanel && leftPanel->count()) {
        layoutA->addTabbedPanel(leftPanel, 30);
        layoutA->addWeightBar();
    }

    ReactionLayout *layoutB = layoutA->createVerticalLayout();

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

void ReactionMainWindow::setMainView(ReactionWidget *view)
{
    mainView = view;
}

void ReactionMainWindow::setTopBar(ReactionWidget *top)
{
    topBar = top;
}

void ReactionMainWindow::addLeftPanelWidget(ReactionWidget *widget)
{
    if(!leftPanel) leftPanel = new ReactionPanel(this);
    leftPanel->addWidget(widget);
}

void ReactionMainWindow::addBottomPanelWidget(ReactionWidget *widget)
{
    if(!bottomPanel) bottomPanel = new ReactionPanel(this);
    bottomPanel->addWidget(widget);
}

void ReactionMainWindow::addRightPanelWidget(ReactionWidget *widget)
{
    if(!rightPanel) rightPanel = new ReactionPanel(this);
    rightPanel->addWidget(widget);
}

void ReactionMainWindow::openSeparated()
{
    separateWidgets.clear();
    separateWidgets.push_back(topBar);
    separateWidgets.push_back(mainView);
    if(leftPanel)
    for(int i = 0; i < leftPanel->count(); i++) {
        ReactionWidget *w = leftPanel->getWidget(i);
        separateWidgets.push_back(w);
    }
    if(bottomPanel)
    for(int i = 0; i < bottomPanel->count(); i++) {
        ReactionWidget *w = bottomPanel->getWidget(i);
        separateWidgets.push_back(w);
    }
    if(rightPanel)
    for(int i = 0; i < rightPanel->count(); i++) {
        ReactionWidget *w = rightPanel->getWidget(i);
        separateWidgets.push_back(w);
    }
    for(ReactionWidget *w : separateWidgets) {
        w->openWindow();
        if(mainMenu) w->setMenu(mainMenu);
    }
}

void ReactionMainWindow::closeSeparated()
{
    for(ReactionWidget *w : separateWidgets) {
        w->closeWindow();
    }
    separateWidgets.clear();
}

void ReactionMainWindow::waitForClose()
{
    if(unityState == MAINWINDOW_UNIFIED) {
        ReactionWidget::waitForClose();
        return;
    }

    bool close = false;
	
	while (!close) {
        uint32 mask = 0x0;
        for(ReactionWidget *w : separateWidgets)
            mask |= w->windowSignalMask();
		uint32 result = IExec->Wait (mask | SIGBREAKF_CTRL_C);

		if (result & SIGBREAKF_CTRL_C) {
			close = true;
            break;
		}

		while(mask) {
            for(ReactionWidget *w : separateWidgets) {
                if(w->windowSignalMask() & mask) {
                    Object *object = w->windowObject();

            		bool done = false;
                    while(!done) {
    			        uint32 Class;
	            		uint16 Code;
		    	        Class = IIntuition->IDoMethod (object, WM_HANDLEINPUT, &Code);
			            if(Class == WMHI_LASTMSG) {
			            	done = true;
            			} else {
			            	switch (Class & WMHI_CLASSMASK) {
					            case WMHI_CLOSEWINDOW:
						            close = true;
						            break;

                                case IDCMP_MENUPICK: {
                                    uint32 id = NO_MENU_ID;
                                    while ((id = IIntuition->IDoMethod(mainMenu->systemObject(),MM_NEXTSELECT,0,id)) != NO_MENU_ID)
                                    close = mainMenu->handleMenuPick(id);
                                    done = close;
                                }
                                break;

        					    default:
		            				close = w->processEvent(Class, Code);
					            	done = close;
						        break;
				            }
			            }
		            }
                    mask = mask & ~(w->windowSignalMask());
                }
            }
        }
	}
	closeWindow ();
}
