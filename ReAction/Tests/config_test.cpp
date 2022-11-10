#include "../Config.hpp"
#include <iostream>
int main() {
    if(1) {
        Config config("test.prefs");
        config.setValue("Window", "Width", 440);
    }
    if(1) {
        Config config("test.prefs");
        int width = config.getValue("Window", "Width", 320);
        cout << "Window.Width = " << width << "\n";
    }
    return 0;
}