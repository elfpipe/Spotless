#include <proto/intuition.h>
#include <proto/requester.h>
#include <proto/asl.h>
#include <classes/requester.h>

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <iostream>

#include "screen.h"
#include "requester.h"
#include "../Service/stringtools.h"

using namespace std;

std::string RequesterTools::requesterFile (RequesterCategory category, std::string initialFile, std::string &pathResult, const char *format, ...)
{
	va_list argptr;
	va_start (argptr, format);
	std::string titleText = printStringFormat_helper (format, argptr);
	va_end (argptr);

	struct Screen *publicScreen = PublicScreen::instance()->screenPointer();

	struct FileRequester *requester = (struct FileRequester *)IAsl->AllocAslRequestTags (ASL_FileRequest,
		ASLFR_Screen,				publicScreen,
		ASLFR_TitleText,			titleText.c_str(),
		ASLFR_InitialDrawer,		pathBuffer[category].c_str(),
		ASLFR_InitialFile,			initialFile.c_str(),
		TAG_DONE);
	
	if (!requester) {
		cout << "Failed to allocate ASL requester\n";
		return std::string();
	}
	
	if (!IAsl->AslRequest (requester, 0)) { //if user aborts
		return std::string();
	}

	std::string fileName (requester->fr_File);
	
	pathBuffer[category] = std::string (requester->fr_Drawer);
	pathResult = pathBuffer[category];
	
	IAsl->FreeAslRequest (requester);
	
	return fileName;
}

std::string RequesterTools::requesterPath (enum RequesterCategory category, const char *format, ...)
{
	va_list argptr;
	va_start (argptr, format);
	std::string titleText = printStringFormat_helper (format, argptr);
	va_end (argptr);
	
	struct Screen *publicScreen = PublicScreen::instance()->screenPointer();

	struct FileRequester *requester = (struct FileRequester *)IAsl->AllocAslRequestTags (ASL_FileRequest,
		ASLFR_DrawersOnly,		true,
		ASLFR_Screen,			publicScreen,
		ASLFR_TitleText,		titleText.c_str(),
		ASLFR_InitialDrawer,	pathBuffer[category].c_str(),
		TAG_DONE);
	
	if (!requester) {
		cout << "Failed to allocate ASL requester\n";
		return std::string();
	}
	
	if (!IAsl->AslRequest (requester, 0)) {
		return std::string();
	}

	std::string result (requester->fr_Drawer);
	pathBuffer[category] = std::string (requester->fr_Drawer);

	IAsl->FreeAslRequest (requester);	
	return result;
}

int RequesterTools::requesterChoice(const char *title, const char *gadgetsText, const char *format, ...)
{
	va_list argptr;
	va_start (argptr, format);
	std::string content = printStringFormat_helper (format, argptr);
	va_end (argptr);

	struct Screen *publicScreen = PublicScreen::instance()->screenPointer();

	Object *requesterObject = (Object *) IIntuition->NewObject (IRequester->REQUESTER_GetClass(), 0,
		REQ_Type,		REQTYPE_INFO,
		REQ_TitleText,	title,
		REQ_BodyText, 	content.c_str(),
		REQ_GadgetText,	gadgetsText,
		TAG_DONE);

	uint32 code;
	if (requesterObject) {
		IIntuition->IDoMethod (requesterObject, RM_OPENREQ, 0, 0, publicScreen);
		IIntuition->GetAttr (REQ_ReturnCode, requesterObject, &code);
		IIntuition->DisposeObject (requesterObject);
	}
	return code;
}

std::string RequesterTools::requesterString (const char *title, const char *format, ...)
{
	va_list argptr;
	va_start (argptr, format);
	std::string content = printStringFormat_helper (format, argptr);
	va_end (argptr);

	char buffer[MAX_CHAR_BUFFER_SIZE];	
	memset(buffer, 0, MAX_CHAR_BUFFER_SIZE);

	struct Screen *publicScreen = PublicScreen::instance()->screenPointer();

	Object *requesterObject = (Object *) IIntuition->NewObject (IRequester->REQUESTER_GetClass(), 0,
		REQ_Type,       	REQTYPE_STRING,
		REQ_TitleText,  	title,
		REQS_Buffer,		buffer,
		REQS_MaxChars,		MAX_CHAR_BUFFER_SIZE-1,
		REQS_AllowEmpty,	true,
		REQ_BodyText,		content.c_str(),
		REQ_GadgetText,		"Ok",
		TAG_DONE);

	uint32 code;
	if (requesterObject) {
		IIntuition->IDoMethod (requesterObject, RM_OPENREQ, 0, 0, publicScreen);
		IIntuition->GetAttr (REQ_ReturnCode, requesterObject, &code);
		IIntuition->DisposeObject (requesterObject);
	}
	
	return std::string (buffer);
}

void RequesterTools::showAboutWindow ()
{
	struct Screen *publicScreen = PublicScreen::instance()->screenPointer();
	
	Object *requesterObject = (Object *)IIntuition->NewObject( IRequester->REQUESTER_GetClass(), NULL,
		REQ_Type,       REQTYPE_INFO,
		REQ_TitleText,  "About",
		REQ_BodyText,   "Spotless - Copyright © 2020 Alpha Kilimanjaro",
		REQ_GadgetText, "Ok",
		TAG_DONE);

	if (requesterObject) {
		IIntuition->IDoMethod (requesterObject, RM_OPENREQ, 0, 0, publicScreen);
		IIntuition->DisposeObject (requesterObject);
	}
}
