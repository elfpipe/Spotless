#ifndef CHECKBOX_h
#define CHECKBOX_h

#include <proto/intuition.h>
#include <gadgets/checkbox.h>

#include <list>
using namespace std;

class Widget;
class Layout;
class Checkbox {
private:
	static list<Object *> checkboxes;

public:
	static bool isCheckbox(Object *o);
	static void clean() { checkboxes.clear(); }

private:
	Object *checkbox;
	Object *systemObject () { return checkbox; }

private:
	// const char *text;
	unsigned int id;

public:
	Checkbox(Widget *parent, const char *text, bool checked = false);
	~Checkbox();
	
	// void setText(const char *text);
	// const char *getText();

    void setChecked(bool checked);
    bool getChecked();

	unsigned int getId() { return id; }

public:
	friend Widget;
	friend Layout;	
};
#endif
