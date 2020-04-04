#ifndef REQUESTERS_HPP
#define REQUESTERS_HPP

using namespace std;

class Requesters {
public:
	enum RequesterCategory {
		REQUESTER_EXECUTABLE,
		REQUESTER_TEXTFILE,
		REQUESTER_MODULE,
		REQUESTER_LOG,
		
		REQPATH_N
	};
	
private:
	static string pathBuffer[REQPATH_N];
	
public:
	Requesters() {}
	~Requesters() {}

	static string file (enum RequesterCategory category, string inputFile, string &pathResult, const char *format, ...);
	static string path (enum RequesterCategory category, const char *format, ...);
	static int choice (const char *title, const char *gadgetsText, const char *format, ...);
	static string requestString (const char *title, const char *format, ...);
	static void showAboutWindow();
};
#endif