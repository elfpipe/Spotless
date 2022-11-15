#ifndef SPOTLESS_CONFIG_HPP
#define SPOTLESS_CONFIG_HPP
#include <proto/application.h>
#include <string>
#include <iostream>
#include <vector>

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

    vector<string> getArray(string object);
    void setArray(string object, vector<string> array);
};
#endif