#include <proto/intuition.h>
#include <proto/graphics.h>

#include "Screen.hpp"

#include <string>
#include <string.h>

using namespace std;

// NOTE: Could this be done shorter?
bool PublicScreen::usingPubScreen = false;
PublicScreen *PublicScreen::me = 0;
void cleanupPublicScreen() { delete PublicScreen::me; }
void __attribute__((destructor)) cleanupPublicScreen(); 

const unsigned int PublicScreen::penColors[] = {
	0xff000000, //		DEFAULT
	0xffaa0000, //		CRITICAL
	0xff0000aa, //		INFO
	0xff00aa00, //		OUTPUT
	0xffaa00aa, //		COMMAND
	0xffaaaa00  //		EVENT
};

PublicScreen::PublicScreen()
{
	usingPubScreen = false;
	screen = IIntuition->LockPubScreen(0);
	obtainScreenPens();
}

PublicScreen::~PublicScreen()
{
	closePublicScreen();
}

void PublicScreen::openPublicScreen (string _name, string title)
{
	if(usingPubScreen) {
		closePublicScreen();
	}
	
	screen = IIntuition->OpenScreenTags (0,
		SA_LikeWorkbench,		TRUE,
    	SA_PubName,				_name.c_str(),
		SA_Title,				strdup(title.c_str()),
		SA_ShowTitle,			TRUE,
		SA_WindowDropShadows,	FALSE,
		//SA_Quiet,				TRUE,
		TAG_END
	);

	IIntuition->PubScreenStatus(screen, 0);

	usingPubScreen = screen ? true : false;

	obtainScreenPens();
}

void PublicScreen::closePublicScreen ()
{
	if (usingPubScreen) IIntuition->CloseScreen (screen);
	screen = IIntuition->LockPubScreen(0);
	usingPubScreen = false;
}

int PublicScreen::screenWidth ()
{
	int screenWidth;
	IIntuition->GetScreenAttrs (screen, SA_Width, &screenWidth, TAG_DONE);
	return screenWidth;
}

int PublicScreen::screenHeight ()
{
	int screenHeight;
	IIntuition->GetScreenAttrs (screen, SA_Height, &screenHeight, TAG_DONE);
	return screenHeight;
}

int PublicScreen::screenBarHeight ()
{
	return screen->BarHeight + 1;
}

void PublicScreen::obtainScreenPens ()
{
	struct ColorMap *colorMap = screen->ViewPort.ColorMap;
	for (int i = 0; i < ScreenPens; i++)
		screenPens[i] = IGraphics->ObtainBestPen (colorMap, penColors[i] << 8, penColors[i] << 16, penColors[i] << 24, TAG_DONE);
}

void PublicScreen::releaseScreenPens ()
{
	struct ColorMap *colorMap = screen->ViewPort.ColorMap;
	for (int i = 0; i < ScreenPens; i++)
		IGraphics->ReleasePen (colorMap, screenPens[i]);
}

int PublicScreen::getPen (PenType type)
{
	return screenPens[type];
}
