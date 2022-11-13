#include <proto/exec.h>
#include "Config.hpp"
#include <iostream>
using namespace std;
Config::Config(string file) {
    ApplicationBase = IExec->OpenLibrary("application.library", 52);
    if(ApplicationBase)
        IPrefsObjects = (struct PrefsObjectsIFace *) IExec->GetInterface(ApplicationBase, "prefsobjects", 2, NULL);

    this->file = file;
	uint32 error;
	dict = IPrefsObjects->PrefsDictionary(NULL, &error, ALPO_Alloc, 0, TAG_DONE);
	if(error) {
        cout << "Error creating prefs object " << file << "\n";
        return;
    }
	error = IPrefsObjects->ReadPrefs(dict, READPREFS_FileName, file.c_str(), TAG_DONE);
	if(error) {
        cout << "Error reading prefs file from disk " << file << "\n";
    }
}

Config::~Config() {
	uint32 error = IPrefsObjects->WritePrefs(dict, WRITEPREFS_FileName, file.c_str(), TAG_DONE);
	if(error) {
        cout << "Error writing to prefs file " << file << "\n";
    }	
	IPrefsObjects->PrefsDictionary(dict, &error, ALPO_Release, 0, TAG_DONE);

    IExec->DropInterface((struct Interface *)IPrefsObjects);
    IExec->CloseLibrary(ApplicationBase);
}
int Config::getValue(string object, string value, int def) {
    PrefsObject *o = IPrefsObjects->DictGetObjectForKey(dict, object.c_str());
    return IPrefsObjects->DictGetIntegerForKey(o, value.c_str(), def);
}
void Config::setValue(string object, string value, int number) {
    cout << "setValue() " << object << " " << value << "\n";
    uint32 error;
    PrefsObject *o = IPrefsObjects->DictGetObjectForKey(dict, object.c_str());
    if(!o) {
        o = IPrefsObjects->PrefsDictionary(0, &error, ALPO_Alloc, 0, TAG_DONE);
        IPrefsObjects->DictSetObjectForKey(dict, o, object.c_str());
    }
	IPrefsObjects->DictSetObjectForKey(o,
        IPrefsObjects->PrefsNumber(NULL, &error, ALPONUM_AllocSetLong, number, TAG_DONE),
        value.c_str());
}
bool Config::getBool(string object, bool def) {
    return IPrefsObjects->DictGetBoolForKey(dict, object.c_str(), def);
} 
void Config::setBool(string object, bool value) {
    uint32 error;
	IPrefsObjects->DictSetObjectForKey(dict,
        IPrefsObjects->PrefsNumber(NULL, &error, ALPONUM_AllocSetBool, value, TAG_DONE),
        object.c_str());
}