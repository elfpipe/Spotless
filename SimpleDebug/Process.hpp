#ifndef PROCESSHANDLER_HPP
#define PROCESSHANDLER_HPP

#include <proto/exec.h>
#include <proto/dos.h>
#include <stdint.h>
#include <string>

#include <list>
#include <vector>

#include "Pipe.hpp"

using namespace std;

struct KernelDebugMessage
{
  uint32 type;
  union
  {
    struct ExceptionContext *context;
    struct Library *library;
  } message;
};

class AmigaProcess {
public:
	typedef enum {
		MSGTYPE_EXCEPTION,
		MSGTYPE_TRAP,
		MSGTYPE_CRASH,
		MSGTYPE_OPENLIB,
		MSGTYPE_CLOSELIB,
		MSGTYPE_REMTASK,
		MSGTYPE_ADDTASK
	} DebugMessageType;

	struct DebugMessage {
		struct Message msg;
		DebugMessageType type;
		struct Library *library;
		struct Task *task;
	    struct ExceptionContext contextCopy;
	};

	struct TaskData {
		TaskData(struct Task *task, struct ExceptionContext *context) {
			this->task = task;
			this->contextCopy = *context;
		}
		struct Task *task;
	    struct ExceptionContext contextCopy;
	};

	struct HookData {
		struct Task *caller;
		int8_t signal;
		HookData(struct Task *caller, int8_t signal) {
			this->caller = caller;
			this->signal = signal;
		}
	};

private:
    static struct Process *process;

	static vector<struct TaskData *> tasks;
	// static APTR tasksMutex;

	static struct Hook hook;
	static ExceptionContext context;

	static bool exists;
	static bool running;
	static bool attached;

	static struct MsgPort *port;
	static uint8_t signal;

	Pipe pipe;

private:
	static ULONG amigaos_debug_callback (struct Hook *hook, struct Task *currentTask, struct KernelDebugMessage *dbgmsg);

public:
	AmigaProcess() { init(); }
	~AmigaProcess() { cleanup(); }

	static void handleMessages();

    void init();
    void cleanup();

	void clear();
	
    APTR load(string path, string command, string arguments);
	APTR attach(string name);
	void detach();

	static void hookOn(struct Task *task);
	static void hookOff(struct Task *task);

	void readContext ();
	void writeContext ();

	void skip();
	void backSkip();
	void step();

	bool isReturn(uint32_t address);
	void stepNoBranch();
	uint32_t branchAddress();

	uint32_t ip () { if(!exists) return 0; readContext(); return context.ip; }
	uint32_t sp () { if(!exists) return 0; readContext(); /*return context.gpr[1]; }*/ return (uint32_t)process->pr_Task.tc_SPReg; }
	uint32_t lr () { if(!exists) return 0; readContext(); return context.lr; }

	vector<TaskData *> getTasks();
	// void releaseTasks();

    void go();
	void wait();
	void wakeUp();

	void restartAll();
	static void restartTask(struct Task *);

	void suspend();
	static void suspendTask(struct Task *);
	void suspendAll();

	bool lives();
	bool isRunning();
	void resetSignals();
	Process *getProcess() { return process; }
	vector<string> emptyPipe() { return vector<string>(); } //pipe.emptyPipe(); }

	uint32_t getTrapSignal() {
		return 1 << signal;
	}
	uint32_t getPortSignal() {
		return 1 << port->mp_SigBit;
	}
	uint32_t getPipeSignal() {
		return 0x0; //fix
	}
	// vector<string> getMessages();
};
#endif