//
//
// nativesymbols.cpp - native symbols handling by elf.libary (Debug 101)
//
//

#include <proto/elf.h>
#include "Symbols.hpp"

#include <sstream>
#include <string>
#include <string.h>

using namespace std;

#include "Handle.hpp"

ULONG amigaos_symbols_callback(struct Hook *hook, struct Task *task, struct SymbolMsg *symbolmsg) {
	if (symbolmsg->Name) {
		ElfSymbols *symbols = (ElfSymbols *)hook->h_Data;
		symbols->addSymbol (string ((const char *)symbolmsg->Name), symbolmsg->AbsValue, (uint16)symbolmsg->Sym->st_size);
	}
	return 1;
}

ElfSymbols::ElfSymbols() {
}

ElfSymbols::~ElfSymbols() {
	clear ();
}

void ElfSymbols::clear() {
	while(!symbols.empty()) delete symbols.front(), symbols.pop_front();
}

void ElfSymbols::readAll(ElfHandle *elfHandle) {
	APTR handle = elfHandle->getHandle();
	
	struct Hook hook;
	hook.h_Entry = (HOOKFUNC)amigaos_symbols_callback;
	hook.h_Data =  this;

	IElf->ScanSymbolTable((Elf32_Handle)handle, &hook, NULL);
	loaded = true;
}

void ElfSymbols::addSymbol(string name, uint32 value, uint16 size) {
	ElfSymbol *symbol = new ElfSymbol(name, value, size);
	symbols.push_back(symbol);
}

uint32 ElfSymbols::valueOf (string name) {
	for (list <ElfSymbol *>::iterator it = symbols.begin (); it != symbols.end (); it++)
		if (!(*it)->name.compare(name))
			return (*it)->value;
	return 0; //we should at least throw something in this case ??
}

uint16 ElfSymbols::sizeOf (string name) {
	for (list <ElfSymbol *>::iterator it = symbols.begin (); it != symbols.end (); it++)
		if (!(*it)->name.compare(name))
			return (*it)->size;
	return 0; //we should at least throw something in this case ??
}

string ElfSymbols::nameFromValue(uint32 value) {
	for (list <ElfSymbol *>::iterator it = symbols.begin (); it != symbols.end (); it++)
		if ((*it)->value <= value && value < (*it)->value + (*it)->size)
			return (*it)->name;
	return string(); //throw something in this case too ??
}

vector<string> ElfSymbols::printable(){
    vector<string> result;
	for (list<ElfSymbol *>::iterator it = symbols.begin (); it != symbols.end (); it++) {
		stringstream str;
        str << (*it)->name << ": 0x" << (void *)(*it)->value << "\n";
		result.push_back(str.str());
	}
	return result;
}