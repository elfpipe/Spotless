#ifndef WIDGET_h
#define WIDGET_h

#include <proto/intuition.h>

#include <string>
#include <vector>
#include <list>
using namespace std;

class Menubar;
class Panel;
class MainWindow;
class Layout;
class RButton;
class Speedbar;
class Listbrowser;
class Event;
class RString;
class Checkbox;
class Spotless;

using namespace std;

class Widget {
private:
	Object *object;
	struct Window *window;
	static list<Widget *> openedWindows;

	Object *createContent();
	void destroyContent();
	
	uint32 windowSignalMask();
	uint32 openedWindowsSignalMask();

	bool processEvent (uint32 Class, uint16 Code, bool *exit);

	struct Window *windowPointer() { return window; }
	Object *windowObject() { return object; }

private:
	bool isOpen;
public:
	bool open() { return isOpen; }

public:
	static unsigned int gadgetId;
private:
	static list<Object *> children; //We need this to delegate input events

	static unsigned int addChild(Object *object); //return id
	static Object *findChild(unsigned int id);
	static void removeChild(Object *object) { children.remove(object); }

	// Widget *findOpenedWindowWidget(uint32 mask);

    struct MsgPort *appPort;

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
		return result;
	}

public:
	Widget(Widget *parentWidget = 0);
	~Widget();

	void setParentLayout(Layout *layout) { parentLayout = layout; }	
	void setMenubar(Menubar *menu);

	void setName(string name);
	string name() { return widgetName; }
		
	int waitForClose();
	virtual bool openWindow();
	// bool openNewWindow(Widget *);

	virtual void closeWindow();
	// void closeNewWindow(Widget *);

	void closeAllWindows();
	void closeAllExceptThis();
	
	void iconify();
	void uniconify();
	void windowToFront();

	void addSignalHandler(Handler handler, uint32_t signal) {
		handlers.push_back(new HandlerReference(handler, signal));
	}

	virtual void createGuiObject (Layout *layout) = 0;

	virtual bool handleEvent (Event *event, bool *exit) { return false; };

public:
	friend MainWindow;
	friend Listbrowser;
	friend Speedbar;
	friend Layout;
	friend RButton;
	friend Panel;
	friend Menubar;
	friend RString;
	friend Checkbox;
	friend Spotless;
};
#endif
