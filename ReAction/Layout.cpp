#include "Layout.hpp"
#include "Widget.hpp"
#include "Panel.hpp"
#include "String.hpp"

#include <proto/layout.h>
#include <proto/space.h>
#include <gadgets/layout.h>

#include <reaction/reaction_macros.h>

#include <string.h>
#include <iostream>

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
	// std::cout << "cleanup.\n";
	//cleanup
	for(list<RButton *>::iterator it = buttons.begin(); it != buttons.end(); it++)
	{
		// cout << "deleting button " << (void *)(*it) << "\n";
		delete (*it);
	}
	buttons.clear();
	for(list<Speedbar *>::iterator it = speedbars.begin(); it != speedbars.end(); it++)
		delete (*it);
	speedbars.clear();
	for(list<Listbrowser *>::iterator it = listbrowsers.begin(); it != listbrowsers.end(); it++)
		delete (*it);
	listbrowsers.clear();
	for(list<RString *>::iterator it = strings.begin(); it != strings.end(); it++)
		delete (*it);
	strings.clear();
	for(list<Checkbox *>::iterator it = checkboxes.begin(); it != checkboxes.end(); it++)
		delete (*it);
	checkboxes.clear();
	for(list<Layout *>::iterator it = layouts.begin(); it != layouts.end(); it++) {
		// cout << "deleting layout " << (void *)(*it) << "\n";
		delete (*it);
	}
	layouts.clear();
	// for(list<Widget *>::iterator it = embeddedWidgets.begin(); it != embeddedWidgets.end(); it++) {
	// 	// (*it)->destroyContent();
	// }
	// embeddedWidgets.clear();
	for(list<Panel *>::iterator it = panels.begin(); it != panels.end(); it++) {
		delete (*it);
	}
	panels.clear();
}

Layout *Layout::createVerticalLayout(int weightedHeight, int weightedWidth)
{
	Layout *childLayout = new Layout(parent, LAYOUT_Vertical);
	IIntuition->SetAttrs (layout,
		LAYOUT_AddChild, childLayout->systemObject(),
		CHILD_WeightedHeight,	weightedHeight,
		CHILD_WeightedWidth,	weightedWidth,
	TAG_DONE);
	layouts.push_back(childLayout);	
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
	layouts.push_back(childLayout);	
	return childLayout;
}

Layout *Layout::createLabeledLayout (string label)
{
	Layout *childLayout = new Layout(parent, label);
	IIntuition->SetAttrs (layout,
		LAYOUT_AddChild, childLayout->systemObject(),
	TAG_DONE);
	layouts.push_back(childLayout);	
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
	speedbars.push_back(buttonBar);
	return buttonBar;
}

Listbrowser *Layout::createListbrowser ()
{
	Listbrowser *listbrowser = new Listbrowser(parent);
	IIntuition->SetAttrs (layout, LAYOUT_AddChild, listbrowser->systemObject(), TAG_DONE);
	listbrowsers.push_back(listbrowser);
	return listbrowser;
}

RButton *Layout::createButton (const char *text, const char *image)
{
	// cout << "add Button " << Widget::gadgetId << "\n";
	RButton *button = new RButton(parent, text, image);
	IIntuition->SetAttrs (layout,
		LAYOUT_AddChild, button->systemObject(),
		CHILD_WeightedHeight,	0,
		CHILD_WeightedWidth,	0,
	TAG_DONE);
	buttons.push_back(button);
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
	strings.push_back(rstring);
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
	checkboxes.push_back(checkbox);
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

void Layout::createTabbedPanel(Widget *parent, list<Widget *> &widgets, int weight)
{
	Panel *panel = new Panel(parent, widgets);
	
	IIntuition->SetAttrs (layout,
		LAYOUT_AddChild, panel->createGuiObject(),
		CHILD_WeightedHeight,	weight,
		CHILD_WeightedWidth,	weight,
	TAG_DONE);

	panels.push_back(panel);
}

void Layout::getDimensions(int *left, int *top, int *width, int *height)
{
	IIntuition->GetAttrs (layout, GA_Left, left, GA_Top, top, GA_Width, width, GA_Height, height, TAG_DONE);
}
