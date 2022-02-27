#include <proto/button.h>
#include <gadgets/button.h>
#include <proto/string.h>
#include <gadgets/string.h>

#include <string>

#include "String.hpp"
#include "Widget.hpp"

#include <iostream>

RString::RString (Widget *parent, const char *content)
{	
	rstring = IIntuition->NewObject(NULL, "string.gadget",
		GA_UserData,	parent,
		GA_RelVerify,	true,
		GA_TabCycle,	true,
		STRINGA_MaxChars,		1023,
		STRINGA_TextVal,		content,
	TAG_DONE);

	parent->topLevelParent()->addChild(rstring);
}

// gb_StringGad = (struct Gadget *) IIntuition->NewObject(IString->STRING_GetClass(),NULL,
//       GA_ID,                      GAD_ID_String,
//       GA_RelVerify,               TRUE,
//       GA_TabCycle,                TRUE,
//       STRINGA_MaxChars,           30+1,
//       STRINGA_MinVisible,         30,
//       STRINGA_TextVal,            "Amiga 1000",
//       TAG_END);

RString::~RString ()
{
}

void RString::setContent (const char *newContent)
{
	IIntuition->SetAttrs (rstring,
		STRINGA_TextVal,	newContent,
	TAG_DONE);
}

string RString::getContent()
{
	char *content;
	IIntuition->SetAttrs (rstring,
		STRINGA_TextVal, &content,
	TAG_DONE);
	return string(content);
}