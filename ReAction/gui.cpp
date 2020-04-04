#include "gui.h"
#include "menu.h"
#include "layout.h"
#include "screen.h"
#include "reaction.h"

#include "Stage/surfaces.hpp"

#define MAX(x,y) ((x)>(y)?(x):(y))

ReactionGui::ReactionGui() :
	_windowPointer(0),
	menu(0)
{
}

ReactionGui::~ReactionGui()
{
}

void mainIDCMPHookFunction (struct Hook *hook, Object *windowObject, struct IntuiMessage *message);
void ReactionGui::createUserExperience()
{
	PublicScreen::instance()->openPublicScreen("Spotless", "Spotless - Copyright © 2020 by Alpha Kilimanjaro");

	//Read dimensions from Settings

	if(_windowPointer) {
		IIntuition->CloseWindow(_windowPointer);
		_windowPointer = 0;
	}

	if (PublicScreen::usingPublicScreen()) {
		int screenWidth = PublicScreen::instance()->screenWidth();
		int screenHeight = PublicScreen::instance()->screenHeight();

		int barHeight = PublicScreen::instance()->screenBarHeight();

		windowWidth = screenWidth;
		windowHeight = screenHeight - barHeight;
	}

	bool backdropWindow = PublicScreen::usingPublicScreen();
	
	Object *content = createEmbeddedWidgets();
	
//	RA_SetUpHook (idcmpHook, mainIDCMPHookFunction, 0);
	_windowObject = WindowObject,
		WA_ScreenTitle,         "Spotless",
		WA_Title,              	"Spotless",
		WA_PubScreen,           PublicScreen::instance()->screenPointer(),
		WA_Top,					PublicScreen::instance()->screenBarHeight(),
		WA_Width,               windowWidth,
		WA_Height,              windowHeight,
		WA_CloseGadget,			true,
		//WA_Backdrop,            backdropWindow,
		//WA_Borderless,          backdropWindow,
		WA_Activate,			TRUE,
		WA_ReportMouse,			TRUE,
		
//		WINDOW_IDCMPHook,		&idcmpHook, //to prevent keeping track of gadgets from gui generator
//		WINDOW_IDCMPHookBits,	IDCMP_GADGETUP,
		
		WINDOW_ParentLayout,		content,
	EndWindow;
	
	if (_windowObject) {
		_windowPointer = (struct Window *) RA_OpenWindow(_windowObject);
		if (_windowPointer) {
//			IIntuition->SetMenuStrip (_windowPointer, (struct Menu *)menuStrip);
		}
	}
	
///	IDOS->NotifyProcListChange(NULL, SIGBREAKB_CTRL_F, 0); //for the ProcessManager

	//Create an extra menu
	if(!menu) {
		menu = new ReactionMenu;
	}
	Object *windowMenu = menu->addCreateMenu("Window");
	menu->addCreateMenuItem(windowMenu, "Iconify/Unifonify", "CTRL+U", 33);
	menu->addCreateMenuItem(windowMenu, "Switch fullscreen/separate windows", "CTRL+W", 34);
	menu->addCreateMenuItem(windowMenu, "About", "", 35);
}

void ReactionGui::attachMenu() {
    if(windowPointer()) IIntuition->SetMenuStrip(windowPointer(), (Menu *)menu->systemObject());
}

void ReactionGui::detachMenu() {
    if(windowPointer()) IIntuition->ClearMenuStrip(windowPointer());
}

