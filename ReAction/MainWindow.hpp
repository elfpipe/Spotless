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
    // Panel *leftPanel;
    // Panel *bottomPanel;
    // Panel *rightPanel;
    list<Widget *> leftPanel;
    list<Widget *> bottomPanel;
    list<Widget *> rightPanel;

    list<Widget *> extraWindows;

    // Layout *mainLayout;
    
    static bool split;

private:
    Object *createContent();
    void destroyContent();

public:
    MainWindow();
    ~MainWindow();

    bool openExtraWindow(Widget *widget);
    bool closeExtraWindow(Widget *widget);
    
    bool openWindow();
    void closeWindow();
    bool showSplit();
    bool isSplit() { return split; }
    void setSplit(bool split) { this->split = split; }
        
    void setMainView(Widget *view);
    void setTopBar(Widget *top);

    void addLeftPanelWidget(Widget *widget);
    void addBottomPanelWidget(Widget *widget);
    void addRightPanelWidget(Widget *widget);

    void createGuiObject(Layout *layout) {} //dummy
};
#endif