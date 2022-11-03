#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/listbrowser.h>
#include <gadgets/listbrowser.h>

#include <reaction/reaction_macros.h>

#include <string.h>

#include "Screen.hpp"
#include "Widget.hpp"
#include "Listbrowser.hpp"
#include "../SimpleDebug/Strings.hpp"

using namespace std;

Listbrowser::Listbrowser(Widget *parent)
	:   columnInfo(0),
		noColumns(1),
		penType(PublicScreen::PENTYPE_DEFAULT)
{
	this->parent = parent;
    init();
}

Listbrowser::~Listbrowser()
{
}

void Listbrowser::init()
{
	IExec->NewList(&labels);
	
	object = ListBrowserObject,
		GA_RelVerify,				TRUE,
		GA_UserData,				parent,
		// GA_TabCycle,				TRUE,
		LISTBROWSER_Labels,			&labels,
		LISTBROWSER_AutoFit,		TRUE,
		LISTBROWSER_ShowSelected,	TRUE,
	ListBrowserEnd;

	parent->topLevelParent()->addChild(object); //To direct selection events, object needs to be stored in the top level widget
}

void Listbrowser::setColumnTitles(const char *titlesString) //separated by '|'
{
	astream str(titlesString);
	vector<string> titles = str.split('|'); //splitString (titlesString, "|");
	noColumns = titles.size();

	freeColumnTitles();
	
	columnInfo = IListBrowser->AllocLBColumnInfo ((UWORD)noColumns, TAG_DONE);
	
	for(unsigned int i = 0; i < noColumns; i++) {
		IListBrowser->SetLBColumnInfoAttrs (columnInfo,
			LBCIA_Column,				i,
			//LBCIA_CopyTitle,			true, //For some reason, this crashes
			LBCIA_Title,				strdup(titles[i].c_str()), //hiddeous hack
			LBCIA_DraggableSeparator,	true,
		TAG_DONE);
	}
	
    detach();
	IIntuition->SetAttrs (object,
		LISTBROWSER_ColumnInfo,		columnInfo,
		LISTBROWSER_ColumnTitles,	true,
	TAG_DONE);
    attach();
}

void Listbrowser::freeColumnTitles()
{
	if (columnInfo)
		IListBrowser->FreeLBColumnInfo (columnInfo);
}

void Listbrowser::setHierachical(bool enable)
{
	IIntuition->SetAttrs((Object *)object,
		LISTBROWSER_Hierarchical,	enable,
	TAG_DONE);
}

void Listbrowser::setStriping (bool enable)
{
	IIntuition->SetAttrs(object,
		LISTBROWSER_Striping, enable ? LBS_ROWS : LBS_NONE,
	TAG_DONE);
}

void Listbrowser::attach ()
{
	IIntuition->RefreshSetGadgetAttrs((struct Gadget *)object, parent->topLevelParent()->windowPointer(), 0,
		LISTBROWSER_Labels,		&labels,
	TAG_DONE);
}

void Listbrowser::detach ()
{
	IIntuition->SetAttrs(object,
		LISTBROWSER_Labels,		~0,
	TAG_DONE);
}

void Listbrowser::clear()
{
	detach();	
	IListBrowser->FreeListBrowserList(&labels);
	attach();
}

void Listbrowser::scrollToBottom ()
{
	IIntuition->SetAttrs(object,
		LISTBROWSER_Position, LBP_BOTTOM,
	TAG_DONE);	
}

void Listbrowser::focus (int line)
{
	IIntuition->RefreshSetGadgetAttrs ((struct Gadget *)object, parent->topLevelParent()->windowPointer(), 0,
		LISTBROWSER_Selected,		line-1,
		LISTBROWSER_MakeVisible,	line-1,
	TAG_DONE);
}

void *Listbrowser::getUserData (int lineNumber)
{
	struct List *labels;
	IIntuition->GetAttrs (object, LISTBROWSER_Labels, &labels, TAG_DONE);

	struct Node *node = IExec->GetHead (labels);
	int line = 1;
	while (line < lineNumber) {
		node = IExec->GetSucc (node);
		line++;
	}
	
	//now we have the correct node
	void *data = 0;
	if (node)
		IListBrowser->GetListBrowserNodeAttrs (node, LBNA_UserData, &data, TAG_DONE);
	return data;
}

