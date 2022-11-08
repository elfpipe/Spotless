#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/elf.h>

#include "Process.hpp"
#include "Tracer.hpp"
#include "Roots.hpp"

#include <string>
#include <vector>
#include <string.h>
#include <iostream>

using namespace std;
ExceptionContext AmigaProcess::contextCopy;
// ExceptionContext *AmigaProcess::context;
struct MsgPort *AmigaProcess::port = 0;
bool AmigaProcess::tracing = false;
uint8_t AmigaProcess::signal = 0x0;

struct DebugIFace *IDebug = 0;
struct MMUIFace *IMMU = 0;

bool AmigaProcess::exists = false;
bool AmigaProcess::running = false;
bool AmigaProcess::attached = false;

struct Process *AmigaProcess::process = 0;
// vector<AmigaProcess::TaskData *> AmigaProcess::tasks;
// APTR AmigaProcess::tasksMutex = 0;;

struct Hook AmigaProcess::hook;

void AmigaProcess::init()
{
	IDebug = (struct DebugIFace *)IExec->GetInterface ((struct Library *)SysBase, "debug", 1, 0);
	if (!IDebug) {
		return;
	}

	IMMU = (struct MMUIFace *)IExec->GetInterface ((struct Library *)SysBase, "mmu", 1, 0);
	if (!IMMU) {
		return;
	}
	if(!port)
		port = (struct MsgPort *)IExec->AllocSysObject(ASOT_PORT, TAG_DONE);
	signal = IExec->AllocSignal(-1);
	seglist = 0;

	exists = false;
	running = false;
	attached = false;
}

void AmigaProcess::cleanup ()
{
	if (IDebug)
		IExec->DropInterface((struct Interface *)IDebug);
	IDebug = 0;

	if (IMMU)
		IExec->DropInterface((struct Interface *)IMMU);
	IMMU = 0;

	if(port) IExec->FreeSysObject(ASOT_PORT, port);
	IExec->FreeSignal(signal);

	clear();
}

void AmigaProcess::clear()
{
	process = 0;
	exists = false;
	running = false;
	attached = false;

	// hookOff((struct Task *)process);
	// for (auto t : tasks) { delete t; } tasks.clear();

	// resetTrapSignal();
	// if(seglist) IDOS->UnLoadSeg(seglist);
	// seglist = 0;

}

APTR AmigaProcess::load(string path, string file, string arguments)
{
	BPTR lock = IDOS->Lock(path.c_str(), SHARED_LOCK);
	if (!lock) {
		return 0;
	}
	BPTR homelock = IDOS->DupLock (lock);

	string command = Roots::append(path, file);
	seglist = IDOS->LoadSeg (command.c_str());
	
	if (!seglist) {
		IDOS->UnLock(lock);
		return 0;
	}

	IExec->Disable(); //can we avoid this

    process = IDOS->CreateNewProcTags(
		NP_Seglist,					seglist,
//		NP_Entry,					foo,
		NP_FreeSeglist,				false, //important
		NP_Name,					strdup(command.c_str()),
		// NP_CurrentDir,				lock,
		// NP_ProgramDir,				homelock,
		// NP_StackSize,				2000000,
		NP_Cli,						true,
		NP_Child,					false, //important
		NP_Arguments,				strdup(arguments.c_str()),
		NP_Input,					IDOS->Input(),
		NP_CloseInput,				false,
		NP_Output,					IDOS->Output(), //pipe.getWrite(),
		NP_CloseOutput,				false,
		NP_Error,					IDOS->ErrorOutput(),
		NP_CloseError,				false,
		NP_NotifyOnDeathSigTask,	IExec->FindTask(0),
		TAG_DONE
	);

	if (!process) {
		IExec->Enable();
		return 0;
	} else {
		IExec->SuspendTask ((struct Task *)process, 0L);		
		exists = true;
		running = false;
		attached = false;

		hookOn((struct Task *)process);
		readContext();
		IExec->Enable();
	}

	APTR handle;
	
	IDOS->GetSegListInfoTags (seglist, 
		GSLI_ElfHandle, &handle,
		TAG_DONE
	);
	
    return handle;
}

