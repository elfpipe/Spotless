#include <proto/button.h>
#include <gadgets/button.h>

#include <string>

#include "GoButton.hpp"
#include "Widget.hpp"

GoButton::GoButton (Widget *parent, const char *text)
{
	button = IIntuition->NewObject(NULL, "button.gadget",
		GA_UserData,	parent,
		GA_RelVerify,	true,
		GA_Text,		text,
	TAG_DONE);

	parent->topLevelParent()->addChild(button);
}

GoButton::~GoButton ()
{
}

void GoButton::setText (const char *text)
{
	IIntuition->SetAttrs (button,
		GA_Text,	text,
	TAG_DONE);
}

bool GoButton::isButton(Object *o)
{
	char *dummy;
	IIntuition->GetAttr(GA_Text, o, (uint32*)&dummy);
	return dummy ? true : false;
}