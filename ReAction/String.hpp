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
	
	static list<Object *> strings;
	unsigned int id;

public:
	RString(Widget *parent, const char *content);
	~RString();
	
	static bool isString(Object *o);

	void setContent(const char *newContent);
	string getContent();

	unsigned int getId() { return id; }

public:
	friend Widget;
	friend Layout;	
};
#endif
