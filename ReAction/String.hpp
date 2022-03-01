#ifndef STRING_h
#define STRING_h

#include <proto/intuition.h>
#include <string>

using namespace std;

class Widget;
class Layout;
class RString {
private:
	Object *rstring;
	Object *systemObject () { return rstring; }

	Widget *parent;
	
private:
	char contents[1024];
	
public:
	RString(Widget *parent, const char *content);
	~RString();
	
	void setContent(const char *newContent);
	string getContent();

public:
	friend Widget;
	friend Layout;	
};
#endif
