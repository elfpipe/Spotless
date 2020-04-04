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

	Widget *parent() { return parentWidget; }
	Widget *topLevelParent();
	void setParent(Widget *parent) { parentWidget = parent; }

	Menubar *mainMenu;

public:
	typedef void (*Handler)();
	struct HandlerReference {
		Handler handler;
		uint32_t signal;
		HandlerReference(Handler handler, uint32_t signal) { this->handler = handler; this->signal = signal; }
	};
	vector<HandlerReference *> handlers;
	uint32_t handlerSignals() {
		uint32_t result = 0x0;
		for(int i = 0; i < handlers.size(); i++)
			result |= handlers[i]->signal;
	}

public:
	Widget(Widget *parentWidget = 0);
	~Widget();

	void setParentLayout(Layout *layout) { parentLayout = layout; }	
	void setMenubar(Menubar *menu);

	void setName(string name) { widgetName = name; }
	string name() { return widgetName; }
		
	virtual int waitForClose();
	virtual void openWindow();
	void closeWindow();
	
	void iconify();
	void uniconify();
	void windowToFront();

	void addSignalHandler(Handler *handler, uint32_t signal) {
		handlers.push_back(new HandlerReference(handler, signal));
	}

	virtual void createGuiObject (Layout *layout) = 0;

	virtual bool handleEvent (Event *event) { return false; };

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
