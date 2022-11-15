#include <proto/button.h>
#include <gadgets/button.h>
#include <proto/string.h>
#include <gadgets/string.h>

#include <string>
#include <list>

#include "String.hpp"
#include "Widget.hpp"

#include <iostream>

using namespace std;

list<Object *> RString::strings;

RString::RString (Widget *parent, const char *content)
{	
	rstring = IIntuition->NewObject(NULL, "string.gadget",
		GA_UserData,	parent,
		GA_RelVerify,	true,
		GA_TabCycle,	true,
		STRINGA_MaxChars,		1024,
		STRINGA_TextVal,		content,
	TAG_DONE);

	strings.push_back(rstring);
	this->parent = parent;

	id = Widget::addChild(rstring);
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
	strings.remove(rstring);
	Widget::removeChild(rstring);
}

void RString::setContent (const char *newContent)
{
	// IIntuition->SetAttrs (rstring,
	// 	STRINGA_TextVal,	newContent,
	// TAG_DONE);
	// cout << "windowPointer : " << (void *)parent->topLevelParent()->windowPointer() << "\n";
	IIntuition->RefreshSetGadgetAttrs((struct Gadget *)rstring, parent->topLevelParent()->windowPointer(), 0,
		STRINGA_TextVal,	newContent,
	TAG_DONE);
}

string RString::getContent()
{
	char *content;
	IIntuition->GetAttrs (rstring,
		STRINGA_TextVal, &content,
	TAG_DONE);
	// cout << "getContent() : STRINGA_TextVal = " << content << "\n";
	return string(content);
}

bool RString::isString(Object *o)
{
	for(list<Object *>::iterator it = strings.begin(); it != strings.end(); it++) {
		if((*it) == o) return true;
	}
	return false;
}