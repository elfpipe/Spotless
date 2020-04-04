#include "Progress.hpp"
#include "Screen.hpp"

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/window.h>
#include <proto/layout.h>
#include <proto/fuelgauge.h>

#include <classes/window.h>
#include <gadgets/fuelgauge.h>

#include <reaction/reaction_macros.h>

//----------------------------------

ProgressWindow::ProgressWindow()
: _windowObject(0)
{
}

ProgressWindow::~ProgressWindow()
{
}

void ProgressWindow::open (const char *windowTitle, int total, int initial)
{
	if (_windowObject) {
		updateParameters (windowTitle, total, initial);
		return;
	}
	
	_oldLevel = initial;
	_total = total;

	struct Screen *publicScreen = PublicScreen::instance()->screenPointer();

	_windowObject = WindowObject,
		WA_PubScreen,			publicScreen,
		WA_ScreenTitle,			"Spotless",
		WA_Title,				windowTitle,
		WA_Activate,			TRUE,
		WA_DepthGadget,			TRUE,
		WA_DragBar,				TRUE,
		WA_Width,				300,
		WINDOW_Position,		WPOS_CENTERSCREEN,
		WINDOW_ParentGroup,		HLayoutObject,
			//LAYOUT_SpaceOuter,	TRUE,
			//LAYOUT_DeferLayout,	TRUE,

			LAYOUT_AddChild, _fuelGaugeObject = FuelGaugeObject,
				//GA_ID,					GID_FUELGAUGE,
				FUELGAUGE_Orientation,	FGORIENT_HORIZ,
				FUELGAUGE_Min,			0,
				FUELGAUGE_Max,			total,
				FUELGAUGE_Level,		initial,
				FUELGAUGE_Percent,		TRUE,
				FUELGAUGE_TickSize,		5,
				FUELGAUGE_Ticks,		5,
				FUELGAUGE_ShortTicks,	TRUE,
			FuelGaugeEnd,
		EndGroup,
	EndWindow;

	if (_windowObject) {
		_windowPointer = (struct Window *) RA_OpenWindow (_windowObject);
		if (_windowPointer) {
//			IIntuition->SetWindowAttr (Global::mainWindow (), WA_BusyPointer, (APTR)TRUE, 0);
		}
	}
}

void ProgressWindow::updateParameters (const char * windowTitle, int total, int level)
{
	if (!_windowObject)
		return;
	
	IIntuition->SetGadgetAttrs ((struct Gadget *)_fuelGaugeObject, _windowPointer, NULL,
		FUELGAUGE_Max,		total,
		FUELGAUGE_Level,	level,
		TAG_DONE
	);
	
	IIntuition->SetAttrs (_windowObject, WA_Title, windowTitle, TAG_END);
	
	_total = total;
	_oldLevel = level;
}

void ProgressWindow::updateLevel (int level)
{
	if(!_windowObject)
		return;
	
	if (_total / (level - _oldLevel) <= 20) {
		IIntuition->SetGadgetAttrs ((struct Gadget *)_fuelGaugeObject, _windowPointer, NULL,
			FUELGAUGE_Level, level,
			TAG_DONE
		);
		_oldLevel = level;
	}
}

void ProgressWindow::close ()
{	
	if (_windowObject) IIntuition->DisposeObject (_windowObject);
	_windowObject = 0;
	
	// TODO
	// IIntuition->SetWindowAttr (Global::mainWindow (), WA_BusyPointer, (APTR)FALSE, 0);
}
// ----------------------------------------------
