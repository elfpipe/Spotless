#include <stdarg.h>
#include <string>
#include <sstream>

#include "Strings.hpp"

using namespace std;

string printStringFormat_helper (const char *format, va_list argptr)
{
	char result[MAX_CHAR_BUFFER_SIZE];
	vsprintf(result, format, argptr);
	string resultString(result);
	
	return resultString;
}

string printStringFormat (const char *format, ...)
{
	va_list argptr;
	va_start(argptr, format);
	string result = printStringFormat_helper (format, argptr);
	va_end(argptr);

	return result;
}

string formatRawString(string text) {
	string result;
	stringstream str(text);
	while(!str.eof()) {
		char c = str.get();
		switch(c) {
			case '\t':
				result += "    ";
				break;
			case '\n':
			case '\r':
			case '\0':
				break;
			default:
				result += c;
				break;
		}
	}
	return result;
}