#include "Menubar.hpp"
#include "Event.hpp"
#include "Layout.hpp"
#include "Widget.hpp"
#include "Speedbar.hpp"
#include "reaction.h"
#include "Screen.hpp"

#include <libraries/keymap.h>

#include <string.h>
#include <iostream>

Widget::Widget(Widget *parentWidget)
	:	object(0),
		window(0),
		parentLayout(0),
		mainMenu(0),
		gadgetId(1),
		appPort(0)
{
	this->parentWidget = parentWidget;
}

Widget::~Widget()
{
    if(window) closeWindow();
	handlers.clear();
}

bool Widget::openWindow()
{
	if (window) return true;

	struct Screen *publicScreen = PublicScreen::instance()->screenPointer();

	object = WindowObject,
		WA_PubScreen,		publicScreen,
		WA_ScreenTitle,		"Spotless",
		WA_Title,			widgetName.size() ? widgetName.c_str() : "Spotless",
 		WA_DepthGadget,		TRUE,
		WA_SizeGadget,		TRUE,
		WA_DragBar,			TRUE,
		WA_CloseGadget,		TRUE,
		WA_Activate,		TRUE,
		WA_IDCMP,			IDCMP_RAWKEY,

		WINDOW_Position,	WPOS_CENTERSCREEN,
		WINDOW_ParentLayout,	createContent(),
		WINDOW_MenuStrip,		mainMenu ? mainMenu->systemObject() : 0,
        WINDOW_GadgetHelp,      TRUE,

	EndWindow;
	
	if (object) window = (struct Window *) RA_OpenWindow (object);

	openedWindows.push_back(this);

	return window != 0;
}

bool Widget::openNewWindow(Widget *widget)
{
	if(widget->openWindow()) {
		//widget->setMenubar(mainMenu);
		openedWindows.push_back(widget);
		return true;
	}
	return false;
}

Object *Widget::createContent()
{
    parentLayout = new Layout(this, Layout::LAYOUT_Vertical);
	createGuiObject(parentLayout);
	return parentLayout->systemObject();
}

void Widget::setMenubar(Menubar *menu)
{
	mainMenu = menu;
	if(mainMenu) mainMenu->createMenu();
	if(window) IIntuition->SetMenuStrip(window, (struct Menu *)mainMenu->systemObject());
}

void Widget::closeWindow ()
{
	if (object) IIntuition->DisposeObject (object);
	object = 0;
	window = 0;

	if (parentLayout) delete parentLayout;
	parentLayout = 0;

	children.clear();
	openedWindows.remove(this);
}

void Widget::closeNewWindow(Widget *widget)
{
	widget->closeWindow();
	openedWindows.remove(widget);

}
void Widget::closeAllWindows()
{
	list<Widget *> opened(openedWindows);
	for (list<Widget *>::iterator it = opened.begin(); it != opened.end(); it++)
		closeNewWindow(*it);
}

void Widget::closeAllExceptThis()
{
	list<Widget *> opened(openedWindows);
	for (list<Widget *>::iterator it = opened.begin(); it != opened.end(); it++) {
		if((*it) != this) closeNewWindow(*it);
	}
	openedWindows.clear();
	openedWindows.push_back(this);
}

extern struct MsgPort *AppPort;

int Widget::waitForClose()
{
	bool closeAll = false;
	
	while (!closeAll) {
		uint32 result = IExec->Wait (handlerSignals() | openedWindowsSignalMask() | SIGBREAKF_CTRL_C );

		if (result & SIGBREAKF_CTRL_C) {
			closeAll = true;
		}

		for(int i = 0; i < handlers.size(); i++) {
			if(result & handlers[i]->signal)
				handlers[i]->handler();
		}

		for(list<Widget *>::iterator it = openedWindows.begin(); it != openedWindows.end(); it++) {
			bool close = false;
			Widget *target = (*it);
			if(!target) continue;
			if(!target->windowPointer()) continue;
			Object *object = target->windowObject();

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
							done = true;
							close = true;
							if(target == this) closeAll = true;
							break;

						case WMHI_MENUPICK: {
							uint32 id = NO_MENU_ID;
							while ((id = IIntuition->IDoMethod(mainMenu->systemObject(),MM_NEXTSELECT,0,id)) != NO_MENU_ID)
							bool closeAll = false;
							done = mainMenu->handleMenuPick(id, &closeAll);
							break;
						}

						case WMHI_ICONIFY : {
							if(!appPort) break;

							closeAllExceptThis();
							iconify();
							result = 0x0;

							uint32 newResult = IExec->Wait (1L << appPort->mp_SigBit);
							uniconify();
							done = true;
							break;
						}

						case WMHI_RAWKEY :
							if ((Code & WMHI_KEYMASK) == RAWKEY_ESC && target != this)
								close = true;
							else {
								target->processEvent(Class, Code);
							}
							break;

						default:
							close = target->processEvent(Class, Code);
							done = close;
							closeAll = (close && target==this);
							break;
					}
				}
			}
			if(close && target != this) { closeNewWindow(target); result = 0x0; break; } close = false;
		}
		// if(mainMenu->switchScreen()) {
		// 	closeAllWindows();
		// 	if (!PublicScreen::usingPublicScreen())
		// 		PublicScreen::instance()->openPublicScreen("Spotless", "Spotless - Copyright © 2020, 2022 by Alfkil Thorbjørn Wennermark");
		// 	else
		// 		PublicScreen::instance()->closePublicScreen();
		// 	openWindow();
		// 	mainMenu->doScreenSwitch = false;
		// }
	}
	// closeWindow ();
	closeAllWindows();
	return 0;
}

