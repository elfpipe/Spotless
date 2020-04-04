#ifndef MAINWINDOW_h
#define MAINWINDOW_h

#include <exec/types.h>

#include <vector>

#include "widget.h"
#include "panel.h"

using namespace std;

class ReactionPanel;
class ReactionMenu;
class ReactionMainWindow : public ReactionWidget {
public:
    typedef enum {
        MAINWINDOW_UNIFIED,
        MAINWINDOW_SEPARATED
    } MainWindowState;

private:
    ReactionWidget *mainView;
    ReactionWidget *topBar;
    ReactionPanel *leftPanel;
    ReactionPanel *bottomPanel;
    ReactionPanel *rightPanel;

private:
    MainWindowState unityState;

    vector<ReactionWidget *> separateWidgets;
    
private:
    Object *createContent();

public:
    ReactionMainWindow();
    ~ReactionMainWindow();

    void openWindow();

    void setMainView(ReactionWidget *view);
    void setTopBar(ReactionWidget *top);

    void addLeftPanelWidget(ReactionWidget *widget);
    void addBottomPanelWidget(ReactionWidget *widget);
    void addRightPanelWidget(ReactionWidget *widget);

    void openSeparated();
    void closeSeparated();
    void waitForClose();

    void createGuiObject(ReactionLayout *layout) {} //dummy
};
#endif