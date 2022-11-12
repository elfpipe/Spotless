#include "reaction.h"
#include "Panel.hpp"
#include "Layout.hpp"
#include "Widget.hpp"

#include <string.h>

Panel::Panel(Widget *parent, list<Widget *> widgets) {
    parentWidget = parent;
    state = PANEL_TABBED;

    for(list<Widget *>::iterator it = widgets.begin(); it != widgets.end(); it++)
        addWidget(*it);
}

Panel::~Panel() {
    for(list<Widget *>::iterator it = widgets.begin(); it != widgets.end(); it++)
        (*it)->destroyContent();
    widgets.clear();
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
    int i = 0;
    for(list<Widget *>::iterator it = widgets.begin(); it != widgets.end(); it++) {
    // for(int i = 0; i < widgets.size(); i++) {
        pageLabels[i++] = strdup((*it)->name().c_str());

        Layout *layout = new Layout(*it); //parentWidget);
        (*it)->createGuiObject(layout);
        (*it)->setParentLayout(layout);
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
