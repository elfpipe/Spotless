#include <proto/checkbox.h>
#include <gadgets/checkbox.h>

#include <reaction/reaction_macros.h>

// #include <string>
// #include <string.h>
#include <iostream>

#include "Checkbox.hpp"
#include "Widget.hpp"

using namespace std;

list<Object *> Checkbox::checkboxes;

Checkbox::Checkbox (Widget *parent, const char *text, bool checked)
{
    checkbox = IIntuition->NewObject(NULL, "checkbox.gadget",
        GA_UserData,	    parent,
        GA_RelVerify,	    true,
        GA_Text,		    text,
        CHECKBOX_Checked,   checked,
    TAG_DONE);

	// this->text = text;
	checkboxes.push_back(checkbox);

	id = parent->topLevelParent()->addChild(checkbox);
}

Checkbox::~Checkbox ()
{
}

void Checkbox::setChecked(bool checked)
{
	IIntuition->SetAttrs (checkbox,
        CHECKBOX_Checked,   checked,
	TAG_DONE);
}

bool Checkbox::getChecked()
{
    ULONG checked;
    IIntuition->GetAttr (CHECKBOX_Checked, checkbox, &checked);
    cout << "CHECKBOX_Checked : " << (checked ? "true" : "false") << "\n";
    return checked;
}
// void RButton::setText (const char *text)
// {
// 	IIntuition->SetAttrs (button,
// 		GA_Text,	text,
// 	TAG_DONE);
// 	this->text = text;
// }

// const char *RButton::getText()
// {
// 	return text;
// }

bool Checkbox::isCheckbox(Object *o)
{
	for(list<Object *>::iterator it = checkboxes.begin(); it != checkboxes.end(); it++) {
		if((*it) == o) return true;
	}
	return false;
}