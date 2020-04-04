#include "layout.h"
#include "widget.h"
#include "panel.h"

#include <proto/layout.h>
#include <proto/space.h>
#include <gadgets/layout.h>

#include <reaction/reaction_macros.h>

#include <string.h>

ReactionLayout::ReactionLayout(ReactionWidget *parent, LayoutDirection direction)
{
	this->parent = parent;
    this->direction = direction;
	if (direction == LAYOUT_Horizontal) {
		layout = HLayoutObject, EndMember;
	} else {
		layout = VLayoutObject,	EndMember;
	}
}

ReactionLayout::ReactionLayout (ReactionWidget *parent, string label)
{
	this->parent = parent;
	layout = HLayoutObject,
		LAYOUT_SpaceOuter, TRUE,
		LAYOUT_BevelStyle, BVS_GROUP,
		LAYOUT_Label, strdup(label.c_str()),
	EndMember;
}

ReactionLayout::~ReactionLayout()
{
}

ReactionLayout *ReactionLayout::createVerticalLayout()
{
	ReactionLayout *childLayout = new ReactionLayout(parent, LAYOUT_Vertical);
	IIntuition->SetAttrs (layout,
		LAYOUT_AddChild, childLayout->systemObject(),
	TAG_DONE);
	return childLayout;
}

ReactionLayout *ReactionLayout::createHorizontalLayout ()
{
	ReactionLayout *childLayout = new ReactionLayout(parent, LAYOUT_Horizontal);
	IIntuition->SetAttrs (layout,
		LAYOUT_AddChild, childLayout->systemObject(),
	TAG_DONE);
	return childLayout;
}

ReactionLayout *ReactionLayout::createLabeledLayout (string label)
{
	ReactionLayout *childLayout = new ReactionLayout(parent, label);
	IIntuition->SetAttrs (layout,
		LAYOUT_AddChild, childLayout->systemObject(),
	TAG_DONE);
	return childLayout;
}	

void ReactionLayout::createSpace()
{
	IIntuition->SetAttrs (layout,
		LAYOUT_AddChild, SpaceObject, End,
	TAG_DONE);
}

ReactionSpeedbar *ReactionLayout::createSpeedbar()
{
	ReactionSpeedbar *buttonBar = new ReactionSpeedbar(parent);
	IIntuition->SetAttrs (layout, LAYOUT_AddChild, buttonBar->systemObject(), TAG_DONE);
	return buttonBar;
}

ReactionListbrowser *ReactionLayout::createListbrowser ()
{
	ReactionListbrowser *listbrowser = new ReactionListbrowser(parent);
	IIntuition->SetAttrs (layout, LAYOUT_AddChild, listbrowser->systemObject(), TAG_DONE);
	return listbrowser;
}

ReactionButton *ReactionLayout::createButton (const char *text)
{
	ReactionButton *button = new ReactionButton(parent, text);
	IIntuition->SetAttrs (layout,
		LAYOUT_AddChild, button->systemObject(),
		CHILD_WeightedHeight,	0,
		CHILD_WeightedWidth,	0,
	TAG_DONE);
	return button;
}

void ReactionLayout::addWeightBar()
{
	IIntuition->SetAttrs (layout, LAYOUT_WeightBar, TRUE, TAG_DONE);
}

void ReactionLayout::addEmbeddedWidget(ReactionWidget *widget)
{
	widget->setParent(parent);
	widget->setParentLayout(this);
	widget->createGuiObject(this);
}

void ReactionLayout::addTabbedPanel(ReactionPanel *panel, int weight)
{
	IIntuition->SetAttrs (layout,
		LAYOUT_AddChild, panel->createGuiObject(),
		CHILD_WeightedHeight,	weight,
		CHILD_WeightedWidth,	weight,
	TAG_DONE);
}

void ReactionLayout::getDimensions(int *left, int *top, int *width, int *height)
{
	IIntuition->GetAttrs (layout, GA_Left, left, GA_Top, top, GA_Width, width, GA_Height, height, TAG_DONE);
}
