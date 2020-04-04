#include "speedbar.h"
#include "screen.h"

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/speedbar.h>
#include <proto/bitmap.h>
#include <gadgets/speedbar.h>
#include <images/bitmap.h>
#include <images/bevel.h>

#include <reaction/reaction_macros.h>

#include <string>
#include <string.h>
#include <iostream>

#include "widget.h"

using namespace std;

ReactionSpeedbar::ReactionSpeedbar(ReactionWidget *parent) {
	this->parent = parent;
	init();
}

ReactionSpeedbar::~ReactionSpeedbar() {
}

void ReactionSpeedbar::init()
{
	IExec->NewList (&buttonList);
	speedbar = SpeedBarObject,
		GA_UserData,			parent,
		GA_RelVerify,			TRUE,
		SPEEDBAR_Orientation,	SPEEDBAR_HORIZONTAL,
//		SPEEDBAR_BevelStyle,	BVS_NONE,
		SPEEDBAR_Buttons,		&buttonList,
    	EndMember;
	
	parent->topLevelParent()->addChild(speedbar);
}

void ReactionSpeedbar::clear()
{
	struct Node *node, *nextnode;

	node = IExec->GetHead (&buttonList);
	while (node != 0) {
		nextnode = IExec->GetSucc (node);
		ISpeedBar->FreeSpeedButtonNode (node);
		node = nextnode;
	}
	IExec->NewList (&buttonList);
}

void ReactionSpeedbar::addNode (struct Node *node)
{
	IExec->AddTail (&buttonList, node);
}

void ReactionSpeedbar::addButton (int buttonId, string buttonText, string iconName)
{
	struct Screen *screen = PublicScreen::instance()->screenPointer();

    string imagePath = "tbimages:" + iconName;
	string selectedPath = imagePath + "_s";
	string disabledPath = imagePath + "_g";

    Object *image = BitMapObject,
        BITMAP_Screen,				screen,
        BITMAP_Masking,				true,
        BITMAP_SourceFile,			strdup(imagePath.c_str()),
		BITMAP_SelectSourceFile,	strdup(selectedPath.c_str()),
		BITMAP_DisabledSourceFile,	strdup(disabledPath.c_str()),
        EndMember;

	struct Node *node = ISpeedBar->AllocSpeedButtonNode((uint16)buttonId,
		SBNA_Text,		strdup(buttonText.c_str()),
		SBNA_Image,		image,
		SBNA_Enabled,	true,
//		SBNA_Spacing,	2,
//		SBNA_Highlight,	SBH_RECESS,
		TAG_DONE);
	
	if (node) {
		addNode (node);
	}
}

#define SBID_SPACER 0x9999

void ReactionSpeedbar::addSpacer ()
{
	struct Node *node = ISpeedBar->AllocSpeedButtonNode (
		SBID_SPACER,
		SBNA_Spacer,	TRUE,
		TAG_DONE);
	
	if (node)
		addNode (node);
}

Gadget *ReactionSpeedbar::findButtonGadget (uint16 id)
{
	struct Node *node;
	node = IExec->GetHead (&buttonList);
	
	while (node != 0) {
		if (node->ln_Pri == id)
			return (Gadget *)node;
		node = IExec->GetSucc (node);
	}
	return 0;
}

void ReactionSpeedbar::attach()
{
	IIntuition->RefreshSetGadgetAttrs((struct Gadget *)speedbar, parent->topLevelParent()->windowPointer(), 0,
		SPEEDBAR_Buttons,		&buttonList,
	TAG_DONE);
}

void ReactionSpeedbar::detach()
{
	IIntuition->SetAttrs(speedbar,
		SPEEDBAR_Buttons,		~0,
	TAG_DONE);
}

void ReactionSpeedbar::enableButton (int id, bool enable)
{
	detach();
	Gadget *gadget = findButtonGadget (id);
	if (gadget) {
		ISpeedBar->SetSpeedButtonNodeAttrs ((struct Node *)gadget, SBNA_Disabled, !enable, TAG_DONE);
	}
	attach();
}

void ReactionSpeedbar::setButtonText (int id, string text)
{
	detach();
	Gadget *gadget = findButtonGadget (id);
	if (gadget) {
		ISpeedBar->SetSpeedButtonNodeAttrs ((struct Node *)gadget, SBNA_Text, text.c_str(), TAG_DONE);
	}
	attach();
}

bool ReactionSpeedbar::isSpeedbar(Object *o)
{
	uint32 dummy;
	return IIntuition->GetAttr(SPEEDBAR_Buttons, o, &dummy) ? true : false;
}
