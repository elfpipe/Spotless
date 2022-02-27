#ifndef SPOTLESS_SOURCECODE_H
#define SPOTLESS_SOURCECODE_H

#include "../ReAction/classes.h"
#include "Spotless.hpp"

#include "../SimpleDebug/TextFile.hpp"
#include "../SimpleDebug/Strings.hpp"

class Code : public Widget {
private:
    Spotless *spotless;
    Listbrowser *listbrowser;
    string fileName;

public:
    Code(Spotless *parent) : Widget((Widget *)parent) { spotless = parent; }
    void createGuiObject(Layout *layout) {
        listbrowser = layout->createListbrowser();
        listbrowser->setColumnTitles("Break|Line|Text");
    }
    void show(string file, string foundPath) {
        if(!foundPath.size()) {
            clear();
            char buffer[4096];
            sprintf(buffer, "Failed to find source file \'%s\' in the project search path. Did you configure your project?", file.c_str());
            vector<string> lineData;
            lineData.push_back("");
            lineData.push_back("");
            lineData.push_back(buffer);
            listbrowser->addNode(lineData);
            highlight(0);
            return;
        }
        TextFile text(foundPath);
        listbrowser->clear();
        int line = 1;
        while(text.good()) {
            string textLine = text.getLine();
            vector<string> data;
            data.push_back("");
            data.push_back(patch::toString(line));
            data.push_back(formatRawString(textLine));
            listbrowser->addCheckboxNode(
                data,
                spotless->debugger.isSourceLine(file, line),
                spotless->debugger.isBreakpoint(file, line)
            );
            line++;
        }
    }
    void highlight(int line) {
        listbrowser->focus(line);
    }
    void update() {
        string file = spotless->debugger.getSourceFile();
        if(!file.compare("<built-in>")) {
            clear();
            vector<string> lineData;
            lineData.push_back("");
            lineData.push_back("0");
            lineData.push_back("<built-in>");
            listbrowser->addNode(lineData);
            fileName = "<built-in>";
            return;
        }
        if(fileName.compare(file)) {
            string filePath = spotless->debugger.searchSourcePath(file);
            show(file, filePath);
        }
        highlight(spotless->debugger.getSourceLine());
        fileName = file;
    }
    void checkboxSelected(string file, bool checked) {
        int line = listbrowser->getSelectedLineNumber();
        spotless->debugger.breakpoint(file, line, checked);
    }

    void clear() {
        listbrowser->clear();
    }
};
#endif