void *Listbrowser::getSelectedNodeData ()
{
	struct Node *node = 0;
	IIntuition->GetAttrs (object, LISTBROWSER_SelectedNode, &node, TAG_DONE);
	
	void *data = 0;
	if (node)
		IListBrowser->GetListBrowserNodeAttrs(node, LBNA_UserData, &data, TAG_DONE);
	return data;
}

void Listbrowser::setPen (PublicScreen::PenType pen)
{
	penType = pen;
}

void Listbrowser::addNode (string text, void *userData, bool hasChildren, int generation)
{
	vector<string> columnText;
	columnText.push_back(text);
	addNode (columnText, userData, hasChildren, generation);
}

void Listbrowser::addNode (vector<string> columnTexts, void *userData, bool hasChildren, int generation)
{
	addCheckboxNode (columnTexts, false, false, userData, hasChildren, generation);
}

string Listbrowser::getNode(int line) {
	int i = 1;
	struct Node *node = IExec->GetHead(&labels);
	while(i < line) {
		node = IExec->GetSucc(node);
		i++;
	}
	const char *str;
	IListBrowser->GetListBrowserNodeAttrs(node, LBNCA_Text, &str, TAG_DONE);
	return string(str ? str : "");
}

void Listbrowser::addCheckboxNode (vector<string> columnTexts, bool checkbox, bool checked, void *userData, bool hasChildren, int generation)
{	
	uint32 flags = 0x0;
	flags |= hasChildren ? LBFLG_HASCHILDREN | LBFLG_SHOWCHILDREN : 0;
	flags |= penType != PublicScreen::PENTYPE_DEFAULT ? LBFLG_CUSTOMPENS : 0;

	struct Node *node = IListBrowser->AllocListBrowserNode(noColumns, //because of a bug, we need to take this from ColumnInfo
		LBNA_Flags,			flags,
		LBNA_Generation,	generation,
		LBNA_UserData,		userData,
		LBNA_CheckBox,		checkbox,
		LBNA_Checked,		checked,
	TAG_DONE);

	for (unsigned int i = 0; i < noColumns; i++) {
		IListBrowser->SetListBrowserNodeAttrs(node,
			LBNA_Column,		i,
				LBNCA_CopyText,		true,
				LBNCA_Text,			i < columnTexts.size() ? strdup(columnTexts[i].c_str()) : "", //because of the before mentioned bug
				LBNCA_FGPen,		PublicScreen::instance()->getPen (penType),
		TAG_END);
	}

	// detach();
	IExec->AddTail (&labels, node);
	// attach();
}

int Listbrowser::getSelectedLineNumber()
{
	int lineNumber = 0;
	IIntuition->GetAttrs (object, LISTBROWSER_Selected, &lineNumber, TAG_DONE);
	return ++lineNumber;
}

bool Listbrowser::checkboxChecked()
{
	uint32 event;
	IIntuition->GetAttrs (object, LISTBROWSER_RelEvent, &event, TAG_DONE);
	switch (event) {
		case LBRE_CHECKED:
			return true;
		case LBRE_UNCHECKED:
			return false;
	}
	return false;
}

void Listbrowser::showSelected(string text)
{
	int line = 0;
	struct Node *node = IExec->GetHead(&labels);
	while(node) {
		line++;
		const char *str;
		IListBrowser->GetListBrowserNodeAttrs(node, LBNCA_Text, &str, TAG_DONE);
		string current(str);
		if(!current.compare(text)) {
			focus(line);
			break;
		}
		node = IExec->GetSucc(node);
	}
}
unsigned int Listbrowser::getId()
{
	unsigned int id;
	IIntuition->GetAttrs(object, GA_ID, &id, TAG_DONE);
	return id;
}

bool Listbrowser::isListbrowser(Object *o)
{
	uint32 dummy;
	return IIntuition->GetAttr(LISTBROWSER_Labels, o, &dummy) ? true : false;
}