// void copy_exception_context(ExceptionContext *contextCopy, ExceptionContext *context)
// {
// 		contextCopy->Flags = context->Flags;
// 		contextCopy->msr = context->msr;
// 		contextCopy->ip = context->ip;
// 		for(int i = 0; i < 32; i++) {
// 			contextCopy->gpr[i] = context->gpr[i];
// 		}
// 		contextCopy->cr = context->cr;
// 		contextCopy->xer = context->xer;
// 		contextCopy->ctr = context->ctr;
// 		contextCopy->lr = context->lr;
// 		contextCopy->dsisr = context->dsisr;
// 		contextCopy->dar = context->dar;
// 		for(int i = 0; i < 32; i++) {
// 			contextCopy->fpr[i] = context->fpr[i];
// 		}
// 		contextCopy->fpscr = context->fpscr;
// 		/* The following are only used on AltiVec */
// 		// uint8   vscr[16]; /* AltiVec vector status and control register */
// 		// uint8   vr[512];  /* AltiVec vector register storage */
// 		// uint32  vrsave;   /* AltiVec VRSAVE register */

// }

ULONG AmigaProcess::amigaos_debug_callback (struct Hook *hook, struct Task *currentTask, struct KernelDebugMessage *dbgmsg)
{
    struct ExecIFace *IExec = (struct ExecIFace *)((struct ExecBase *)SysBase)->MainInterface;
	uint32 traptype = 0;

	HookData *data = (HookData *)hook->h_Data;
	// bool sendSignal = false;

	ULONG ret = 0;

	/* these are the 4 types of debug msgs: */
	switch (dbgmsg->type)
	{
		case DBHMT_ADDTASK: {
			//IDOS->Printf("ADDTASK\n");

			struct DebugMessage *message = (struct DebugMessage *)IExec->AllocSysObjectTags (ASOT_MESSAGE,
				ASOMSG_Size, sizeof(struct DebugMessage),
				TAG_DONE
			);

			message->type = MSGTYPE_ADDTASK;
			message->task = currentTask;
			// message->context = dbgmsg->message.context;
			// copy_exception_context(&message->contextCopy, dbgmsg->message.context);

			// if((struct Task *)process != currentTask) {
			// 	struct TaskData *data = new TaskData(message->task, &message->contextCopy);
			// 	tasks.push_back(data);
			// }

			IExec->PutMsg (port, (struct Message *)message);

			// hookOn(currentTask);

			ret = 1; //suspend, so we can do all of the above in good order in the main app
		}
		break;

		case DBHMT_REMTASK: {
			//IDOS->Printf("REMTASK\n");

			struct DebugMessage *message = (struct DebugMessage *)IExec->AllocSysObjectTags (ASOT_MESSAGE,
				ASOMSG_Size, sizeof(struct DebugMessage),
				TAG_DONE
			);
			message->type = MSGTYPE_REMTASK;
			message->task = currentTask;
			// message->context = dbgmsg->message.context;
			// copy_exception_context(&message->contextCopy, dbgmsg->message.context);

			// IDOS->Printf("REMTASK\n");
			// IDOS->Printf("[HOOK] ip = 0x%x\n", context.ip);
			// IDOS->Printf("[HOOK} trap = 0x%x\n", context.Traptype);

				// if(currentTask == (struct Task *)process) { // if this is main process
				// 	process = 0;
				// 	// cout << "exists == false\n";
				// 	exists = false;
				// 	running = false;
				// 	attached = false;
				// 	// clear();
				// }

			// sendSignal = true;  //if process has ended, we must signal caller
			IExec->PutMsg (port, (struct Message *)message);

			ret = 0; //1?
			break;
		}
		case DBHMT_EXCEPTION: {
			traptype = dbgmsg->message.context->Traptype;

			if (tracing) { // && traptype == 0x700 || traptype == 0xd00) {
				IExec->Signal(data->caller, 1 << data->signal);
				return 1;
			}
			
			// IDOS->Printf("EXCEPTION\n");
			// IDOS->Printf("[HOOK] ip = 0x%x\n", context.ip);
			// IDOS->Printf("[HOOK} trap = 0x%x\n", context.Traptype);
			
			struct DebugMessage *message = (struct DebugMessage *)IExec->AllocSysObjectTags (ASOT_MESSAGE,
				ASOMSG_Size, sizeof (struct DebugMessage),
				TAG_DONE
			);
			
			if (traptype == 0x700 || traptype == 0xd00) {
				message->type = MSGTYPE_TRAP;
				ret = 1;
			} else {
				message->type = MSGTYPE_EXCEPTION;
				ret = 0;
				// sendSignal = true;
			}
			message->task = currentTask;
			// message->context = dbgmsg->message.context;
			// copy_exception_context(&message->contextCopy, dbgmsg->message.context);

			IExec->PutMsg (port, (struct Message *)message);

			// running = false;

			// returning 1 will suspend the task
			// ret = 0;
			break;
		}
		case DBHMT_OPENLIB: {
			//IDOS->Printf("OPENLIB\n");

			// struct DebugMessage *message = (struct DebugMessage *)IExec->AllocSysObjectTags (ASOT_MESSAGE,
			// 	ASOMSG_Size, sizeof(struct DebugMessage),
			// 	TAG_DONE
			// );
			// message->type = MSGTYPE_OPENLIB;
			// message->library = dbgmsg->message.library;
			// message->task = currentTask;
			// message->contextCopy = *dbgmsg->message.context;
				
			// IExec->PutMsg(port, (struct Message *)message);			

			ret = 0;
		}
		break;

		case DBHMT_CLOSELIB: {
			// IDOS->Printf("CLOSELIB\n");

			// struct DebugMessage *message = (struct DebugMessage *)IExec->AllocSysObjectTags(ASOT_MESSAGE,
			// 	ASOMSG_Size, sizeof(struct DebugMessage),
			// 	TAG_DONE
			// );
			// message->type = MSGTYPE_CLOSELIB;
			// message->library = dbgmsg->message.library;
			// message->task = currentTask;
			// message->contextCopy = *dbgmsg->message.context;
				
			// IExec->PutMsg(port, (struct Message *)message);
			ret = 0;
		}
		break;

		default:
			break;
	}
	return ret;
}

