#ifndef Progress_hpp
#define Progress_hpp

#include <exec/types.h>
#include <proto/intuition.h>

class ReactionProgressWindow {
private:
	Object *_windowObject;
	Object *_fuelGaugeObject;
	
	struct Window *_windowPointer;
		
	int _oldLevel;
	int _total;
	
public:
	
	ReactionProgressWindow();
	~ReactionProgressWindow();
	
	void open (const char *, int, int);
	void updateParameters (const char *, int, int);
	void updateLevel (int);
	void close ();
};
#endif