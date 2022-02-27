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

extern struct DebugIFace *IDebug;

using namespace std;

// ---------------------------------------------------------------------------- //

class Debugger {
private:
	AmigaProcess process;
    ElfSymbols symbols;
    Breaks breaks, linebreaks;

	ElfHandle *handle = 0;
	Binary *binary = 0;

	Roots roots;

	bool suspended = false;
	int line = 0;

	string entryPoint;

public:
	Debugger() {
		entryPoint = "main";
	}
	~Debugger() {
		clear();
	}
	void open(APTR _handle, string name) {
		handle = new ElfHandle(_handle, name);

		symbols.readAll(handle);

		if(handle->performRelocation())
			binary = new Binary(handle->getName(), (SymtabEntry *)handle->getStabSection(), handle->getStabstrSection(), handle->getStabsSize());
	}
	bool load(string path, string file, string args) {
		// clear();
		APTR handle = process.load(path, file, args);
		roots.clear();
		roots.add(path);
		if (handle) open(handle, file);

		//experimental entry code, to prevent breaks in kernel :
		if(entryPoint.size() && symbols.hasSymbols()) {
			uint32 value = symbols.valueOf(entryPoint.c_str());
			if(value) {
				Breaks temp;
				temp.insert(value);
				temp.activate();
				process.go();
				process.wait();
			}
		}

		// return success
		return handle != 0;
	}
	bool attach(string name) {
		// clear();
		APTR handle = process.attach(name);
		if(handle) open(handle, name);
		return handle != 0;
	}
	void detach() {
		process.detach();
	}
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
		if(address)	set ? breaks.insert(address) : breaks.remove(address);
		return address != 0;
	}
	bool breakpoint(string function, bool set) {
		if(!binary) return false;
		uint32_t address = binary->getFunctionAddress(function);
		if(address)	set ? breaks.insert(address) : breaks.remove(address);
		return address != 0;
	}
	void suspendBreaks() {
		if(!process.isDead()) { //necessary on 440ep
			linebreaks.deactivate();
			breaks.deactivate();
			linebreaks.clear();
		}
	}
	void start() {
		if(isDead()) return;

		if (!suspended) {
			process.step();
		}
		breaks.activate();
		process.go();
		// process.wait();
		// breaks.deactivate(); //do the last bit in trap handler
	}
	void stop() {
		process.suspend();
		suspended = true;
	}
	void wait() {
		process.wait();
	}
	void skip() {
		process.skip();
	}
	void step() {
		process.step();
	}
	void stepOver() {
		if(!binary || isDead()) return;

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
		if(!binary || isDead()) return;

		// if (binary->isLastLine(process.ip())) {
		// 	start();
		// 	return;
		// }
		do {
			int32 dummy;
			if(PPC_DisassembleBranchInstr(*(uint32 *)process.ip(), &dummy) == PPC_BRANCHTOLINK) {
				start();
				return;
			}

			if(binary->getSourceFile(process.branchAddress()).size() > 0)
				process.step();
			else
				process.stepNoBranch();
		} while(!binary->isLocation(process.ip()));
		//process.wakeUp();
	}
	void stepOut() {
		if (!binary || isDead()) return;

		Breaks outBreak;
		if(binary->getSourceFile(process.lr()).size() > 0)
			outBreak.insert(process.lr());

		process.step();

		breaks.activate();
		outBreak.activate();

		process.go();
		process.wait();

		if(!process.isDead()) {
			outBreak.deactivate();
			breaks.deactivate();
		}
	}
	vector<string> context() {
		return binary ? binary->getContext(process.ip(), process.sp()) : vector<string>();
	}
	vector<string> globals() {
		return binary ? binary->getGlobals(symbols) : vector<string>();
	}
	string getEntryPoint() {
		return entryPoint;
	}
	void setEntryPoint(string newEntry) {
		entryPoint = newEntry;
	}
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
	int getSourceLine(uint32_t address) {
		return binary ? binary->getSourceLine(address) : 0;
	}
	bool isSourceLine(string file, int line) {
		return binary ? binary->getLineAddress(file, line) : false;
	}
	bool isBreakpoint(string file, int line) {
		return binary ? breaks.isBreak(binary->getLineAddress(file, line)) : false;
	}
	string printLocation() {
		return binary ? binary->getSourceFile(process.ip()) + " at line " + patch::toString(binary->getSourceLine(process.ip())) : string();
	}
	bool isDead() {
		return process.isDead();
	}
	bool isRunning() {
		return process.isRunning();
	}
	vector<string> emptyPipe() {
		return process.emptyPipe();
	}
	vector<string> stacktrace() {
		Stacktracer stacktracer;
		return process.isDead() ? vector<string>() : stacktracer.stacktrace((Task *)process.getProcess(), getSp());
	}
	vector<string> functionSource() {
		vector<string> result;
		string source = binary->getSourceFile(process.ip());
		cout << "Source file : " << source << "\n"; 
		cout << "ip : " << (void *)process.ip();
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
		vector<string> result;
		Function *function = binary ? binary->getFunction(getIp()) : 0;
		if(!function) return result;
		int entry = 1;
		result.push_back(function->name + " :");
        for(int i = 0; i < function->lines.size(); i++) {
			int maxOffset = function->lines[i]->address + 4;
			if(i < function->lines.size() - 1)
				maxOffset = function->lines[i+1]->address;

			for(uint32_t offset = function->lines[i]->address; offset < maxOffset; offset += 4) {
				uint32_t address = function->address + offset;
				char opcode[256], operands[256];
				IDebug->DisassembleNative((APTR)address, opcode, operands);

				entry++;
				if(isLocation(address)) {
					result.push_back(printStringFormat("[line %d] 0x%x : %s %s", getSourceLine(address), address, opcode, operands));
				} else {
					result.push_back(printStringFormat("          0x%x : %s %s", address, opcode, operands));
				}

				//for hightlighting
				if(address == getIp()) line = entry;
			}
        }
		return result;
	}
	int getDisassebmlyLine() {
		return line;
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
	vector<string> getMessages() {
		return process.getMessages();
	}
    void clear() {
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
		if(debugger.isDead()) break;

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