#ifndef BUTTON_h
#define BUTTON_h

#include <proto/intuition.h>

class ReactionWidget;
class ReactionLayout;
class ReactionButton {
public:
	static bool isButton(Object *o);

private:
	Object *button;

	Object *systemObject () { return button; }

private:
	const char *text;
	struct Icon *icon;
	
public:
	ReactionButton(ReactionWidget *parent, const char *text);
	~ReactionButton();
	
	void setText(const char *text);

public:
	friend ReactionWidget;
	friend ReactionLayout;	
};
#endif
