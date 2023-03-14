#ifndef SPOTLESS_DEBUGGER_HPP
#define SPOTLESS_DEBUGGER_HPP

//#include "Symbols.hpp"
#include "Process.hpp"
#include "Breaks.hpp"

#include "Binary.hpp"
#include "Handle.hpp"

#include "Stacktracer.hpp"
#include "Strings.hpp"
#include "TextFile.hpp"
#include "Binary.hpp"
#include "Roots.hpp"

#include <iostream>
#include <string>
#include <vector>

#include <inttypes.h>

extern struct DebugIFace *IDebug;

using namespace std;

// ---------------------------------------------------------------------------- //

class Debugger {
private:
	AmigaProcess process;
    ElfSymbols symbols;
    Breaks breaks, linebreaks, outbreak;

	ElfHandle *handle;
	Binary *binary;

	Roots roots;

	int line, hexLine;

	bool firstRun;
	// string entryPoint;

public:
	Debugger() : handle(0), binary(0), line(0) {
		// entryPoint = "main";
	}
	~Debugger() {
		clear();
	}
	void open(APTR _handle, string name) {
		handle = new ElfHandle(_handle, name);
		handle->open();

		symbols.readAll(handle);

		if(handle->performRelocation())
			binary = new Binary(handle->getName(), (SymtabEntry *)handle->getStabSection(), handle->getStabstrSection(), handle->getStabsSize());
		else cout << "Relocations failed.\n";
		firstRun = true;

		handle->close();
	}
	bool load(string path, string file, string args) {
		APTR handle = process.load(path, file, args);
		if (handle) { open(handle, file); }

		//experimental entry code, to prevent breaks in kernel :
		// if(entryPoint.size() && symbols.hasSymbols()) {
		// 	uint32 value = symbols.valueOf(entryPoint.c_str());
		// 	if(value) {
		// 		Breaks temp;
		// 		temp.insert(value);
		// 		temp.activate();
		// 		process.go();
		// 		process.wait();
		// 	}
		// }

		// return success
		return handle != 0;
	}
	// void handleDeath() {
	// 	process.handleDeath();
	// }
	bool attach(string name) {
		APTR handle = process.attach(name);
		if(handle) open(handle, name);
		return handle != 0;
	}
	void detach() {
		process.detach();
	}
	ElfHandle *getElfHandle() { return handle; }
	bool handleMessages() {
		return process.handleMessages();
	}
	vector<string> sourceFiles() {
		return binary ? binary->getSourceNames() : vector<string>();
	}
	vector<string> elfSymbols() {
		return symbols.printable();
	}
	string binaryStructure() {
		return binary ? binary->toString() : string();
	}
	bool breakpoint(string file, int line, bool set) {
		if(!binary) return false;
		uint32_t address = binary->getLineAddress(file, line);
		if(address)	set ? (void) breaks.insert(address) : breaks.remove(address);
		return address != 0;
	}
	bool breakpoint(string function, bool set) {
		if(!binary) return false;
		uint32_t address = binary->getFunctionAddress(function);
		if(address)	set ? (void) breaks.insert(address) : breaks.remove(address);
		return address != 0;
	}
	bool breakpointSymbol(string symbolName, bool set) {
		uint32_t address = symbols.valueOf(symbolName);
		if(address)	set ? (void) breaks.insert(address) : breaks.remove(address);
		return address != 0;
	}
	bool breakpointAddress(uint32_t address, bool set) {
		if(address) set ? (void) breaks.insert(address) : breaks.remove(address);
		return address != 0;
	}
	bool isBreak(uint32_t address) {
		return breaks.isBreak(address);
	}
	void activateBreaks() { // only for blind running
		if(process.lives()) { //necessary on 440ep
			breaks.activate();
		}
	}
	void suspendBreaks() {
		if(process.lives()) { //necessary on 440ep
			outbreak.deactivate(); //order of these essential!
			linebreaks.deactivate();
			breaks.deactivate();
		}
		linebreaks.clear();
		outbreak.clear();
	}
	void start() {
		if(!process.lives() || process.isRunning()) return;

		if (is_writable_address(process.ip() + 4) && !firstRun && !process.isRunning() && binary->isBinary(process.ip())) {
			process.step();
		}
		firstRun = false;
		breaks.activate();
		process.restartAll();
		// process.wait();
		// breaks.deactivate(); //do the last bit in trap handler
	}
	void justGo() {
		firstRun = false;
		if(process.lives() || !process.isRunning())
			process.restartAll();
	}
	void stop() {
		if(process.isRunning()) process.suspendAll();
	}
	// void setTrace() {
	// 	process.setTrace();
	// }
	void waitTrace() {
		process.waitTrace();
	}
	void backSkip() {
		if(lives() && binary->getFunction(process.ip()-4))
			process.backSkip();
	}
	void step() {
		if(lives() && binary->getFunction(process.ip()+4))
			process.step();
	}
	void skip() {
		if(lives() && binary->getFunction(process.ip()+4))
			process.skip();
	}
	void unsafeBackSkip() {
		if(lives())
			process.backSkip();
	}
	void unsafeStep() {
		if(lives())
			process.step();
	}
	void unsafeSkip() {
		if(lives())
			process.skip();
	}
	void asmStepOver() {
		if(lives())
		process.stepNoBranch();
	}
	void asmStepInto() {
		unsafeStep();
	}
	void asmStepOut() {
		if(!lives()) return;

		// Breaks outBreak;
		outbreak.insert(process.lr());
		outbreak.activate();

		// process.setTrace();
		process.go();
		// process.waitTrace();
		// outBreak.deactivate();
	}
	void safeStep() {
		if(!lives() || !binary->getFunction(process.ip())) return;

		if(binary->getSourceFile(process.branchAddress()).size() > 0)
			process.step();
		else
			process.stepNoBranch();
	}
	void stepOver() {
		if(!binary || !process.lives() || process.isRunning()) return;

		if(!binary->getFunction(process.ip())) { //if not inside known code, just keep running
			start();
			return;
		}

		Function *function = binary->getFunction(process.ip());
		linebreaks.clear();
		if(function)
		for(int i = 0; i < function->lines.size(); i++) {
			linebreaks.insert(function->address + function->lines[i]->address);
		}
		process.step();

		breaks.activate();
		linebreaks.activate();

		process.go();
		// process.wait();

		// linebreaks.deactivate(); //do the last bit in trap handler
		// breaks.deactivate();
	}
	void stepInto() {
		if(!binary || !process.lives() || process.isRunning()) return;

		Function *f = binary->getFunction(process.ip());

		if(!f) {
			start();
			return;
		}
		// if (binary->isLastLine(process.ip())) {
		// 	start();
		// 	return;
		// }
		do {
			if(process.isReturn(process.ip())) { //if we are returning from the function, we need to let go and run normally
				start();
				return;
			}
			if(process.branchAddress() && binary->getFunction(process.branchAddress())) {
				if(!process.step()) break; 
			}
			else {
				if(!process.stepNoBranch()) break;
			}
		} while(!binary->isLocation(process.ip()));
		//process.wakeUp();
	}
	void stepOut() {
		if (!binary || !process.lives() || process.isRunning()) return;

		uint32_t nip = process.ip();
		Function *f = binary->getFunction(process.ip());

		while (f == binary->getFunction(nip)) {
			while(!process.isReturn(nip)) nip += 4;
			if(process.isReturn(nip)) outbreak.insert(nip);
			nip += 4;
		}

		if(1)
		{
			outbreak.activate();
			// tracing = true;
			process.setTrace();
			process.resetTrapSignal();
			process.go();
			waitTrace();
			outbreak.deactivate();
			outbreak.clear();
		}

		if(binary->isFunction(process.lr()) && outbreak.insert(process.lr())) {
			outbreak.activate();
		}
		// start();
		process.go();

		// breaks.activate();
		// outBreak.activate();

		// process.setTrace();
		// process.go();
		// process.waitTrace();

		// if(process.lives()) {
		// 	outBreak.deactivate();
		// 	breaks.deactivate();
		// }
	}
	vector<string> context() {
		return binary ? binary->getContext(process.getContext(), process.ip(), process.sp()) : vector<string>();
	}
	vector<string> globals() {
		return binary ? binary->getGlobals(symbols) : vector<string>();
	}
	// string getEntryPoint() {
	// 	return entryPoint;
	// }
	// void setEntryPoint(string newEntry) {
	// 	entryPoint = newEntry;
	// }
	uint32_t getIp() {
		return process.ip();
	}
	uint32_t getSp() {
		return process.sp();
	}
	string getSourceFile() {
		return binary ? binary->getSourceFile(process.ip()) : string();
	}
	int getSourceLine() {
		return binary ? binary->getSourceLine(process.ip()) : 0;
	}
	string getSourceFile(uint32 address) {
		return binary ? binary->getSourceFile(address) : string();
	}
	int getSourceLine(uint32 address) {
		return binary ? binary->getSourceLine(address) : 0;
	}
	int getSourceLine(uint32_t address) {
		return binary ? binary->getSourceLine(address) : 0;
	}
	bool isSourceLine(string file, int line) {
		return binary ? binary->getLineAddress(file, line) : false;
	}
	bool isBreakpoint(string file, int line) {
		return binary ? breaks.isBreak(binary->getLineAddress(file, line)) : false;
	}
	uint32_t getSymbolValue(string symbolName) {
		return symbols. valueOf(symbolName);
	}
	string getSymbolFromAddress(uint32_t address) {
		return symbols.nameFromValue(address);
	}
	string printLocation() {
		return binary ? binary->getSourceFile(process.ip()) + " at line " + patch::toString(binary->getSourceLine(process.ip())) : string();
	}
	bool lives() {
		return process.lives();
	}
	bool isRunning() {
		return process.isRunning();
	}
	vector<string> emptyPipe() {
		return process.emptyPipe();
	}
	vector<string> stacktrace() {
		Stacktracer stacktracer;
		return !process.lives() || process.isRunning() || process.isTracing() ? vector<string>() : stacktracer.stacktrace((Task *)process.getProcess(), this, getSp());
	}
	vector<string> functionSource() {
		vector<string> result = functionSource(process.ip());
		vector<AmigaProcess::TaskData *> tasks = process.getTasks();
		for(int i = 0; i < tasks.size(); i++) {
			result.push_back("");
			result.push_back(printStringFormat("Sub-Task %d : (0x%x)", i+1, (void *)tasks[i]->task));
			vector<string> taskResults = functionSource(tasks[i]->ip());
            result.insert(result.end(), taskResults.begin(), taskResults.end());
		}
		return result;
	}
	vector<string> functionSource(uint32_t address) {
		vector<string> result;
		string source = binary->getSourceFile(address);
		
		if(source.size() == 0) return result;
		string fullPath = roots.search(source);
		if(fullPath.size() == 0) return result;
		TextFile file(fullPath);
		Function *function = binary->getFunction(process.ip());
		if(!function) return result;
		int line = function->lines[0]->line;
		for(int i = 0; i < function->lines.size(); i++)
			while(line <= function->lines[i]->line) {
				string l = patch::toString(line);
				string s;
				for(int n = s.size(); n < 8; n++)
					s += " ";
				s += l;
				if (line == function->lines[i]->line) {
					if(line == binary->getSourceLine(process.ip()))
						s += " -->: ";
					else if(breaks.isBreak(function->address + function->lines[i]->address))
						s += " [*]: ";
					else
						s += " -  : ";
				}
				else
					s += "    : ";
				result.push_back(s + file.getLine(line++));
			}
		return result;
	}
	vector<string> disassemble() {
		vector<string> result = (binary && binary->getFunction(process.ip())) ? disassembleFunction(process.ip()) : disassembleAddress(process.ip());
		// This below is useful but unsafe :
		// vector<AmigaProcess::TaskData *> tasks = process.getTasks();
		// for(int i = 0; i < tasks.size(); i++) {
		// 	result.push_back("");
		// 	result.push_back(printStringFormat("Sub-Task %d : (0x%x)", i+1, (void *)tasks[i]->task));
		// 	vector<string> taskResults = binary && binary->getFunction(tasks[i]->ip()) ? disassembleFunction(tasks[i]->ip()) : disassembleAddress(tasks[i]->ip());
        //     result.insert(result.end(), taskResults.begin(), taskResults.end());
		// }
		return result;
	}
	vector<string> disassembleAddress(uint32_t start) {
		vector<string> result;
		if(!process.lives() || process.isRunning()) return result;
		if(!is_readable_address(start)) { result.push_back("<not a readable address>"); return result; }
		int entry = 1;
		for(uint32_t address = start; address < start+128; address += 4) {
			char opcode[256], operands[256];
			IDebug->DisassembleNative((APTR)address, opcode, operands);

			int32 offset;
			string symbolName;
			ppctype branchType = PPC_DisassembleBranchInstr(*(uint32_t *)address, &offset);
			if (branchType == PPC_BRANCH || branchType == PPC_BRANCHCOND) {
				string name = symbols.nameFromValue(address + offset);
				symbolName = " <" + name + ">";
			}

			entry++;
			if(isLocation(address)) {
				result.push_back(printStringFormat("[line %d] 0x%x : %s %s", getSourceLine(address), address, opcode, operands) + symbolName);
			} else {
				result.push_back(printStringFormat("          0x%x : %s %s", address, opcode, operands) + symbolName);
			}

			//for hightlighting
			if(address == getIp()) line = entry;
		}
		return result;
	}
	vector<string> disassembleFunction(uint32_t address) {
		vector<string> result;
		if(!process.lives() || process.isRunning()) return result;

		Function *function = binary ? binary->getFunction(address) : 0;
		if(!function) return result;
		int entry = 1;
		result.push_back(function->name + " :");
		Scope *scope = function->locals[0];

			for(uint32_t address = scope->begin; address <= scope->end; address += 4) {
				char opcode[256], operands[256];
				IDebug->DisassembleNative((APTR)address, opcode, operands);

				int32 offset;
				string symbolName;
				ppctype branchType = PPC_DisassembleBranchInstr(*(uint32_t *)address, &offset);
				if (branchType == PPC_BRANCH || branchType == PPC_BRANCHCOND) {
					string name = symbols.nameFromValue(address + offset);
					symbolName = " <" + name + ">";
				}

				entry++;
				if(isLocation(address)) {
					result.push_back(printStringFormat("[line %d] 0x%x : %s %s", getSourceLine(address), address, opcode, operands) + symbolName);
				} else {
					result.push_back(printStringFormat("          0x%x : %s %s", address, opcode, operands) + symbolName);
				}

				//for hightlighting
				if(address == getIp()) line = entry;
			}
        // for(int i = 0; i < function->lines.size(); i++) {
		// 	int maxOffset = function->lines[i]->address + 4;
		// 	if(i < function->lines.size() - 1)
		// 		maxOffset = function->lines[i+1]->address;

		// 	for(uint32_t offset = function->lines[i]->address; offset < maxOffset; offset += 4) {
		// 		uint32_t address = function->address + offset;
		// 		char opcode[256], operands[256];
		// 		IDebug->DisassembleNative((APTR)address, opcode, operands);

		// 		entry++;
		// 		if(isLocation(address)) {
		// 			result.push_back(printStringFormat("[line %d] 0x%x : %s %s", getSourceLine(address), address, opcode, operands));
		// 		} else {
		// 			result.push_back(printStringFormat("          0x%x : %s %s", address, opcode, operands));
		// 		}

		// 		//for hightlighting
		// 		if(address == getIp()) line = entry;
		// 	}
        // }
		return result;
	}
	vector<string> disassembleSymbol(string symbolName) {

		vector<string> result;
		if(!process.lives() || process.isRunning()) return result;

		uint32_t addressBegin = symbols.valueOf(symbolName);

		if(!addressBegin) {
			result.push_back("<No such address>");
			return result;
		}
		// This is taken care of by the system functions
		// if(!is_readable_address(addressBegin)) {
		// 	result.push_back("<not a readable address>");
		// 	return result;
		// }
		int entry = 1;
		line = 0;
		for(uint32_t offset = 0; offset < symbols.sizeOf(symbolName); offset += 4) {
				uint32_t address = addressBegin + offset;

				char opcode[256], operands[256];
				IDebug->DisassembleNative((APTR)address, opcode, operands);

				int32 branchOffset;
				string symbolName;
				ppctype branchType = PPC_DisassembleBranchInstr(*(uint32_t *)address, &branchOffset);
				if (branchType == PPC_BRANCH || branchType == PPC_BRANCHCOND) {
					string name = symbols.nameFromValue(address + branchOffset);
					symbolName = " <" + name + ">";
				}

				if(isLocation(address)) {
					result.push_back(printStringFormat("[line %d] 0x%x : %s %s", getSourceLine(address), address, opcode, operands) + symbolName);
				} else {
					result.push_back(printStringFormat("          0x%x : %s %s", address, opcode, operands) + symbolName);
				}
				//for hightlighting
				if(address == process.ip()) line = entry;
				entry++;

				// string op(opcode);
				// if(!op.compare("b") || !op.compare("blr") || !op.compare("bctr")) break;

				// address += 4;
			}
			return result;
	}
	int getDisassebmlyLine() {
		return line;
	}
	vector<string> hexDump(string addressString) {
		uint32_t address = (uint32_t)strtol(addressString.c_str(), 0, 0);
		vector<string> result;
		if(!address) {
			result.push_back("<not a readable address>");
			return result;
		}
		int entry = 1;
		uint32_t begin = address - address % 1024;
		for(uint32_t offset = 0x0; offset < 1024; offset+=32) {
			result.push_back(printStringFormat("0x%x + 0x%x : 0x%x 0x%x 0x%x 0x%x", begin, offset, *(uint32_t *)(begin + offset), *(uint32_t *)(begin + offset + 4), *(uint32_t *)(begin + offset + 8), *(uint32_t *)(begin + offset+ 12)));
			if(address - begin >= offset && address - begin < offset + 32) hexLine = entry;
			entry++;
		}
		return result;
	}
	int getHexLine() {
		return hexLine;
	}

