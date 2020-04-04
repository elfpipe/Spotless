#ifndef PANEL_h
#define PANEL_h

#include <vector>

#include <proto/intuition.h>

using namespace std;

class ReactionWidget;
class ReactionPanel {
public:
    typedef enum {
        PANEL_TABBED,
        PANEL_INDIVIDUAL
    } PanelState;

private:
    vector<ReactionWidget *> widgets;
    PanelState state;
    Object *object;

    ReactionWidget *parentWidget;

public:
    ReactionPanel(ReactionWidget *parent);
    ~ReactionPanel();

    int count() { return widgets.size(); }
    
    void setTabbed(bool tabbed);
    void addWidget(ReactionWidget *widget);
    ReactionWidget *getWidget(int index) { return widgets.at(index); }

    Object *createGuiObject();
};
#endif