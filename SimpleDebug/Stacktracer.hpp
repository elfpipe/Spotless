#ifndef STACKTRACER_HPP
#define STACKTRACER_HPP

#include <proto/exec.h>
#include <vector>
#include <string>
#include <stdint.h>

#include "Handle.hpp"
class Debugger;

using namespace std;

class Stacktracer {
private:
    static vector<string> trace;
private:
    static int32 stacktrace_callback(struct Hook *hook, struct Task *task, struct StackFrameMsg *frame);
public:
    vector<string> stacktrace(Task *task, Debugger *debugger, uint32_t sp);
};
#endif