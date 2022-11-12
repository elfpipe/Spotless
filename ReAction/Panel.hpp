#ifndef PANEL_h
#define PANEL_h

#include <vector>
#include <list>

#include <proto/intuition.h>

using namespace std;

class Widget;
class Panel {
public:
    typedef enum {
        PANEL_TABBED,
        PANEL_INDIVIDUAL
    } PanelState;

private:
    list<Widget *> widgets;
    PanelState state;
    Object *object;

    Widget *parentWidget;

public:
    Panel(Widget *parent, list<Widget *> widgets);
    ~Panel();

    int count() { return widgets.size(); }
    
    void setTabbed(bool tabbed);
    void addWidget(Widget *widget);
    // Widget *getWidget(int index) { return widgets[index]; }

    Object *createGuiObject();

    friend class MainWindow;
};
#endif