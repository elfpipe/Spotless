#ifndef MAINWINDOW_h
#define MAINWINDOW_h

#include <exec/types.h>

#include <vector>

#include "Widget.hpp"
#include "Panel.hpp"

using namespace std;

class Panel;
class Menu;
class MainWindow : public Widget {
private:
    Widget *mainView;
    Widget *topBar;
    Panel *leftPanel;
    Panel *bottomPanel;
    Panel *rightPanel;
    
private:
    Object *createContent();

public:
    MainWindow();
    ~MainWindow();

    bool openWindow();

    void setMainView(Widget *view);
    void setTopBar(Widget *top);

    void addLeftPanelWidget(Widget *widget);
    void addBottomPanelWidget(Widget *widget);
    void addRightPanelWidget(Widget *widget);

    void createGuiObject(Layout *layout) {} //dummy
};
#endif