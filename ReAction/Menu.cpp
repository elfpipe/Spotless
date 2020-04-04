#include <intuition/menuclass.h>

#include <string.h>

#include "menu.h"
#include "gui.h"

ReactionMenu::ReactionMenu(ReactionWidget *parent)
    : menu(0)
{
    this->parent = parent;
    createMenuStrip();
}

ReactionMenu::~ReactionMenu()
{
    destroyMenuStrip();
}

void ReactionMenu::createMenuStrip () {
    menu = IIntuition->NewObject(NULL, "menuclass", MA_Type, T_ROOT, TAG_END);
}

void ReactionMenu::destroyMenuStrip () {
    if (menu) IIntuition->DisposeObject(menu);
    menu = 0;
}

void ReactionMenu::attach() {
    if(parent && parent->windowPointer()) IIntuition->SetMenuStrip(parent->topLevelParent()->windowPointer(), (Menu *)menu);
}

void ReactionMenu::detach() {
    if(parent && parent->windowPointer()) IIntuition->ClearMenuStrip(parent->topLevelParent()->windowPointer());
}

Object *ReactionMenu::addCreateMenu (string label) {
    detach();
    Object *panel = IIntuition->NewObject(NULL, "menuclass",
        MA_Type, 		T_MENU,
        MA_Label,		strdup(label.c_str()), //hiddeous hack
        TAG_END);

    if (menu) IIntuition->SetAttrs (menu, MA_AddChild, panel, TAG_END);
    attach();
    
    return panel;
}

void ReactionMenu::addCreateMenuItem (Object *panel, string label, string shortCut, int itemId) {
    detach();
    Object *item = IIntuition->NewObject(NULL, "menuclass",
        MA_ID,			itemId,
        MA_Type, 		T_ITEM,
        MA_Label,		strdup(label.c_str()),
        MA_Key,			strdup(shortCut.c_str()),
        TAG_END);

    if (item) IIntuition->SetAttrs (panel, MA_AddChild, item, TAG_END);
    attach();
}
