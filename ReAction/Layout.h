#ifndef LAYOUT_h
#define LAYOUT_h

#include <proto/intuition.h>

#include "button.h"
#include "speedbar.h"
#include "listbrowser.h"

#include <string>

class ReactionMainWindow;
class ReactionWidget;
class ReactionPanel;

using namespace std;

class ReactionLayout {
public:
	typedef enum {
		LAYOUT_Vertical,
		LAYOUT_Horizontal
	}  LayoutDirection;

private:
	LayoutDirection direction;
	Object *layout;

	ReactionWidget *parent;

private:
	void clear();

	Object *systemObject() { return layout; }

public:
	ReactionLayout(ReactionWidget *parent, LayoutDirection direction = LAYOUT_Horizontal);
	ReactionLayout (ReactionWidget *parent, string label);
	~ReactionLayout();

	void getDimensions(int *, int *, int *, int *);

	ReactionLayout *createVerticalLayout ();
	ReactionLayout *createHorizontalLayout ();
	ReactionLayout *createLabeledLayout (string label);

	void createSpace();

	ReactionListbrowser *createListbrowser ();
	ReactionSpeedbar *createSpeedbar ();
	ReactionButton *createButton (const char *text);

	void addEmbeddedWidget(ReactionWidget *widget);
	void addTabbedPanel(ReactionPanel *panel, int weight);
	void addWeightBar();

public:
	friend ReactionMainWindow;
	friend ReactionWidget;
	friend ReactionPanel;
};
#endif
