//
// nativesymbols.hpp - native symbol handling by elf handle for symbol table lookup
//

#ifndef DB101_NATIVESYMBOLS_hpp
#define DB101_NATIVESYMBOLS_hpp

#include <string>
#include <list>
#include <vector>

#include <exec/types.h>

using namespace std;

class ElfHandle;
class ElfSymbols
{
public:
	struct ElfSymbol {
		string name;
		uint32 value;
		uint16 size;

		ElfSymbol (string _name, uint32 _value, uint16 _size) : name(_name), value(_value), size(_size) { }
	};

private:
	list <ElfSymbol *> symbols;
	bool loaded;

public:
	ElfSymbols();
	~ElfSymbols();

	bool hasSymbols() { return loaded; }
	void clear();
	
	void readAll(ElfHandle *handle);
	void addSymbol(string name, uint32 value, uint16 size);
	
	uint32 valueOf(string name);
	uint16 sizeOf (string name);
	string nameFromValue(uint32 value);

    vector<string> printable();
};
#endif
