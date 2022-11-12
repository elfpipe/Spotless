#include <proto/button.h>
#include <proto/bitmap.h>
#include <gadgets/button.h>
#include <images/bitmap.h>
#include <images/bevel.h>

#include <reaction/reaction_macros.h>

#include <string>
#include <string.h>
#include <iostream>

#include "Button.hpp"
#include "Widget.hpp"
#include "Screen.hpp"

#include <iostream>

list<Object *> RButton::buttons;

RButton::RButton (Widget *parent, const char *text, const char *iconName)
{
	struct Screen *screen = PublicScreen::instance()->screenPointer();

	string name(iconName);
	if(name.size()) {
		string imagePath = "tbimages:" + name;
		string selectedPath = imagePath + "_s";
		string disabledPath = imagePath + "_g";

		Object *image = BitMapObject,
			BITMAP_Screen,				screen,
			BITMAP_Masking,				true,
			BITMAP_SourceFile,			strdup(imagePath.c_str()),
			BITMAP_SelectSourceFile,	strdup(selectedPath.c_str()),
			BITMAP_DisabledSourceFile,	strdup(disabledPath.c_str()),
			EndMember;

		button = IIntuition->NewObject(NULL, "button.gadget",
			// GA_ID,			Widget::gadgetId++,
			GA_UserData,	parent,
			GA_RelVerify,	true,
			GA_Image,		image,
			GA_HintInfo,	text,
			// GA_Text,		" ",
		TAG_DONE);
	} else {
		button = IIntuition->NewObject(NULL, "button.gadget",
			// GA_ID,			Widget::gadgetId++,
			GA_UserData,	parent,
			GA_RelVerify,	true,
			GA_Text,		text,
		TAG_DONE);
	}
	this->text = text;
	buttons.push_back(button);

	// cout << "add Button " << Widget::gadgetId << " " << (void *)button << "\n";

	id = parent->topLevelParent()->addChild(button);
}

RButton::~RButton ()
{
	// cout << "Remove button : " << (void *)button << "\n";
	buttons.remove(button);
	Widget::removeChild(button);
}

void RButton::setText (const char *text)
{
	IIntuition->SetAttrs (button,
		GA_Text,	text,
	TAG_DONE);
	this->text = text;
}

const char *RButton::getText()
{
	return text;
}

bool RButton::isButton(Object *o)
{
	// cout << "isButton " << (void *)o << "\n";
	for(list<Object *>::iterator it = buttons.begin(); it != buttons.end(); it++) {
		// cout << "check : " << (void *)(*it) << "\n";
		if((*it) == o) return true;
	}
	return false;
}