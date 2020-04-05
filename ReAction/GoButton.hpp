#ifndef BUTTON_h
#define BUTTON_h

#include <proto/intuition.h>

class Widget;
class Layout;
class GoButton {
public:
	static bool isButton(Object *o);

private:
	Object *button;

	Object *systemObject () { return button; }

private:
	const char *text;
	struct Icon *icon;
	
public:
	GoButton(Widget *parent, int id, const char *text);
	~GoButton();
	
	void setText(const char *text);

public:
	friend Widget;
	friend Layout;	
};
#endif
