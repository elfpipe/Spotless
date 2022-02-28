#ifndef BUTTON_h
#define BUTTON_h

#include <proto/intuition.h>

#include <list>
using namespace std;

class Widget;
class Layout;
class RButton {
private:
	static list<Object *> buttons;

public:
	static bool isButton(Object *o);

private:
	Object *button;
	Object *systemObject () { return button; }

private:
	const char *text;
	struct Icon *icon;
	unsigned int id;

public:
	RButton(Widget *parent, const char *text, const char *iconName = "");
	~RButton();
	
	void setText(const char *text);
	const char *getText();

	unsigned int getId() { return id; }

public:
	friend Widget;
	friend Layout;	
};
#endif
