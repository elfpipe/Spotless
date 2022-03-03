#include "Layout.hpp"
#include "Widget.hpp"
#include "Panel.hpp"
#include "String.hpp"

#include <proto/layout.h>
#include <proto/space.h>
#include <gadgets/layout.h>

#include <reaction/reaction_macros.h>

#include <string.h>

Layout::Layout(Widget *parent, LayoutDirection direction)
{
	this->parent = parent;
    this->direction = direction;
	if (direction == LAYOUT_Horizontal) {
		layout = HLayoutObject, EndMember;
	} else {
		layout = VLayoutObject,	EndMember;
	}
}

Layout::Layout (Widget *parent, string label)
{
	this->parent = parent;
	layout = HLayoutObject,
		LAYOUT_SpaceOuter, TRUE,
		LAYOUT_BevelStyle, BVS_GROUP,
		LAYOUT_Label, strdup(label.c_str()),
	EndMember;
}

Layout::~Layout()
{
}

Layout *Layout::createVerticalLayout(int weightedHeight, int weightedWidth)
{
	Layout *childLayout = new Layout(parent, LAYOUT_Vertical);
	IIntuition->SetAttrs (layout,
		LAYOUT_AddChild, childLayout->systemObject(),
		CHILD_WeightedHeight,	weightedHeight,
		CHILD_WeightedWidth,	weightedWidth,
	TAG_DONE);
	return childLayout;
}

Layout *Layout::createHorizontalLayout (int weightedHeight, int weightedWidth)
{
	Layout *childLayout = new Layout(parent, LAYOUT_Horizontal);
	IIntuition->SetAttrs (layout,
		LAYOUT_AddChild, childLayout->systemObject(),
		CHILD_WeightedHeight,	weightedHeight,
		CHILD_WeightedWidth,	weightedWidth,
	TAG_DONE);
	return childLayout;
}

Layout *Layout::createLabeledLayout (string label)
{
	Layout *childLayout = new Layout(parent, label);
	IIntuition->SetAttrs (layout,
		LAYOUT_AddChild, childLayout->systemObject(),
	TAG_DONE);
	return childLayout;
}	

void Layout::createSpace()
{
	IIntuition->SetAttrs (layout,
		LAYOUT_AddChild, SpaceObject, End,
	TAG_DONE);
}

Speedbar *Layout::createSpeedbar()
{
	Speedbar *buttonBar = new Speedbar(parent);
	IIntuition->SetAttrs (layout, LAYOUT_AddChild, buttonBar->systemObject(), TAG_DONE);
	return buttonBar;
}

Listbrowser *Layout::createListbrowser ()
{
	Listbrowser *listbrowser = new Listbrowser(parent);
	IIntuition->SetAttrs (layout, LAYOUT_AddChild, listbrowser->systemObject(), TAG_DONE);
	return listbrowser;
}

RButton *Layout::createButton (const char *text, const char *image)
{
	RButton *button = new RButton(parent, text, image);
	IIntuition->SetAttrs (layout,
		LAYOUT_AddChild, button->systemObject(),
		CHILD_WeightedHeight,	0,
		CHILD_WeightedWidth,	0,
	TAG_DONE);
	return button;
}

RString *Layout::createString (const char *content)
{
	RString *rstring = new RString(parent, content);
	IIntuition->SetAttrs (layout,
		LAYOUT_AddChild, rstring->systemObject(),
		CHILD_WeightedHeight,	0,
		CHILD_WeightedWidth,	0,
	TAG_DONE);
	return rstring;
}

Checkbox *Layout::createCheckbox (const char *text, bool checked)
{
	Checkbox *checkbox = new Checkbox(parent, text, checked);
	IIntuition->SetAttrs (layout,
		LAYOUT_AddChild, checkbox->systemObject(),
		CHILD_WeightedHeight,	0,
		CHILD_WeightedWidth,	0,
	TAG_DONE);
	return checkbox;
}

void Layout::addWeightBar()
{
	IIntuition->SetAttrs (layout, LAYOUT_WeightBar, TRUE, TAG_DONE);
}

void Layout::addEmbeddedWidget(Widget *widget)
{
	widget->setParent(parent);
	widget->setParentLayout(this);
	widget->createGuiObject(this);
}

void Layout::addTabbedPanel(Panel *panel, int weight)
{
	IIntuition->SetAttrs (layout,
		LAYOUT_AddChild, panel->createGuiObject(),
		CHILD_WeightedHeight,	weight,
		CHILD_WeightedWidth,	weight,
	TAG_DONE);
}

void Layout::getDimensions(int *left, int *top, int *width, int *height)
{
	IIntuition->GetAttrs (layout, GA_Left, left, GA_Top, top, GA_Width, width, GA_Height, height, TAG_DONE);
}