Widget *Widget::topLevelParent()
{
	Widget *top = parentWidget;
	while(top && top->parentWidget)
		top = top->parentWidget;
	return top ? top : this;
}

unsigned int Widget::addChild(Object *object)
{
	children.push_back(object);
	IIntuition->SetAttrs(object, GA_ID, gadgetId, TAG_DONE);
	return gadgetId++;
}

Object *Widget::findChild(unsigned int id)
{
	for (int i = 0; i < children.size(); i++) {
		Object *child = children[i];
		unsigned int childId;
		IIntuition->GetAttrs (child, GA_ID, &childId, TAG_DONE);
		if (childId == id)
			return child;
	}
	return 0;
}

void Widget::setName(string name)
{
	widgetName = name;
	if(window) IIntuition->SetWindowTitles(window, name.c_str(), "Spotless");
}

void Widget::iconify()
{
	if(object) {
		RA_Iconify(object);
		cout << "RA_Iconify.\n";
		window = 0;
	}
}

void Widget::uniconify()
{
	if(object)
		window = RA_Uniconify(object);
}

void Widget::windowToFront ()
{
	if (window) IIntuition->WindowToFront (window);
}

uint32 Widget::windowSignalMask ()
{
	uint32 mask = 0x0;
	if (object) IIntuition->GetAttr (WINDOW_SigMask, object, &mask);
	return mask;
}

uint32 Widget::openedWindowsSignalMask()
{
	uint32 mask = 0x0;
	for (list<Widget *>::iterator it = openedWindows.begin(); it != openedWindows.end(); it++) {
		mask |= (*it)->windowSignalMask();
	// 	Object *object = (*it)->windowObject();
	// 	uint32 sigMask;
	// 	if (object) {
	// 		IIntuition->GetAttr (WINDOW_SigMask, object, &sigMask);
	// 		mask |= sigMask;
	// 	}
	// }
	}
	return mask;
}

Widget *Widget::findOpenedWindowWidget(uint32 mask) {
	for (list<Widget *>::iterator it = openedWindows.begin(); it != openedWindows.end(); it++) {
		// cout << "windowSignalMask() : " << (void *)(*it)->windowSignalMask() << "\n";
		if(mask & (*it)->windowSignalMask())
			return *it;
		// Object *object = (*it)->windowObject();
		// uint32 sigMask;
		// if (object) {
		// 	IIntuition->GetAttr (WINDOW_SigMask, object, &sigMask);
		// 	if(sigMask & mask) {
		// 		return *it;
		// 	}
		// }
	}
	return 0x0;
}

bool Widget::processEvent (uint32 Class, uint16 Code)
{
	if(!window) return false; // if iconified

	int MouseX = window->MouseX;
	int MouseY = window->MouseY;

	Event *event = 0;
	Widget *parent = 0;
	switch (Class & WMHI_CLASSMASK) {
		case WMHI_RAWKEY : {
			event = new Event (Event::CLASS_KeyPress);
			event->setElementId (Code & WMHI_KEYMASK);
			event->setElementDescription ("");
		}
		break;

		case WMHI_GADGETUP: {
			// cout << "WMHI_GADGETUP\n";
			uint32 gadgetId = Class & WMHI_GADGETMASK;
				// cout << "gadgetId : " << gadgetId << "\n";
			
			Object *gadget = findChild(gadgetId);
			if(!gadget) break;

			// cout << "gadget : " << (void *) gadget << "\n";			
			IIntuition->GetAttrs(gadget,
				GA_UserData, &parent,
				TAG_DONE);

			// cout << "parent : " << (void *)parent << "\n";

			if(Checkbox::isCheckbox(gadget)) {
				event = new Event (Event::CLASS_CheckboxPress);
				event->setElementId (gadgetId);
				event->setElementDescription ("");
			} else if(RButton::isButton(gadget)) {
				// char *text;

				// IIntuition->GetAttrs(gadget,
				// 	GA_Text, &text,
				// TAG_DONE);

				event = new Event (Event::CLASS_ButtonPress);
				event->setElementId (gadgetId);
				event->setElementDescription ("");
			} else if(RString::isString(gadget)) {
				char *text;

				IIntuition->GetAttrs(gadget,
					STRINGA_TextVal, &text,
				TAG_DONE);

				event = new Event (Event::CLASS_StringEntry);
				event->setElementId (gadgetId);
				event->setElementDescription(text);
			} else if(Listbrowser::isListbrowser(gadget)) {
				uint32 relEvent, selected;

				IIntuition->GetAttrs(gadget,
					LISTBROWSER_RelEvent,	&relEvent,
					LISTBROWSER_Selected,	&selected,
				TAG_DONE);
			
				event = new Event(Event::CLASS_SelectNode);

				if (relEvent == LBRE_CHECKED)
					event->setEventClass (Event::CLASS_CheckboxCheck);
				else if (relEvent == LBRE_UNCHECKED)
					event->setEventClass (Event::CLASS_CheckboxUncheck);

				event->setElementId(gadgetId);
				event->setItemId (selected);
			} else if(Speedbar::isSpeedbar(gadget)) {
				event = new Event (Event::CLASS_ActionButtonPress);
				event->setElementId (Code);
			}
			break;
		}
		case WMHI_MOUSEBUTTONS: {
			event = new Event(Event::CLASS_MouseButtonDown);
			event->setMousePosition (MouseX, MouseY);
			break;
		}
		case WMHI_MOUSEMOVE: {
			event = new Event(Event::CLASS_MouseMove);
			event->setMousePosition (MouseX, MouseY);
			break;
		}
	}
	bool result = false;
	if (event) {
		result = parent ? parent->handleEvent(event) : handleEvent(event);
		delete event;
	}
	return result;
}