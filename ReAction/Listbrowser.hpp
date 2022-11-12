#ifndef LISTBROWSER_h
#define LISTBROWSER_h

#include <proto/intuition.h>
#include <proto/listbrowser.h>
#include <gadgets/listbrowser.h>

#include <string>
#include <vector>
#include <list>

#include "Screen.hpp"

using namespace std;

class Widget;
class Layout;
class Listbrowser {
private:
	static list<Object *> listbrowsers;

public:
	static void clean() { listbrowsers.clear(); }
	static bool isListbrowser(Object *o);

private:
	struct List labels;
	Object *listbrowser;
	struct ColumnInfo *columnInfo;
	int noColumns;
	Widget *parent;

	PublicScreen::PenType penType;
	int gid; //only values btw 0 and 255

private:
	void init ();

	void *systemObject() { return listbrowser; }
	unsigned int getId();

public:
	Listbrowser(Widget *parent = 0);
	~Listbrowser();

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

	void attach();
	void detach();

	void addNode (string text, void *userData = 0, bool hasChildren = false, int generation = 0);
	void addNode (vector<string> columnTexts, void *userData = 0, bool hasChildren = false, int generation = 0);
    void addCheckboxNode (vector<string> columnTexts, bool checkbox = false, bool checked = false, void *userData = 0, bool hasChildren = false, int generation = 0);

	void showSelected (string text);
	
	string getNode(int line);
	
public:
	friend Widget;
	friend Layout;
};
#endif
