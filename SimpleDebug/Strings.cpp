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
	while(str.good()) {
		char c = str.get();
		if(str.eof()) break;
		switch(c) {
			case '\t':
				result += "    ";
				break;
			case '\n':
			case '\r':
			case '\0':
				break;
			default:
				if(c > 31 && c < 127)
					result += c;
				break;
		}
	}
	return result;
}