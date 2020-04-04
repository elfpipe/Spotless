#ifndef WIDGET_h
#define WIDGET_h

#include <proto/intuition.h>

#include <string>
#include <vector>

class ReactionMenu;
class ReactionPanel;
class ReactionMainWindow;
class ReactionLayout;
class ReactionButton;
class ReactionSpeedbar;
class ReactionListbrowser;
class GuiEvent;

using namespace std;

class ReactionWidget {
private:
	Object *object;
	struct Window *window;

	Object *createContent();

	uint32 windowSignalMask();
	bool processEvent (uint32 Class, uint16 Code);

	struct Window *windowPointer() { return window; }
	Object *windowObject() { return object; }

private:
	unsigned int gadgetId;
	vector<Object *> children; //We need this to delegate input events

	void addChild(Object *object);
	Object *findChild(unsigned int id);

private:
	string widgetName;

	ReactionLayout *parentLayout;
	ReactionMenu *mainMenu;
	ReactionWidget *parentWidget;

	ReactionWidget *parent() { return parentWidget; }
	void setParent(ReactionWidget *parent) { parentWidget = parent; }
	ReactionWidget *topLevelParent();

public:
	ReactionWidget(ReactionWidget *parentWidget = 0);
	~ReactionWidget();

	void setParentLayout(ReactionLayout *layout) { parentLayout = layout; }	
	void setMenu(ReactionMenu *menu);

	void setName(string name) { widgetName = name; }
	string name() { return widgetName; }
		
	virtual void waitForClose();
	virtual void openWindow();
	void closeWindow();
	//void embedWidget();
	
	void iconify();
	void uniconify();
	void windowToFront();

	virtual void createGuiObject (ReactionLayout *layout) = 0;

	virtual bool handleGuiEvent (GuiEvent *event) { return false; };

public:
	friend ReactionMainWindow;
	friend ReactionListbrowser;
	friend ReactionSpeedbar;
	friend ReactionLayout;
	friend ReactionButton;
	friend ReactionPanel;
	friend ReactionMenu;
};
#endif