	vector<string> registersDump() {
		vector<string> result;
		if(!process.lives()) return result;
		struct ExceptionContext *context = process.getContext();
		result.push_back(printStringFormat("Flags : %0x%x", context->Flags));    /* Flags, describing the context (READ-ONLY)*/
		result.push_back(printStringFormat("msr : 0x%x", context->msr));
		result.push_back(printStringFormat("ip : 0x%x", context->ip));
		for(int i = 0; i < 32; i++) {
			result.push_back(printStringFormat("gpr[%d] : 0x%x (%d)", i, context->gpr[i], context->gpr[i]));
		}
		result.push_back(printStringFormat("cr : 0x%x", context->cr));
		result.push_back(printStringFormat("xer : 0x%x", context->xer));
		result.push_back(printStringFormat("ctr : 0x%x", context->ctr));
		result.push_back(printStringFormat("lr : 0x%x", context->lr));
		result.push_back(printStringFormat("dsisr : 0x%x", context->dsisr));
		result.push_back(printStringFormat("dar : 0x%x", context->dar));
		for(int i = 0; i < 32; i++) {
			stringstream fstream;
			fstream.precision(5);
			fstream << context->fpr[i];
			string fpr;
			fstream >> fpr; 
			result.push_back(printStringFormat("fpr[%d] : %s", i, fpr.c_str()));
		}
		result.push_back(printStringFormat("fpscr : 0x%llx (%llu)", (uint64_t)context->fpscr, (uint64_t)context->fpscr));
		/* The following are only used on AltiVec */
		// uint8   vscr[16]; /* AltiVec vector status and control register */
		// uint8   vr[512];  /* AltiVec vector register storage */
		// uint32  vrsave;   /* AltiVec VRSAVE register */
		return result;

	}
	uint32_t getTrapSignal() {
		return process.getTrapSignal();
	}
	uint32_t getPortSignal() {
		return process.getPortSignal();
	}
	uint32_t getPipeSignal() {
		return process.getPipeSignal();
	}
	// vector<string> getMessages() {
	// 	return process.getMessages();
	// }
    void clear() {
		// cout << "debugger.clear()\n";
		linebreaks.clear();
		breaks.clear();
		if(handle) delete handle;
		if(binary) delete binary;
		handle = 0;
		binary = 0;
		symbols.clear();
		process.clear(); //resetSignals();
    }
	bool hasFunction() {
		if(!process.lives() || process.isRunning()) return false;
		return binary ? binary->getFunction(getIp()) != 0 : false;
	}
	bool isFunction(uint32_t address) {
		return binary ? binary->isFunction(address) : false;
	}
	string getFunctionName(uint32_t address) {
		return binary ? binary->getFunctionName(address) : string();
	}
	bool isLocation(uint32_t address) {
		return binary ? binary->isLocation(address) : false;
	}
	bool hasSymbols() {
		return binary != 0;
	}
	void addSourceRoot(string root) {
		roots.add(root);
	}
	void removeSourceRoot(string root) {
		roots.remove(root);
	}
	void clearRoots() {
		roots.clear();
	}
	list<string> getSourceRoots() {
		return roots.get();
	}
	string searchSourcePath(string file) {
		return roots.search(file);
	}
};

