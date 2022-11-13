#ifndef BUTTONBAR_h
#define BUTTONBAR_h

#include <proto/intuition.h>

#include <string>
#include <vector>

using namespace std;

class Widget;
class Layout;
class Speedbar {
private:
	static list<Object *> speedbars;
public:
	static void clean() { speedbars.clear(); }
	static bool isSpeedbar(Object *o);

private:
	struct List buttonList;
	Object *speedbar;
	// int gid;

	Widget *parent;

	void addNode (struct Node *node);
	
	void init ();
	void clear ();
	
	void attach();
	void detach();

	struct Gadget *findButtonGadget (uint16 id);	

	Object *systemObject () { return speedbar; }

public:
	Speedbar(Widget *parent);
	~Speedbar();

	void addButton (int id, string buttonText, string iconName = string());	
	void addSpacer ();

	void enableButton (int id, bool enable);
	void setButtonText (int id, string text);

public:
	friend Widget;
	friend Layout;
};
#endif
