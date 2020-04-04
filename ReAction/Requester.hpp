#ifndef DB101_REQUESTERTOOLS_h
#define DB101_REQUESTERTOOLS_h

#include <string>

class RequesterTools {
public:
	enum RequesterCategory {
		REQUESTER_EXECUTABLE,
		REQUESTER_TEXTFILE,
		REQUESTER_MODULE,
		REQUESTER_LOG,
		
		REQPATH_N
	};
	
private:
	std::string pathBuffer[REQPATH_N];
	
public:
	RequesterTools() {}
	~RequesterTools() {}

	std::string requesterFile (enum RequesterCategory category, std::string inputFile, std::string &pathResult, const char *format, ...);
	std::string requesterPath (enum RequesterCategory category, const char *format, ...);
	int requesterChoice (const char *title, const char *gadgetsText, const char *format, ...);
	std::string requesterString (const char *title, const char *format, ...);
	void showAboutWindow();
};
#endif