void AmigaProcess::hookOn(struct Task *task)
{
	struct HookData *data = new HookData(IExec->FindTask(0), signal);

    hook.h_Entry = (ULONG (*)())amigaos_debug_callback;
    hook.h_Data =  (APTR)data;

	IDebug->AddDebugHook(task, &hook);
}

void AmigaProcess::hookOff(struct Task *task)
{
	IDebug->AddDebugHook(task, 0);
}

bool AmigaProcess::handleMessages() {
	bool result = false;
	DebugMessage *message = (DebugMessage *)IExec->GetMsg(port);
	while(message) {
		switch(message->type) {
			case AmigaProcess::MSGTYPE_EXCEPTION:
				cout << "EXCEPTION\n";

			case AmigaProcess::MSGTYPE_TRAP:
			case AmigaProcess::MSGTYPE_CRASH:
				// if(message->task == (struct Task *)process) {
				// 	context = message->context;
				// }
				running = false;
				result = true;
				break;

			case AmigaProcess::MSGTYPE_OPENLIB:
				// cout << "==OPENLIB\n";
				break;

			case AmigaProcess::MSGTYPE_CLOSELIB:
				// cout << "==CLOSELIB\n";
				break;

			case AmigaProcess::MSGTYPE_ADDTASK:
				// cout << "MSGTYPE_ADDTASK.\n";
				if((struct Task *)process != message->task) {
					struct TaskData *data = new TaskData(message->task /*, message->context*/);
					tasks.push_back(data);
				}
				if(running) restartTask(message->task);
				// hookOn();
				break;
			case AmigaProcess::MSGTYPE_REMTASK:
				cout << "REMTASK\n";
				if(message->task == (struct Task *)process) { // if this is main process
					process = 0;
					cout << "exists == false\n";
					exists = false;
					running = false;
					attached = false;
					// clear();
				} else {
					for(vector<TaskData *>::iterator it = tasks.begin(); it != tasks.end(); ) {
						if((*it)->task == message->task) { delete *it; it = tasks.erase(it); } else it++;
					}
				}
				// restart task to let it exit
				// restartTask(message->task);

				// exit = true;
				break;
		}
		IExec->FreeSysObject(ASOT_MESSAGE, (APTR)message);
		message = (struct AmigaProcess::DebugMessage *)IExec->GetMsg(port);
	}
	return result;
}

