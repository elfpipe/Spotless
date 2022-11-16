#include "../Config.hpp"
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
    return 0;
}