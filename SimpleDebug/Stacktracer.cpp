#include "Stacktracer.hpp"
#include "Strings.hpp"
#include "LowLevel.hpp"
#include "Debugger.hpp"
vector<string> Stacktracer::trace;

extern struct DebugIFace *IDebug;

int32 Stacktracer::stacktrace_callback(struct Hook *hook, struct Task *task, struct StackFrameMsg *frame) {
	string entry;
	switch (frame->State) {
		case STACK_FRAME_DECODED: {
			Debugger *debugger = (Debugger *)hook->h_Data;

			Elf32_Handle handle = (Elf32_Handle)debugger->getElfHandle()->getHandle();

			struct Elf32_SymbolQuery query;
			TEXT tempbuffer[256];

			query.Flags      = ELF32_SQ_BYVALUE | ELF32_SQ_LOAD;
			query.Name       = tempbuffer;
			query.NameLength = sizeof(tempbuffer) - 1;
			query.Value      = (uint32)frame->MemoryAddress;

			uint32 queryres = IElf->SymbolQuery(handle, 1, &query);

			if (queryres == 1 && query.Found == TRUE && query.Name[0] != '\0')
			{
					string sourceName = debugger->getSourceFile((uint32)frame->MemoryAddress);
					int sourceLine = debugger->getSourceLine((uint32)frame->MemoryAddress);
					if(sourceLine)
						entry = printStringFormat("[%s: line %d] %s", sourceName.c_str(), sourceLine, query.Name);
					else
						entry = printStringFormat("%s", query.Name);
			}
			else
			{
				entry = printStringFormat("<>");
			}
		}
		break;
		// 	struct DebugSymbol *symbol = is_readable_address_st((uint32_t)frame->MemoryAddress) ? IDebug->ObtainDebugSymbol(frame->MemoryAddress, NULL) : 0;
		// 	// struct DebugSymbol *symbol = IDebug->ObtainDebugSymbol(frame->MemoryAddress, NULL);

		// 	if(symbol) {
		// 		if(symbol->Type == DEBUG_SYMBOL_MODULE_STABS && symbol->SourceFileName)
		// 			entry = printStringFormat("[%s: line %d]: %s", symbol->SourceFileName, symbol->SourceLineNumber, symbol->SourceFunctionName);
		// 		else if(symbol->SourceFunctionName)
		// 			entry = printStringFormat("%s", symbol->SourceFunctionName);
		// 		else
		// 			entry = printStringFormat("[%s]", symbol->Name);
			
		// 		IDebug->ReleaseDebugSymbol(symbol);
		// 	}
		// }
		// break;

		case STACK_FRAME_INVALID_BACKCHAIN_PTR:
			entry = printStringFormat("(%p) invalid backchain pointer", frame->StackPointer);
			break;

		case STACK_FRAME_TRASHED_MEMORY_LOOP:
			entry = printStringFormat("(%p) trashed memory loop", frame->StackPointer);
			break;

		case STACK_FRAME_BACKCHAIN_PTR_LOOP:
			entry = printStringFormat("(%p) backchain pointer loop", frame->StackPointer);
			break;

		default:
			entry = printStringFormat("Unknown state=%lu", frame->State);
			break;
	}
	trace.push_back(entry);
	return 0;  // Continue symbol update.
}

vector<string> Stacktracer::stacktrace(Task *task, Debugger *debugger, uint32_t sp) {
	cout << "Stacktracer::stacktrace()\n";

	trace.clear();
	if(!task) return trace;
	
	Hook *hook = (Hook *)IExec->AllocSysObjectTags(
		ASOT_HOOK,
		ASOHOOK_Entry, stacktrace_callback,
		TAG_END);
	hook->h_Data = (APTR)debugger;

	// trace.clear();

	if (hook) {
		IDebug->StackTrace(task, hook);
		IExec->FreeSysObject(ASOT_HOOK, hook);
	}
	return trace;
}