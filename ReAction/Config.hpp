#include <proto/application.h>
#include <string>
#include <iostream>
 
using namespace std;
class Config {
private:
    struct Library *ApplicationBase = NULL;
    struct ApplicationIFace *IApplication = NULL;
    struct PrefsObjectsIFace *IPrefsObjects = NULL;
private:
    PrefsObject *dict;
    string file;
    
public:
    Config(string file);
    ~Config();

    int getValue(string object, string value, int def);
    void setValue(string object, string value, int number);

    bool getBool(string object, bool def);
    void setBool(string object, bool value);
};