#ifndef GUIEVENT_h
#define GUIEVENT_h

#include <string>

using namespace std;

class Event {
public:
	enum EventClass {
		CLASS_NoClass = 0,
		CLASS_MouseButtonDown = 1,
		CLASS_MouseMove,
		CLASS_ButtonPress,
		CLASS_CheckboxCheck,
		CLASS_CheckboxUncheck,
		CLASS_SelectNode
	};
private:
	EventClass eClass;
	unsigned int id, iid;
	int mousex, mousey;
	string description;

public:
	Event(EventClass eClass) { setEventClass(eClass); }
	~Event() { }

	EventClass eventClass() { return eClass; }
	void setEventClass (EventClass eventClass) { eClass = eventClass; }
	
	unsigned int elementId() { return id; }
	void setElementId (unsigned int id) { this->id = id; }
	
	unsigned int itemId() { return iid; }
	void setItemId (unsigned int iid) { this->iid = iid; }

	string elementDescription() { return description; }
	void setElementDescription(string description) { this->description = description; }

	int mouseX() { return mousex; }
	int mouseY() { return mousey; }
	void setMousePosition(int mouseX, int mouseY) { mousex = mouseX; mousey = mouseY; }
};
#endif
