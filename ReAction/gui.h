#ifndef REACTIONGUIHANDLER_HPP
#define REACTIONGUIHANDLER_HPP

#include <proto/intuition.h>

#include <string>
#include <list>

#include "screen.h"
#include "layout.h"
#include "widget.h"

using namespace std;

class ReactionMenu;

class ReactionGui {
public:
	static ReactionGui *getGui() {
		static ReactionGui *gui = 0;
		if (!gui)
			gui = new ReactionGui;
		return gui;
	}

public:
	static const int MaxRows = 6;

private:
	int windowWidth, windowHeight;

	struct Window *_windowPointer;
	Object *_windowObject;
	struct Hook idcmpHook;
	
	list<ReactionWidget *> widgets;
	int actualMaxRows;

	ReactionMenu *menu;

private:
	ReactionWidget *mouseOverWidget (int mouseX, int mouseY);
	void doMouseMove (int mouseX, int mouseY);

	Object *createEmbeddedWidgets();
	void showStageInterface();

	uint32 collectWidgetsSignalMasks();
	uint32 mainWindowSignalMask();
	bool handleMainWindowEvents ();
	bool handleWindowedEvents (ReactionWidget *widget);

public:
	ReactionGui();
	~ReactionGui();
	
	void createUserExperience();
	void destroyUserExperience();
	
	void addEmbeddedWidget (ReactionWidget *widget, int row = 0);
	ReactionWidget *findWidget(string name);

	void setMenu(ReactionMenu *_menu) { menu = _menu; }
	void attachMenu();
	void detachMenu();

	void mainWindowToFront();	

	void eventLoop();

	void showAboutWindow ();
	
	struct Window *windowPointer() { return _windowPointer; }
};
#endif
