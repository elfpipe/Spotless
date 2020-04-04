#include <proto/button.h>
#include <gadgets/button.h>

#include <string>

#include "button.h"
#include "widget.h"

ReactionButton::ReactionButton (ReactionWidget *parent, const char *text)
{
	button = IIntuition->NewObject(NULL, "button.gadget",
		GA_UserData,	parent,
		GA_RelVerify,	true,
		GA_Text,		text,
	TAG_DONE);

	parent->topLevelParent()->addChild(button);
}

ReactionButton::~ReactionButton ()
{
}

void ReactionButton::setText (const char *text)
{
	IIntuition->SetAttrs (button,
		GA_Text,	text,
	TAG_DONE);
}

bool ReactionButton::isButton(Object *o)
{
	char *dummy;
	IIntuition->GetAttr(GA_Text, o, (uint32*)&dummy);
	return dummy ? true : false;
}