void mainIDCMPHookFunction (struct Hook *hook, Object *windowObject, struct IntuiMessage *message)
{
#if 0
	if (message->Class == IDCMP_GADGETUP) {
//		GuiEvent *event = new GuiEvent;
		
		if (message->Class & WMHI_GADGETMASK == GID_LISTBROWSER) {
			uint32 relEvent;
//			WindowElement *element;
			IIntuition->GetAttrs(message->IAddress,
				GA_UserData,			&element,
				LISTBROWSER_RelEvent,	&relEvent,
			TAG_DONE);
		
			if (relEvent == LBRE_CHECKED) {
				event->setEventClass (GuiEvent::CheckboxCheck);
			} else if (relEvent == LBRE_UNCHECKED) {
				event->setEventClass (GuiEvent::CheckboxUncheck);
			}
		} else if (message->Class & WMHI_GADGETMASK == GID_SPEEDBAR) {
			event->setEventClass (GuiEvent::CLASS_ButtonPress);
			event->setId (Code);
		} else {
			event->setEventClass (GuiEvent::CLASS_ButtonPress);
			event->setId (Class & WMHI_GADGETMASK);
		}
		
		element->handleGuiEvent (event);
		delete event;
	}
#endif
}

void ReactionGui::destroyUserExperience ()
{
	if (_windowPointer) {
		IIntuition->DisposeObject (_windowObject);
		_windowPointer = 0;
	}
//	IDOS->NotifyProcListChange(0, NPLC_END, 0);
	PublicScreen::instance()->closePublicScreen();
}

Object *ReactionGui::createEmbeddedWidgets()
{
	Object *topLayout = IIntuition->NewObject(NULL, "layout.gadget", LAYOUT_Orientation, LAYOUT_VERTICAL, TAG_DONE);

	Object *rowLayouts[MaxRows];
	bool rowUsed[MaxRows];

	for (int r = 0; r < MaxRows; r++) {
		rowLayouts[r] = IIntuition->NewObject(NULL, "layout.gadget", LAYOUT_Orientation, LAYOUT_HORIZONTAL, TAG_DONE);
		rowUsed[r] = false;
	}
	actualMaxRows = 0;
	
	for (list <ReactionWidget *>::iterator it = widgets.begin(); it != widgets.end(); it++)
	{
		ReactionWidget *widget = (*it);
		if (widget->state() == ReactionWidget::STATE_EMBEDDED) {
			int row = widget->row();

			ReactionLayout *layout = new ReactionLayout(widget, widget->name());
			widget->createGuiObject(layout);
			widget->setParentLayout(layout);

			if (rowUsed[row])
				IIntuition->SetAttrs (rowLayouts[row], LAYOUT_WeightBar, TRUE, TAG_DONE);
			rowUsed[row] = true;

			IIntuition->SetAttrs (rowLayouts[row], LAYOUT_AddChild, layout->systemObject(), TAG_DONE);
			actualMaxRows = MAX(actualMaxRows, row);
		}
	}
	
	for (int r = 0; r <= actualMaxRows; r++) {
		IIntuition->SetAttrs (topLayout, LAYOUT_AddChild, rowLayouts[r], TAG_DONE);
		if (r < actualMaxRows)
			IIntuition->SetAttrs (topLayout, LAYOUT_WeightBar, TRUE, TAG_DONE);
	}
	
	for (int r = actualMaxRows + 1; r < MaxRows; r++)
		IIntuition->DisposeObject (rowLayouts[r]);

	return topLayout;
}

void ReactionGui::showStageInterface ()
{
	int screenWidth = PublicScreen::instance()->screenWidth ();
	int screenHeight = PublicScreen::instance()->screenHeight ();

	StageInterface stage (screenWidth, screenHeight);

	stage.createSurfacesFromElements (_windowPointer, widgets);
	
	if (stage.show(PublicScreen::instance()->screenPointer())) {
		stage.loop();
		stage.close();
	}
		//rearrange gui according to final state of stage interface}
}

// -----------------------------------------------------------------------------------------------

ReactionWidget *ReactionGui::mouseOverWidget (int mouseX, int mouseY)
{
	for (list <ReactionWidget *>::iterator it = widgets.begin (); it != widgets.end (); it++) {
		if ((*it)->state () == ReactionWidget::STATE_EMBEDDED) {
			int top, left, width, height;
			(*it)->getDimensions(&left, &top, &width, &height);

			if (mouseX >= left && mouseY >= top && mouseX <= left + width && mouseY <= top + height)
				return *it;
		}
	}
	return 0;
}