APTR AmigaProcess::attach(string name)
{
	struct Process *_process = (struct Process *)IExec->FindTask(name.c_str());
	if(!_process) return 0;

	process = _process;
	if (process->pr_Task.tc_Node.ln_Type != NT_PROCESS) {
		return 0;
	}

	BPTR seglist = IDOS->GetProcSegList (process, GPSLF_SEG|GPSLF_RUN);
  
	if (!seglist) {
		return 0;
	}

	if (process->pr_Task.tc_State == TS_READY || process->pr_Task.tc_State == TS_WAIT) {
		IExec->SuspendTask ((struct Task *)process, 0);
//		IExec->Signal ((struct Task *)_me, _eventSignalMask);
	}

	if (process->pr_Task.tc_State == TS_CRASHED) {
		process->pr_Task.tc_State = TS_SUSPENDED;
	}

	// if(tasksMutex) IExec->MutexObtain(tasksMutex);
	exists = true;
	running = false;
	attached = true;

	hookOn ((struct Task *)process);

	// if(tasksMutex) IExec->MutexRelease(tasksMutex);

//	readTaskContext ();

	APTR handle;
	IDOS->GetSegListInfoTags (seglist, 
		GSLI_ElfHandle, &handle,
		TAG_DONE
	);
		
	return handle;
}

void AmigaProcess::detach()
{
	hookOff((struct Task*)process);
	// for(list<Task *>::iterator it = tasks.begin(); it != tasks.end(); it++)
	// 	hookOff(*it);
	// tasks.clear();

	exists = false;
	running = false;
	attached = false;
}

void AmigaProcess::readContext ()
{
	if(exists)
		IDebug->ReadTaskContext  ((struct Task *)process, &contextCopy, RTCF_SPECIAL|RTCF_GENERAL|RTCF_STATE|RTCF_VECTOR|RTCF_FPU);
}

void AmigaProcess::writeContext ()
{
	if(exists)
		IDebug->WriteTaskContext  ((struct Task *)process, &contextCopy, RTCF_SPECIAL|RTCF_GENERAL|RTCF_STATE|RTCF_VECTOR|RTCF_FPU);
}

void AmigaProcess::TaskData::readContext ()
{
	// if(!exists || running) return;
	if(task) IDebug->ReadTaskContext  (task, &contextCopy, RTCF_SPECIAL|RTCF_STATE|RTCF_GENERAL|RTCF_VECTOR|RTCF_FPU);
}

void AmigaProcess::TaskData::writeContext ()
{
	// if(!exists || running) return;
	if(task) IDebug->WriteTaskContext (task, &contextCopy, RTCF_SPECIAL|RTCF_STATE|RTCF_GENERAL|RTCF_VECTOR|RTCF_FPU);
}

// void AmigaProcess::handleDeath()
// {
// 	exists = false;
// }

// ------------------------------------------------------------------ //

void AmigaProcess::skip() {
	readContext();
	contextCopy.ip += 4;
	writeContext(); //IDebug->WriteTaskContext((struct Task *)process, &contextCopy, RTCF_STATE);
}

void AmigaProcess::backSkip() {
	readContext();
	contextCopy.ip -= 4;
	writeContext(); //IDebug->WriteTaskContext((struct Task *)process, &contextCopy, RTCF_STATE);
}

bool AmigaProcess::step() {
	readContext();
	Tracer tracer(process, &contextCopy);
	if(tracer.activate()) {
		tracing = true;
		resetTrapSignal();
		go();
		waitTrace();
		tracer.suspend();
		if(!lives()) return false;
		return true;
	}
	return false;
}

bool AmigaProcess::stepNoBranch() {
	readContext();
	Tracer tracer(process, &contextCopy);
	if(tracer.activate(false)) {
		tracing = true;
		resetTrapSignal();
		go();
		waitTrace();
		tracer.suspend();
		if(!lives()) return false;
		return true;
	}
	return false;
}

uint32_t AmigaProcess::branchAddress() {
	Tracer tracer(process, &contextCopy);
	// uint32_t result = tracer.branch();
	// cout << "branch: " << (void *)result << "\n";
	return tracer.branch();
}

bool AmigaProcess::isReturn(uint32_t address)
{
	return Tracer::isBranchToLink(address);
}
// --------------------------------------------------------------------------- //


