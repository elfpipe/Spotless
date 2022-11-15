#ifndef MENU_h
#define MENU_h

#include <proto/intuition.h>
#include <string>

using namespace std;

class Widget;
class Menubar {
public:
    typedef Object *MenuReference;

private:
    Object *menu;
    Widget *parent;
    bool created;

public:
    bool isCreated() { return created; }

private:
    void attach();
    void detach();

    void createMenuStrip ();
    void destroyMenuStrip ();

    Object *systemObject () { return menu; }

public:
    Menubar(Widget *parent);
    ~Menubar();        

    Widget *parentWidget() { return parent; }
    
    MenuReference addCreateMenu (string label);
    Object *addCreateMenuItem (MenuReference, string label, string shortCut, int itemId, bool toggle = false, bool selected = false);
    void addSeparator (MenuReference);

    bool isSelected(Object *item);
    void setSelected(Object *item, bool selected);
    
    virtual void createMenu() = 0;

    virtual bool handleMenuPick(int id, bool *openClose, bool *exit) { *openClose = false; *exit = false; return false; }

    // bool switchScreen() { return doScreenSwitch; }

    virtual void update() {}

    friend class Widget;
    friend class MainWindow;
    friend class MainMenu;
};
#endif
