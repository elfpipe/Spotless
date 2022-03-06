#include "reaction.h"
#include "Panel.hpp"
#include "Layout.hpp"
#include "Widget.hpp"

#include <string.h>

Panel::Panel(Widget *parent) {
    parentWidget = parent;
    state = PANEL_TABBED;
}

Panel::~Panel() {
}

void Panel::setTabbed(bool tabbed) {
    if (tabbed) {
        state = PANEL_TABBED;
    } else {
        state = PANEL_INDIVIDUAL;
    }
}

void Panel::addWidget(Widget *widget) {
    widgets.push_back(widget);
    // widget->setParent(parentWidget);
}

Object *Panel::createGuiObject() {
    Object *pages = PageObject, EndMember;

    char *pageLabels[widgets.size() + 1];
    for(int i = 0; i < widgets.size(); i++) {
        pageLabels[i] = strdup(widgets[i]->name().c_str());

        Layout *layout = new Layout(widgets[i]); //parentWidget);
        widgets[i]->createGuiObject(layout);
        widgets[i]->setParentLayout(layout);
        IIntuition->SetAttrs(pages, PAGE_Add, layout->systemObject(), TAG_DONE);
    }
    pageLabels[widgets.size()] = 0;

    object = ClickTabObject,
        GA_Text,            pageLabels,
        CLICKTAB_Current,	0,
        CLICKTAB_PageGroup,	pages,
    EndMember;
    return object;
}