void AmigaProcess::waitTrace()
{
	uint32 received = IExec->Wait(1 << signal);
	tracing = false;
	running = false;
	if(received & SIGF_CHILD) exists = false;
}

void AmigaProcess::wakeUp()
{
	// IExec->Signal((struct Task *)IExec->FindTask(0), 1 << signal);
}

void AmigaProcess::go()
{
	cout << "GO\n";
    IExec->RestartTask((struct Task *)process, 0);
	running = true;
}

void AmigaProcess::restartTask(struct Task *task)
{
	IExec->RestartTask((struct Task *)task, 0);
}

void AmigaProcess::restartAll()
{
	IExec->Disable();
	for(vector<TaskData *>::iterator it = tasks.begin(); it != tasks.end(); it++) 
		IExec->RestartTask((*it)->task, 0);
	go();
	IExec->Enable();
}
void AmigaProcess::suspendAll()
{
	IExec->Disable();
	for(vector<TaskData *>::iterator it = tasks.begin(); it != tasks.end(); it++) 
		IExec->SuspendTask((*it)->task, 0);
	suspend();
	IExec->Enable();
}
void AmigaProcess::suspend()
{
	IExec->SuspendTask((struct Task *)process, 0);
	running = false;
}

void AmigaProcess::suspendTask(struct Task *task)
{
	// IExec->Disable();
	// if(tasksMutex) IExec->MutexObtain(tasksMutex);
	IExec->SuspendTask((struct Task *)task, 0);
	// if(tasks.size()) {
	// 	for(vector<TaskData *>::iterator it = tasks.begin(); it != tasks.end(); it++) {
	// 		IExec->SuspendTask((*it)->task, 0);
	// 	}
	// }
	// running = false;
	// if(tasksMutex) IExec->MutexRelease(tasksMutex);
	// IExec->Enable();
}

bool AmigaProcess::lives() {
	bool result = exists;
	return result;

	// uint32_t signals = IExec->SetSignal(0, 0);
	// return signals & SIGF_CHILD;
}
bool AmigaProcess::isRunning() {
	return exists && running;
}

void AmigaProcess::resetTrapSignal() {
	uint32_t signals = IExec->SetSignal(0, 0);
	if(signals & (1 << signal)) IExec->Wait(1 << signal);
}

vector<AmigaProcess::TaskData *> AmigaProcess::getTasks()
{
	// if(tasksMutex) IExec->MutexObtain(tasksMutex);
	return tasks;
}

// void AmigaProcess::releaseTasks()
// {
// 	// if(tasksMutex) IExec->MutexRelease(tasksMutex);
// }

// vector<string> AmigaProcess::getMessages() {
// 	vector<string> result;
// 	DebugMessage *message = (DebugMessage *)IExec->GetMsg(port);
// 	while(message) {
// 		switch(message->type) {
// 			case AmigaProcess::MSGTYPE_EXCEPTION:
// 				result.push_back("EXCEPTION (ip = 0x" + patch::toString((void *)message->contextCopy.ip) + ")");
// 				break;

// 			case AmigaProcess::MSGTYPE_TRAP:
// 				result.push_back("TRAP (ip = 0x" + patch::toString((void *)message->contextCopy.ip) + ")");
// 				break;

// 			case AmigaProcess::MSGTYPE_CRASH:
// 				result.push_back("CRASH (ip = 0x" + patch::toString((void *)message->contextCopy.ip) + ")");
// 				break;

// 			case AmigaProcess::MSGTYPE_OPENLIB:
// 				result.push_back(printStringFormat("OPENLIB : task (0x%x)", (void *)message->task));
// 				break;

// 			case AmigaProcess::MSGTYPE_CLOSELIB:
// 				result.push_back(printStringFormat("CLOSELIB : task (0x%x)", (void *)message->task));
// 				break;

// 			case AmigaProcess::MSGTYPE_REMTASK:
// 				result.push_back(printStringFormat("A task has been removed... (0x%x)", (void *)message->task));
// 				break;
// 			case AmigaProcess::MSGTYPE_ADDTASK:
// 				result.push_back(printStringFormat("A new sub-task has been started... (0x%x)", (void *)message->task));
// 				break;
// 		}
// 		message = (struct AmigaProcess::DebugMessage *)IExec->GetMsg(port);
// 	}
// 	return result;
// }