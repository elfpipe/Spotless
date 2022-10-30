#ifndef PANEL_h
#define PANEL_h

#include <vector>

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
    vector<Widget *> widgets;
    PanelState state;
    Object *object;

    Widget *parentWidget;

public:
    Panel(Widget *parent);
    ~Panel();

    int count() { return widgets.size(); }
    
    void setTabbed(bool tabbed);
    void addWidget(Widget *widget);
    Widget *getWidget(int index) { return widgets.at(index); }

    Object *createGuiObject();

    friend class MainWindow;
};
#endif