#if 0
int main(int argc, char *argv[])
{
	Debugger debugger;
	bool loaded = false;
	bool attached = false;

	bool exit = false;
	while(!exit) {
		if(!debugger.lives()) break;

		if(debugger.getSourceLine())
			cout << debugger.printLocation() << "\n";

		vector<string> args = getInput();

		if(args.size() > 0) 
		switch(char c = args[0][0]) {
			case 'l': {
				if(args.size() < 2)
					cout << "Missing argument(s)\n";
				else if(args.size() >= 3)
					loaded = debugger.load(args[1], concat(args, 2));
				else
					loaded = debugger.load(args[1], "");
				if(loaded) cout << "Process loaded\n";
			}
			break;

			case 'a': {
				if(args.size() < 2) {
					cout << "Missing argument\n";
				} else {
					if(loaded = debugger.attach(args[1]))
						cout << "Attached to process\n";
					attached = loaded;
				}
			}
			break;

			case 'd':
				cout << "Detach\n";
				if(attached) debugger.detach();
				attached = false;
				break;

			case 'p':
				if(args.size () < 2) {
					cout << "Missing argument\n";
					break;
				}
				if(!args[1].compare("sources")) {
					cout << "-- Source files: --\n";
					cout << vectorToString(debugger.sourceFiles());
					break;
				}
				if(!args[1].compare("ip")) {
					cout << "ip: " << (void *)debugger.getIp() << "\n";
					break;
				}
				if(!args[1].compare("sp")) {
					cout << "sp : 0x" << (void *)debugger.getSp() << "\n";
				}
				if(!args[1].compare("sym")) {
					cout << "--Elf symbols:--\n";
					cout << vectorToString(debugger.elfSymbols());
					break;
				}
				if(!args[1].compare("struct")) {
					cout << "--Binary structure:--\n";
					cout << debugger.binaryStructure();
					break;
				}
				if(!args[1].compare("stack")) {
					cout << "--Stack trace:--\n";
					cout << vectorToString(debugger.stacktrace());
					break;
				}
				break;

			case 'b':
			case 'c':
				if(args.size() < 2) {
					cout << "Missing argument\n";
				} else if (args.size() == 2) {
					if(debugger.breakpoint(args[1], c == 'b'))
						cout << "Breakpoint set\n";
					else
						cout << "Breakpoint not set\n";
				} else {
					astream str(args[2]);
					int line = str.getInt();
					if(debugger.breakpoint(args[1], line, c == 'b'))
						cout << "Breakpoint set\n";
					else
						cout << "Breakpoint not set\n";
				}
				break;
				
			case 's': {
				cout << "Start\n";
				debugger.start();
				break;
			}

			case 'k':
				cout << "Skip\n";
				debugger.skip();
				break;

			case 'z':
				cout << "Step\n";
				debugger.step();
				break;

			case 'w': { // write context data
				vector<string> symbols = debugger.context();
				int indent = 0;
				for(int i = 0; i < symbols.size(); i++) {
					astream str(symbols[i]);
					if(str.endsWith('}')) indent--;
					for(int j = 0; j < indent; j++)
						cout << "\t";
					if(str.endsWith('{')) indent++;
                	cout << symbols[i] << "\n";
				}
                break;
			}

			case 'g': { // write global symbols
				vector<string> symbols = debugger.globals();
				for(int i = 0; i < symbols.size(); i++)
                	cout << symbols[i] << "\n";
                break;
			}

			case 'e': {
				vector<string> output = debugger.emptyPipe();
				for(int i = 0; i < output.size(); i++)
					cout << "--] " << output[i] << "\n";
				break;
			}

			case 'f': //write place in code
				cout << debugger.printLocation() << ":\n";
				cout << vectorToString(debugger.functionSource());
				break;

			case '1': // step over
				if(loaded) debugger.stepOver();
				break;

			case '2': // step into
				if(loaded) debugger.stepInto();
				break;

			case '3': // step out
				if(loaded) debugger.stepOut();
				break;

			case 'q':
				exit = true;
				break;

			case 'h':
				cout << "-- SimpleDebug --\n";
				cout << "\n";
				cout << "l <file> <args>: load child from file\n";
				cout << "a <name>: attach to process in memory\n";
				cout << "d: detach from child\n";
				cout << "\n";
				cout << "p sources: print source file names\n";
				cout << "p ip: print instruction pointer\n";
				cout << "p sp: print stack pointer\n";
				cout << "p sym: print elf symbol list\n";
                cout << "p struct: print binary structure\n";
				cout << "p stack: print stack trace\n";
				cout << "\n";
				cout << "b <file> <line>: insert breakpoint\n";
				cout << "b <function>: insert breakpoint\n";
				cout << "c: clear breakpoint\n";
				cout << "\n";
				cout << "s: start execution\n";
				cout << "k: skip instruction\n";
				cout << "z: execute instruction\n";
				cout << "\n";
				cout << "w: write context data\n";
				cout << "g: write global symbols data\n";
				cout << "f: write text file location\n";
				cout << "\n";
				cout << "e: empty output pipe\n";
				cout << "\n";
				cout << "1: step over\n";
				cout << "2: step into\n";
				cout << "3: step out\n";
				cout << "\n";
				cout << "q: quit debugger\n";
				break;

			default:
				break;
		}
	}
	cout << "Farewell.\n";
    return 0;
}
#endif
#endif