void ReactionGui::doMouseMove (int mouseX, int mouseY)
{
#if 0
	ReactionWidget *widget = mouseOverWidget (mouseX, mouseY);
	bool hasContextMenu = widget->parentElement()->hasContextMenu();
	if (widget)
		IIntuition->SetWindowAttr(_windowPointer, WA_RMBTrap, (APTR)hasContextMenu, 4);
#endif
}

void ReactionGui::showAboutWindow ()
{
	Object *requesterObject = (Object *)IIntuition->NewObject( IRequester->REQUESTER_GetClass(), NULL,
		REQ_Type,       REQTYPE_INFO,
		REQ_TitleText,  "About",
		REQ_BodyText,   "Spotless - Copyright © 2020 Alpha Kilimanjaro",
		REQ_GadgetText, "Ok",
		TAG_DONE);

	if (requesterObject) {
		IIntuition->IDoMethod (requesterObject, RM_OPENREQ, 0, _windowPointer, 0, TAG_END);
		IIntuition->DisposeObject (requesterObject);
	}
}

#define RAWKEY_ESCAPE 0x45

bool ReactionGui::handleMainWindowEvents()
{
	uint32 Class;
	int16 Code;
    bool done = false;
	
	while (!done && (Class = RA_HandleInput (_windowObject, &Code)) != WMHI_LASTMSG) {
		ReactionWidget *widget = mouseOverWidget (_windowPointer->MouseX, _windowPointer->MouseY);
		if (widget) {
			bool result = widget->processEvent (Class, Code);
			if (result == false)
				done = true;
        }
		
		switch (Class & WMHI_CLASSMASK) {
        	case WMHI_CLOSEWINDOW:
				done = true;
                break;
			
            case WMHI_RAWKEY:
				
				if (Code == RAWKEY_ESCAPE)
					//done = true;
					showStageInterface();
				break;

			case WMHI_ICONIFY:

				if (RA_Iconify(_windowObject))
    				_windowPointer = 0;
				break;

			case WMHI_UNICONIFY:

				_windowPointer = RA_Uniconify (_windowObject);
				if (!_windowPointer)
					done = true;
				break;
				
			case WMHI_MOUSEMOVE:

				doMouseMove (_windowPointer->MouseX, _windowPointer->MouseY);	//for context menus
    			break;

#if 0
			case WMHI_MOUSEBUTTONS:

				if(Code == MENUDOWN) {
					ReactionWidget *widget = mouseOverWidget (_windowPointer->MouseX, _windowPointer->MouseY);
					WindowElement *element = widget ? widget->parentElement() : 0;
					if (element && element->hasContextMenu ()) {
						GuiObject *context = element->createContextMenu ();
						IIntuition->IDoMethod ((Object *)context->systemObject (), PM_OPEN, _windowPointer);
					}
				}
				break;
				
			case WMHI_MENUPICK:
			{
				MenuBarHandler *menuHandler = (MenuBarHandler *)ElementsInterface::findElement ("MENU_BAR_HANDLER");
				if (menuHandler) {
					uint32 id;
					while ((id = IIntuition->IDoMethod ((Object *)menuHandler->menuBarObject (), MM_NEXTSELECT, 0, id)) != NO_MENU_ID) {
						menuHandler->handleMenuSelect (id);
					}
				}
				break;
			}
#endif
		}
	}
    return done;
}

void ReactionWidget::setState(ReactionWidget::WidgetState state) {
	if (_state == state) return;
	if (_state == STATE_HIDDEN) {
		if (state == STATE_WINDOWED) {
			openWindow();
		} else if (state == STATE_ICONIFIED) {
			iconify();
		}
	} else if (_state == STATE_ICONIFIED) {
			uniconify();
		if (state == STATE_WINDOWED) {
			openWindow();
		}
	} else if (state == STATE_EMBEDDED) {
		uniconify();
		closeWindow();
		_state = STATE_EMBEDDED;
		return;
	}
	_state = state;
}

