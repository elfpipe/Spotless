#include <intuition/menuclass.h>

#include <string.h>

#include "Menubar.hpp"
#include "Widget.hpp"

Menubar::Menubar(Widget *parent)
    : menu(0),
    created(false)
{
    this->parent = parent;
    // createMenuStrip();
}

Menubar::~Menubar()
{
    destroyMenuStrip();
}

void Menubar::createMenuStrip () {
    menu = IIntuition->NewObject(NULL, "menuclass", MA_Type, T_ROOT, TAG_END);
    // created = true;
}

void Menubar::destroyMenuStrip () {
    if (menu) IIntuition->DisposeObject(menu);
    menu = 0;
    created = false;
}

void Menubar::attach() {
    if(parent && parent->windowPointer()) IIntuition->SetMenuStrip(parent->topLevelParent()->windowPointer(), (struct Menu *)menu);
}

void Menubar::detach() {
    if(parent && parent->windowPointer()) IIntuition->ClearMenuStrip(parent->topLevelParent()->windowPointer());
}

Object *Menubar::addCreateMenu (string label) {
    detach();
    Object *panel = IIntuition->NewObject(NULL, "menuclass",
        MA_Type, 		T_MENU,
        MA_Label,		strdup(label.c_str()), //hiddeous hack
        TAG_END);

    if (menu) IIntuition->SetAttrs (menu, MA_AddChild, panel, TAG_END);
    attach();
    
    return panel;
}

Object *Menubar::addCreateMenuItem (Object *panel, string label, string shortCut, int itemId, bool toggle, bool selected) {
    detach();
    Object *item = IIntuition->NewObject(NULL, "menuclass",
        MA_ID,			                            itemId,
        MA_Type, 		                            T_ITEM,
        MA_Label,		                            strdup(label.c_str()),
        shortCut.length() ? MA_Key : TAG_IGNORE,    strdup(shortCut.c_str()),
        MA_Toggle,                                  toggle,
        MA_Selected,                                selected,
        TAG_END);

    if (item) IIntuition->SetAttrs (panel, MA_AddChild, item, TAG_END);
    attach();
    return item;
}

void Menubar::addSeparator(Object *panel) {
    detach();
    Object *item = IIntuition->NewObject(NULL,"menuclass",
        MA_Type,                                    T_ITEM,
        MA_Label,                                   ML_SEPARATOR,
        TAG_END);

    if (item) IIntuition->SetAttrs (panel, MA_AddChild, item, TAG_END);
    attach();
}

bool Menubar::isSelected(Object *item) {
    uint32 selected;
    IIntuition->GetAttr (MA_Selected, item, &selected);
    return selected;
}

void Menubar::setSelected(Object *item, bool selected) {
    if(item) IIntuition->SetAttrs (item, MA_Selected, selected, TAG_DONE);
}