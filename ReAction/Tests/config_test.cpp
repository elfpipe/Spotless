#include "../Config.hpp"
string convertToUnixRelative(string path)
{
	// cout << "convertToUnixRelative : path = " << path << "\n";

	string result = path;
	for (string::iterator it = result.begin(); it != result.end(); it++) {
		if((*it) != '/') break;
		it = result.insert(it, '.'); it++;
		it = result.insert(it, '.'); it++;
	}

	// cout << "convertToUnixRelative : result = " << result << "\n";

	return result;
}
#include <iostream>
int main() {
    if(1) {
        Config config("test.prefs");
        config.setValue("Window", "Width", 440);
        config.setBool("Window", "Selected", true);
    }
    if(1) {
        Config config("test.prefs");
        int width = config.getValue("Window", "Width", 320);
        cout << "Window.Width = " << width << "\n";
        bool selected = config.getBool("Window", "Selected", false);
        cout << "Window.Selected = " << selected << "\n";
    }
    string result = convertToUnixRelative("//test1/test2");
    cout << "convertToUnixRelative() " << result << "\n";
    return 0;
}