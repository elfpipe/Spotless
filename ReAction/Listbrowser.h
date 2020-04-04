#ifndef LISTBROWSER_h
#define LISTBROWSER_h

#include <proto/intuition.h>
#include <proto/listbrowser.h>
#include <gadgets/listbrowser.h>

#include <string>
#include <vector>

#include "screen.h"

using namespace std;

class ReactionWidget;
class ReactionLayout;
class ReactionListbrowser {
private:
	static bool isListbrowser(Object *o);

private:
	struct List labels;
	Object *object;
	struct ColumnInfo *columnInfo;
	int noColumns;
	ReactionWidget *parent;

	PublicScreen::PenType penType;
	int gid; //only values btw 0 and 255

private:
	void init ();
	void attach();
	void detach();

	void *systemObject() { return object; }
	unsigned int getId();

public:
	ReactionListbrowser(ReactionWidget *parent = 0);
	~ReactionListbrowser();

	void clear();

	void setColumnTitles (const char *titlesString);
	void freeColumnTitles();

	void setHierachical (bool enable);
	void setStriping (bool enable);

	void scrollToBottom();
	void focus (int line);

	void *getSelectedNodeData();
    void *getUserData (int lineNumber);
    int getSelectedLineNumber();
    bool checkboxChecked();

	void setPen (PublicScreen::PenType pen);

	void addNode (string text, void *userData = 0, bool hasChildren = false, int generation = 0);
	void addNode (vector<string> columnTexts, void *userData = 0, bool hasChildren = false, int generation = 0);
    void addCheckboxNode (vector<string> columnTexts, bool checkbox, bool checked, void *userData, bool hasChildren, int generation);

public:
	friend ReactionWidget;
	friend ReactionLayout;
};
#endif