void ReactionGui::addEmbeddedWidget(ReactionWidget *widget, int row)
{
	widgets.push_back (widget);
	widget->setRow(row);
	widget->setState(ReactionWidget::STATE_EMBEDDED);
}

ReactionWidget *ReactionGui::findWidget(string name)
{
	for(ReactionWidget *widget : widgets) {
		if(!widget->name().compare(name))
			return widget;
	}
	return 0;
}

uint32 ReactionGui::mainWindowSignalMask()
{
	uint32 mask = 0x0;
	IIntuition->GetAttrs (_windowObject, WINDOW_SigMask, &mask, TAG_DONE);
	return mask;
}

uint32 ReactionGui::collectWidgetsSignalMasks()
{
	uint32 allSignals = 0x0;

	for (list<ReactionWidget *>::iterator it = widgets.begin(); it != widgets.end(); it++)
		allSignals |= (*it)->windowSignalMask();
	return allSignals;
}

bool ReactionGui::handleWindowedEvents (ReactionWidget *widget)
{
	bool shouldContinue = true;
	uint32 Class;
	uint16 Code;
	
	while ((Class = RA_HandleInput(widget->windowObject, &Code)) != WMHI_LASTMSG) {
		switch (Class & WMHI_CLASSMASK) {
			case WMHI_CLOSEWINDOW:
				widget->closeWindow ();
				break;
			
			case WMHI_ICONIFY:
				widget->iconify();
				break;

			case WMHI_UNICONIFY:
				widget->uniconify();
				break;
			
			case WMHI_MENUPICK: {
#if 0
				MenuBarHandler *menuHandler = (MenuBarHandler *)ElementsInterface::findElement ("MENU_BAR_HANDLER");
				if (menuHandler) {
					uint32 id;
					while ((id = IIntuition->IDoMethod ((Object *)menuHandler->menuBarObject (), MM_NEXTSELECT, 0, id)) != NO_MENU_ID) {
						menuHandler->handleMenuSelect (id);
					}
				}
#endif
				break;
			}

			default:
				shouldContinue = widget->processEvent (Class, Code);
		}
	}
	return shouldContinue;
}

void ReactionGui::eventLoop()
{
	bool done = false;
	while (!done) {
	//	ChildHandler *childHandler = (ChildHandler *)ElementsInterface::findElement ("CHILD_HANDLER");
	
		uint32 allSignals = collectWidgetsSignalMasks();
		allSignals |= mainWindowSignalMask();
//		allSignals |= childHandler->childSignal();
		//allSignals |= childHandler->pipeSignal(); TODO
		allSignals |= SIGBREAKF_CTRL_C;
		allSignals |= SIGBREAKB_CTRL_F; //proclist change
		allSignals |= SIGF_CHILD; //death signal
	
		uint32 result = IExec->Wait (allSignals);

		if (result & SIGBREAKF_CTRL_C) {
			done = true;
			break;
		}
		
		if (result & SIGBREAKB_CTRL_F) {
	//		ProcessManager *processManager = (ProcessManager *)ElementsInterface::findElement("PROCESS_MANAGER");
	//		processManager->update();
		}
		
		if (result & SIGF_CHILD) {
//			childHandler->handleDeath();
		}

#if 0		
		if (result & childHandler->childSignal()) {
			childHandler->handleSignal();
		}	
#endif

/*		if (result & childHandler->pipeSignal()) {
			PipeHandler *pipeHandler = ElementsInterface::findElement ("PIPE_HANDLER");
			pipeHandler->handleInput();
		}*/

		bool shouldContinue = true;
		
		//windowed elements
		for (std::list<ReactionWidget *>::iterator it = widgets.begin(); it != widgets.end(); it++) {
			if ((*it)->state() == ReactionWidget::STATE_WINDOWED && (result & (*it)->windowSignalMask()))
				shouldContinue = handleWindowedEvents((*it));
					
			if (!shouldContinue) {
				done = true;
				break;
			}
		}
		
		//embedded elements
		if (result & mainWindowSignalMask())
			done = handleMainWindowEvents();

		if (!shouldContinue)
			done = true;
	}
}
