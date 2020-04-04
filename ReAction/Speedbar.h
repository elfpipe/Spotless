#ifndef BUTTONBAR_h
#define BUTTONBAR_h

#include <proto/intuition.h>

#include <string>
#include <vector>

using namespace std;

class ReactionWidget;
class ReactionLayout;
class ReactionSpeedbar {
private:
	static bool isSpeedbar(Object *o);

private:
	struct List buttonList;
	Object *speedbar;
	int gid;

	ReactionWidget *parent;

	void addNode (struct Node *node);
	
	void init ();
	void clear ();
	
	void attach();
	void detach();

	struct Gadget *findButtonGadget (uint16 id);	

	Object *systemObject () { return speedbar; }

public:
	ReactionSpeedbar(ReactionWidget *parent);
	~ReactionSpeedbar();

	void addButton (int id, string buttonText, string iconName = string());	
	void addSpacer ();

	void enableButton (int id, bool enable);
	void setButtonText (int id, string text);

public:
	friend ReactionWidget;
	friend ReactionLayout;
};
#endif
