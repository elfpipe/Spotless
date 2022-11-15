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
        cout << "Error reading prefs file from disk " << file << ". A new file will be created.\n";
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
bool Config::getBool(string object, string item, bool def) {
    PrefsObject *o = IPrefsObjects->DictGetObjectForKey(dict, object.c_str());
    if(!o) return def;
    bool result = IPrefsObjects->DictGetBoolForKey(o, item.c_str(), def);
    // cout << "getBool() " << object << " " << item << " " << result << "\n";
    return result;
} 
void Config::setBool(string object, string item, bool value) {
    // cout << "setBool() " << object << " " << item << " " << value << "\n";
    uint32 error;
    PrefsObject *o = IPrefsObjects->DictGetObjectForKey(dict, object.c_str());
    if(!o) {
        IPrefsObjects->PrefsDictionary(0, &error, ALPO_Alloc, 0, TAG_DONE);
        IPrefsObjects->DictSetObjectForKey(dict, o, object.c_str());
    }
	IPrefsObjects->DictSetObjectForKey(o,
        IPrefsObjects->PrefsNumber(NULL, &error, ALPONUM_AllocSetBool, value, TAG_DONE),
        item.c_str());
}
vector<string> Config::getArray(string object) {
    vector<string> result;
    uint32 error;
    PrefsObject *o = IPrefsObjects->DictGetObjectForKey(dict, object.c_str());
    uint32 count;
    struct ALPOObjIndex oi;
    IPrefsObjects->PrefsArray(o, &error, ALPOARR_GetCount, &count, TAG_DONE);
    if(error) return result;
    for(oi.index = 0; oi.index < count; oi.index++) {
        IPrefsObjects->PrefsArray(o, NULL, ALPOARR_GetObjAtIndex, &oi, TAG_DONE);
        struct ALPOString s;
        if(oi.obj)
            IPrefsObjects->PrefsString(oi.obj, &error, ALPOSTR_GetString, &s, TAG_DONE);
        result.push_back(s.string);
    }
    return result;
}
void Config::setArray(string object, vector<string> array) {
 	uint32 error;
	PrefsObject *a = IPrefsObjects->PrefsArray(0, &error, ALPO_Alloc, 0, TAG_DONE);
    for(int i = 0; i < array.size(); i++) {
        IPrefsObjects->PrefsArray(a, &error, ALPOARR_AddObj,
            IPrefsObjects->PrefsString(0, 0, ALPOSTR_AllocSetString, array[i].c_str(), TAG_DONE),
            TAG_DONE);
    }
    IPrefsObjects->DictSetObjectForKey(dict, a, object.c_str());
}