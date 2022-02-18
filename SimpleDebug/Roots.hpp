#ifndef ROOTS_HPP
#define ROOTS_HPP

#include <string>
#include <list>
#include "TextFile.hpp"
#include "Strings.hpp"

using namespace std;

class Roots {
private :
    list<string> roots;
public:
    void add(string root) { roots.push_back(root); }
    void remove(string root) { for(list<string>::iterator it = roots.begin(); it != roots.end(); it++) {
         if(!(*it).compare(root)) { roots.erase(it); break; }
    }}
    list<string> &get() { return roots; }
    static string append(string path, string file) {
        if(path[path.size()-1] == ':') {
            return path + file;
        }
        if(path[path.size()-1 ]=='/') {
            return path + file;
        }
        if(path.size() > 0) {
            return path + string("/") + file;
        }
        return file;
    }
    static string getPath(string fileLocation) {
        for(int i = fileLocation.size()-1; i > 0; i--) {
            if(fileLocation[i] == ':')
                return fileLocation.substr(0, i+1);
            if(fileLocation[i] == '/')
                return fileLocation.substr(0, i);
        }
        return string();
    }
    static string getFilePart(string fileLocation) {
        for(int i = fileLocation.size()-1; i > 0; i--) {
            if(fileLocation[i] == ':' || fileLocation[i] == '/')
                return fileLocation.substr(i+1, fileLocation.size() - i+1);
        }
        return string();
    }
    string search(string file) {
        for(list<string>::iterator it = roots.begin(); it != roots.end(); it++) {
            string test(append(*it, file));
            TextFile tf(test);
            if(tf.good()) return test;
        }
        return string();
    }
    void clear() {
        roots.clear();
    }
};
#endif