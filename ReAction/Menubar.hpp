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
    void addCreateMenuItem (MenuReference, string label, string shortCut, int itemId);

    virtual void createMenu() = 0;

    virtual bool handleMenuPick(int id) { return false; }

    friend class Widget;
    friend class MainWindow;
};
#endif
