#include "reaction.h"
#include "panel.h"
#include "layout.h"
#include "widget.h"

#include <string.h>

ReactionPanel::ReactionPanel(ReactionWidget *parent) {
    parentWidget = parent;
    state = PANEL_TABBED;
}

ReactionPanel::~ReactionPanel() {
}

void ReactionPanel::setTabbed(bool tabbed) {
    if (tabbed) {
        state = PANEL_TABBED;
    } else {
        state = PANEL_INDIVIDUAL;
    }
}

void ReactionPanel::addWidget(ReactionWidget *widget) {
    widgets.push_back(widget);
    widget->setParent(parentWidget);
}

Object *ReactionPanel::createGuiObject() {
    Object *pages = PageObject, EndMember;

    char *pageLabels[widgets.size() + 1];
    for(int i = 0; i < widgets.size(); i++) {
        pageLabels[i] = strdup(widgets[i]->name().c_str());

        ReactionLayout *layout = new ReactionLayout(parentWidget);
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
