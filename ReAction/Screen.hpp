#ifndef DB101_AmigaOSPublicScreen_h
#define DB101_AmigaOSPublicScreen_h

#include <proto/intuition.h>

#include <string>

using namespace std;

class PublicScreen {
private:
	static bool usingPubScreen;
	static PublicScreen *me;

public:
	static bool usingPublicScreen(){ return usingPubScreen; }

public:
	static PublicScreen *instance() {
		if(!me) me = new PublicScreen;
		return me;
	}

public:
	enum PenType {
		PENTYPE_DEFAULT = 0,
		PENTYPE_CRITICAL = 1,
		PENTYPE_INFO = 2,
		PENTYPE_OUTPUT = 3,
		PENTYPE_COMMAND = 4,
		PENTYPE_EVENT = 5,
		
		ScreenPens = 6
	};
	
private:
	struct Screen *screen;
	bool ownsScreen;
	string name;

private:
	int16 screenPens[ScreenPens];
	static const unsigned int penColors[ScreenPens];

public:
	PublicScreen();
	~PublicScreen();

	bool isOwner() { return ownsScreen; }
	string screenName() { return name; }
	struct Screen *screenPointer() { return screen; }

	void openPublicScreen (string name, string title);
	void closePublicScreen();

	int screenWidth();
	int screenHeight();
	int screenBarHeight();
	
	void obtainScreenPens();
	void releaseScreenPens();
	int getPen(PenType type);

	friend void cleanupPublicScreen();
};
#endif
