#ifndef WIDGET_h
#define WIDGET_h

#include <proto/intuition.h>

#include <string>
#include <vector>

class Menubar;
class Panel;
class MainWindow;
class Layout;
class GoButton;
class Speedbar;
class Listbrowser;
class GuiEvent;

using namespace std;

class Widget {
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

	Layout *parentLayout;
	Widget *parentWidget;
	Menubar *mainMenu;

	Widget *parent() { return parentWidget; }
	Widget *topLevelParent();
	void setParent(Widget *parent) { parentWidget = parent; }

public:
	Widget(Widget *parentWidget = 0);
	~Widget();

	void setParentLayout(Layout *layout) { parentLayout = layout; }	
	void setMenubar(Menubar *menu);

	void setName(string name) { widgetName = name; }
	string name() { return widgetName; }
		
	virtual void waitForClose();
	virtual void openWindow();
	void closeWindow();
	
	void iconify();
	void uniconify();
	void windowToFront();

	virtual void createGuiObject (Layout *layout) = 0;

	virtual bool handleGuiEvent (GuiEvent *event) { return false; };

public:
	friend MainWindow;
	friend Listbrowser;
	friend Speedbar;
	friend Layout;
	friend GoButton;
	friend Panel;
	friend Menubar;
};
#endif
