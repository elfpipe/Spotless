#ifndef LAYOUT_h
#define LAYOUT_h

#include <proto/intuition.h>

#include "Button.hpp"
#include "Speedbar.hpp"
#include "Listbrowser.hpp"
#include "String.hpp"
#include "Checkbox.hpp"

#include <string>

class MainWindow;
class Widget;
class Panel;

using namespace std;

class Layout {
public:
	typedef enum {
		LAYOUT_Vertical,
		LAYOUT_Horizontal
	}  LayoutDirection;

private:
	LayoutDirection direction;
	Object *layout;

	Widget *parent;

private:
	void clear();

	Object *systemObject() { return layout; }

public:
	Layout(Widget *parent, LayoutDirection direction = LAYOUT_Horizontal);
	Layout(Widget *parent, string label);
	~Layout();

	void setParent(Widget *newParent) { parent = newParent; }
	
	void getDimensions(int *, int *, int *, int *);

	Layout *createVerticalLayout (int weightedHeight = 100, int weightedWidth = 100);
	Layout *createHorizontalLayout (int weightedHeight = 100, int weightedWidth = 100);
	Layout *createLabeledLayout (string label);

	void createSpace();

	Listbrowser *createListbrowser ();
	Speedbar *createSpeedbar ();
	RButton *createButton (const char *text, const char *image = "");
	RString *createString (const char *content);
	Checkbox *createCheckbox (const char *text, bool checked = false);

	void addEmbeddedWidget(Widget *widget);
	void addTabbedPanel(Panel *panel, int weight);
	void addWeightBar();
	
public:
	friend MainWindow;
	friend Widget;
	friend Panel;
};
#endif
