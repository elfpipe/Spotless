#ifndef MENU_h
#define MENU_h

#include <proto/intuition.h>
#include <string>

using namespace std;

class ReactionWidget;
class ReactionMenu {
public:
    typedef Object *MenuReference;

private:
    Object *menu;
    ReactionWidget *parent;

    void attach();
    void detach();

    void createMenuStrip ();
    void destroyMenuStrip ();

    Object *systemObject () { return menu; }

public:
    ReactionMenu(ReactionWidget *parent);
    ~ReactionMenu();        

    ReactionWidget *parentWidget() { return parent; }
    
    MenuReference addCreateMenu (string label);
    void addCreateMenuItem (MenuReference, string label, string shortCut, int itemId);

    virtual void createMenu() = 0;

    virtual bool handleMenuPick(int id) { return false; }

    friend class ReactionWidget;
    friend class ReactionMainWindow;
};
#endif
