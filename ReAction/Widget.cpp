#include "Menubar.hpp"
#include "Event.hpp"
#include "Layout.hpp"
#include "Widget.hpp"
#include "Speedbar.hpp"
#include "reaction.h"
#include "Screen.hpp"

Widget::Widget(Widget *parentWidget)
	:	object(0),
		window(0),
		parentLayout(0),
		mainMenu(0),
		gadgetId(1)
{
	this->parentWidget = parentWidget;
}

Widget::~Widget()
{
    closeWindow();
}

void Widget::openWindow()
{
	if (window) return;

	struct Screen *publicScreen = PublicScreen::instance()->screenPointer();

	object = WindowObject,
		WA_PubScreen,		publicScreen,
		WA_ScreenTitle,		"Spotless",
		WA_Title,			"Spotless",
 		WA_DepthGadget,		TRUE,
		WA_SizeGadget,		TRUE,
		WA_DragBar,			TRUE,
		WA_CloseGadget,		TRUE,
		WA_Activate,		TRUE,
//		WA_IDCMP,			IDCMP_MENUPICK,
	
		WINDOW_ParentLayout,	createContent(),
		WINDOW_MenuStrip,		mainMenu ? mainMenu->systemObject() : 0,
	EndWindow;
	
	if (object) window = (struct Window *) RA_OpenWindow (object);
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
}

void Widget::waitForClose()
{
	bool close = false;
	
	while (!close) {
		uint32 result = IExec->Wait (windowSignalMask() | SIGBREAKF_CTRL_C);

		if (result & SIGBREAKF_CTRL_C) {
			close = true;
		}

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

					case WMHI_MENUPICK: {
						uint32 id = NO_MENU_ID;
						while ((id = IIntuition->IDoMethod(mainMenu->systemObject(),MM_NEXTSELECT,0,id)) != NO_MENU_ID)
						close = mainMenu->handleMenuPick(id);
						done = close;
					}
						break;

					default:
						close = processEvent(Class, Code);
						done = close;
						break;
				}
			}
		}
	}
	closeWindow ();
}

Widget *Widget::topLevelParent()
{
	Widget *top = parentWidget;
	while(top && top->parentWidget)
		top = top->parentWidget;
	return top ? top : this;
}

void Widget::addChild(Object *object)
{
	children.push_back(object);
	IIntuition->SetAttrs(object, GA_ID, gadgetId++, TAG_DONE);
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
void Widget::iconify()
{
	if(object)
		RA_Iconify(object);
	window = 0;
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

bool Widget::processEvent (uint32 Class, uint16 Code)
{
	int MouseX = window->MouseX;
	int MouseY = window->MouseY;

	GuiEvent *event = 0;
	Widget *parent = 0;
	switch (Class & WMHI_CLASSMASK) {
		case WMHI_GADGETUP: {
			unsigned int gadgetId = Class & WMHI_GADGETMASK;
			Object *gadget = findChild(gadgetId);

			IIntuition->GetAttrs(gadget,
				GA_UserData, &parent,
				TAG_DONE);

			if(Listbrowser::isListbrowser(gadget)) {
				uint32 relEvent, selected;

				IIntuition->GetAttrs(gadget,
					LISTBROWSER_RelEvent,	&relEvent,
					LISTBROWSER_Selected,	&selected,
				TAG_DONE);
			
				event = new GuiEvent(GuiEvent::CLASS_SelectNode);

				if (relEvent == LBRE_CHECKED)
					event->setEventClass (GuiEvent::CLASS_CheckboxCheck);
				else if (relEvent == LBRE_UNCHECKED)
					event->setEventClass (GuiEvent::CLASS_CheckboxUncheck);

				event->setElementId(gadgetId);
				event->setItemId (selected);
			} else if(Speedbar::isSpeedbar(gadget)) {
				event = new GuiEvent (GuiEvent::CLASS_ButtonPress);
				event->setElementId (Code);
			} else if(GoButton::isButton(gadget)) {
				char *text;

				IIntuition->GetAttrs(gadget,
					GA_Text, &text,
				TAG_DONE);

				event = new GuiEvent (GuiEvent::CLASS_ButtonPress);
				event->setElementId (gadgetId);
				event->setElementDescription (text);
			}
			break;
		}
		case WMHI_MOUSEBUTTONS: {
			event = new GuiEvent(GuiEvent::CLASS_MouseButtonDown);
			event->setMousePosition (MouseX, MouseY);
			break;
		}
		case WMHI_MOUSEMOVE: {
			event = new GuiEvent(GuiEvent::CLASS_MouseMove);
			event->setMousePosition (MouseX, MouseY);
			break;
		}
	}
	bool result = false;
	if (event) {
		result = parent ? parent->handleGuiEvent(event) : handleGuiEvent(event);
		delete event;
	}
	return result;
}