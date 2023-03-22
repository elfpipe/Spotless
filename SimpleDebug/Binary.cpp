#include "Binary.hpp"
#include "Breaks.hpp"
#include "../ReAction/Progress.hpp"
#include "symtabs.h"
#include <vector>
Type::~Type(){}
Array::Array(SourceObject *object, astream &str)
: Type(T_Array)
{
    str.peekSkip('a');
    if(str.peek() == 'r') {
        str.skip();
        TypeNo rNo(str);
        range = dynamic_cast<Range *>(object->findType(rNo));
        if(!range) {
            range = new Range(rNo, str);
            object->addType(range);
        }
    } else range = 0;
    if(str.eof()) {
        type = 0;
    } else {
        str.peekSkip(';');
        lower = str.getInt();
        str.peekSkip(';');
        upper = str.getInt();
        str.peekSkip(';');
        if(str.peek() == '(') {
            type = object->interpretType(str);
        } else type = 0;
    }
}
Struct::Struct(SourceObject *object, astream &str)
: Type(T_Struct)
{
    // cout << "struct :" << no.toString() << "\n";
    str.peekSkip('s');
    str.peekSkip('u');
    size = str.getInt();
    while(!str.eof()) {
        string name = str.get(':');
        Type *type = object->interpretType(str);
        str.peekSkip(',');
        uint64_t bitOffset = str.getInt();
        str.peekSkip(',');
        uint64_t bitSize = str.getInt();
        str.peekSkip(';');
        addEntry(name, type, bitOffset, bitSize);
        if(str.peek() == ';') {
            // cout << "= = = = = END OF STRUCT = = = = = =\n";
            str.skip();
            break;
        }
    }
}
Type *SourceObject::interpretType(astream &str) {
    if(str.eof()) return 0;
    str.peekSkip('='); //skip the '='
    char c = str.peek();
    Type *nType = 0;
    switch (c)
    {
        case 'R': // range
        case 'r': {
            Type::TypeNo no(str);
            Type *iType = findType(no);
            if(str.peek() == '=') {
                str.skip();
                if(str.peek() != 'r') {
                    Type *rType = interpretType(str);
                    nType = new Range(no, rType);
                }
                else
                    nType = new Range(no, str);
                addType(nType);
            } else if (str.peek() == ';') {
                nType = new Range(no, str);
                addType(nType);
            } else {
                nType = iType;
            }
            break;
        }
        case 'a': //array
            nType = new Array(this, str);
            break;
        case 's':  //struct or
        case 'u':  //union
            nType = new Struct(this, str);
            break;
        case 'e': //enum
            nType = new Enum(str);
            break;
        case '*': { //pointer
            Type::TypeNo no(str);
            Type *pType = findType(no);
            if(pType)
                nType = new Pointer(this, no, pType);
            if(str.peekSkip('=')) {
                nType = new Pointer(this, no, interpretType(str));
            } else if(!pType) {
                nType = new Pointer(this, no, nullptr);
            }
            addType(nType); //pointers should not come up in search (this is fixed in findType)
            break;
        }
        case 'x': //conformant array
            nType = new ConformantArray(this, str);
            break;
        case 'f': {
            Type::TypeNo no(str);
            Type *fType = findType(no);
            if(fType)
                nType = fType;
            if(str.peek() == '=') {
                nType = new FunctionType(no, interpretType(str));
                addType(nType);
            } else {
                nType = new FunctionType(no, nullptr);
                addType(nType);
            }
            break;
        }
        case 'p':
        case 'P':
        case 'G':
        case 't':
        case 'T':
        case 'S':
        case 'V':
        case '(': {
            Type::TypeNo no(str);
            nType = findType(no);
            Type::TypeNo iNo(0,7);
            if(str.peek() == '=') {
                nType = new Ref(this, no, interpretType(str));
                addType(nType);
            }
            break;
        }
        case 'c':
            break; // do nothing
        default:
            cout << "noise in interpretType() : character == \'" << c << "\'\n";
            break;
    }
    return nType;
}
Type *SourceObject::resolveType(Type::TypeNo no, int *pointers) {
    Type *iType = findType(no);
    return iType->resolve(pointers);
}
Symbol *SourceObject::interpretSymbol( astream &str, uint64_t address, unsigned char stabstype) {
    Symbol *result = 0;
    string name = str.get(':');
    char c = str.peek();
    Type *type = interpretType(str);
    switch(c) {
        case 't':
        case 'T':
            result = new Symbol(Symbol::S_Typedef, name, type, address);
            break;
        case '(':
            result = new Symbol(Symbol::S_Stack, name, type, address);
            break;
        case 'S':
        case 'V':
            result = new Symbol(Symbol::S_Absolute, name, type, address);
            break;
        case 'G':
            result = new Symbol(Symbol::S_Global, name, type, address);
            break;
        case 'p':
        case 'P':
            result = new Symbol(stabstype == N_RSYM ? Symbol::S_Register : Symbol::S_Param, name, type, address);
            break;
        case 'r':
            result = new Symbol(
                stabstype == N_LSYM ? Symbol::S_Stack :
                stabstype == N_RSYM ? Symbol::S_Register :
                stabstype == N_GSYM ? Symbol::S_Global :
                stabstype == N_PSYM ? Symbol::S_Param :
                Symbol::S_Typedef,
                    name, type, address
                );
                break;
        case 'c': {
            str.skip();
            str.peekSkip('=');
            char k = str.get();
            switch(k) {
                case 'i':
                    result = new ConstSymbol(name, str.getInt());
                    break;
                // case 'f':
                //     result = new ConstSymbol(name, str.getFloat());
                //     break;
                default:
                    cout << "noise in ConstSymbol : \'" << k << "\'\n";
                    break;
            }
        }
            break;
        default:
            cout << "noise in interpretSymbol : " << c << "\n";
            break;
    }
    return result;
}
vector<string> Ref::values(uint32_t base, int generation, int maxGeneration) {
    if(ref && ref->typeClass == T_ConformantArray) {
        Type *cType = so->findType(no, this);
        if(cType)
            return cType->values(base, generation, maxGeneration);
    }
    return ref ? ref->values(base, generation, maxGeneration) : vector<string>();
}
vector<string> ConformantArray::values(uint32_t base, int generation, int maxGeneration) {
    vector<string> result;
    Symbol *sym = object->findSymbolByName(name);
    if(sym && sym->type) return sym->type->values(base, generation, maxGeneration);
    else result.push_back("<unresolved conformant>");
    return result;
}
vector<string> Pointer::values(uint32_t base, int generation, int maxGeneration) {
    vector<string> result;

    uint32_t address = 0x0;
    cout << "address : " << (void*)base << "\n";
    if(base && is_readable_address_st(base)) { address = *(uint32_t *)base; }
    if(!is_readable_address_st(address)) {
        result.push_back("<no access>");
        return result;
    }
    if(pointsTo && pointsTo->typeClass == T_ConformantArray) {
        Type *cType = object->findType(no);
        if(cType) {
            return cType->values(address, generation, maxGeneration);
        }
        result.push_back("<unresolved conformant>");
        return result;
    }
    int pointers = 0;
    if(pointsTo && pointsTo->resolve(&pointers)) {
        if(pointers == 0 && pointsTo->resolve(&pointers)->typeClass == T_Range && pointsTo->byteSize() == 1) {
            // cout << "string perhaps?\n";
                // we have a string, perhaps?
            if(is_readable_string(address))
                result.push_back(printStringFormat("(char *) (0x%x) \"%s\"", address, address));
            else result.push_back("<void>");
            // cout << "string done.\n";
            return result;
        }
    }
    vector<string> v;
    if (generation <= maxGeneration) {
        if(pointsTo && address) v = pointsTo->values(address, generation + 1, maxGeneration);
        if(v.size() == 1)
            result.push_back(printStringFormat("(*) (0x%x) %s", address, v[0].c_str()));
        else
            result.insert(result.end(), v.begin(), v.end());
    }
    return result;
}

Type *ConformantArray::resolve(int *pointer)
{
    Symbol *sym = object->findSymbolByName(name);
    if(sym && sym->type) return sym->type->resolve(pointer);
    return this;
}

Function *SourceObject::interpretFun(astream &str, uint64_t address) {
    Function *result = 0;
    string name = str.get(':');
    char c = str.peek();
    switch(c) {
        case 'F':
        case 'f': {
            str.skip();
            Type *type = interpretType(str);
            result = new Function(name, type, address);
            break;
        }
        default:
            break;
    }
    return result;
}
void SourceObject::doEXCL(SymtabEntry *_sym, SymtabEntry *stab, const char *stabstr, uint64_t stabsize)
{
    SymtabEntry *sym = _sym;
    bool found = false;
    string filename = string(stabstr + _sym->n_strx);
    while(!found) {
        sym--;
        string newname = string(stabstr + sym->n_strx);
        if(sym->n_type == N_BINCL && !newname.compare(filename)) found = true;
        if(sym == stab) break;
    }
    if(!found) return;

    int nested = 0;
	while ((uint32_t)sym < (uint32_t)stab + stabsize) {
        astream str(string(stabstr + sym->n_strx));
		switch (sym->n_type) {
			case N_LSYM: {
                    Symbol *symbol = interpretSymbol(str, sym->n_value, sym->n_type);
                    if(symbol) locals.push_back(symbol);
                }
                break;
            case N_BINCL: nested++; break;
            case N_EINCL: if(!--nested) return;
            case N_EXCL: doEXCL(sym, stab, stabstr, stabsize); break;
            default:
                cout << "Unknown n_type in doEXCL : " << (void *)(unsigned int)sym->n_type << " .\n";
                break;
        }
        sym++;
    }
}
SourceObject::SourceObject(SymtabEntry **_sym, SymtabEntry *stab, const char *stabstr, uint64_t stabsize) {
    secondDone = false;
    this->stab = stab;
    this->stabstr = stabstr;
    this->stabsize = stabsize;
    this->_sym = *_sym;
    this->noSym = 0;
    SymtabEntry *sym = this->_sym;
    name = string(stabstr + sym->n_strx);
    start = sym->n_value;
    sym++;

	while ((uint32_t)sym < (uint32_t)stab + stabsize) {
		switch (sym->n_type) {
            case N_SO:
                end = sym->n_value;
                *_sym = ++sym;
                noSym = sym - this->_sym;

                // cout << "SourceObject name : " << name << " start : " << (void*)start << " end : " << (void*)end << "\n";

                return;
        }
        sym++;
    }
    *_sym = sym;
}
void SourceObject::secondPass() { //SymtabEntry **_sym, SymtabEntry *stab, const char *stabstr, uint64_t stabsize) {
    if(secondDone) return;
    ProgressWindow progress;
    progress.open("Loading stabs (second pass)...", (unsigned int)noSym, 0);

    astream temp("r(0,0);0;-1;");
    addType(new Range(Type::TypeNo(0, 0), temp));

    SymtabEntry *sym = _sym;
    name = string(stabstr + sym->n_strx);
    start = sym->n_value;
    sym++;

    int is = 0;
    string source = name;
    Function *function = 0;
    Scope *scope = 0;
    bool closeScope = false;
    vector<Symbol *> symbols;
	while ((uint32_t)sym < (uint32_t)stab + stabsize) {
        string s(stabstr + sym->n_strx);
        progress.updateLevel(is++);
        // cout << "str : " << s << "\n";
        astream str(s);
		switch (sym->n_type) {
            case N_SO:
                end = sym->n_value;
                // *_sym = ++sym;
                if(closeScope) scope->end = end-4; //  function->address + function->lines[function->lines.size()-1]->address;
                secondDone = true;
                progress.close();
                return;
            case N_SOL:
                source = string(stabstr + sym->n_strx);
                break;
			case N_LSYM: {
                if(function) {
                    Symbol *symbol = interpretSymbol(str, sym->n_value, sym->n_type);
                    if(symbol) symbols.push_back(symbol);
                }
                else {
                    Symbol *symbol = interpretSymbol(str, sym->n_value, sym->n_type);
                    if(symbol) locals.push_back(symbol);
                }
                break;
            }
            case N_LCSYM:
            case N_STSYM:
            case N_ROSYM: {
                astream test(s);
                test.get(':');
                char c = test.peek();
                switch (c) {
                    case 'S': {
                        Symbol *symbol = interpretSymbol(str, sym->n_value, sym->n_type);
                        if(symbol)
                            globals.push_back(symbol);
                    }
                    break;

                    case 'V': {
                        if(function) {
                            Symbol *symbol = interpretSymbol(str, sym->n_value, sym->n_type);
                            if(symbol) symbols.push_back(symbol);
                        }
                        else {
                            Symbol *symbol = interpretSymbol(str, sym->n_value, sym->n_type);
                            if(symbol) globals.push_back(symbol); // stray 'V' symbols go into the globals
                        }
                    }
                    break;
                }
            }
            break;

			case N_GSYM: {
                Symbol *symbol = interpretSymbol(str, sym->n_value, sym->n_type);
                if(symbol)
                    globals.push_back(symbol);
                break;
            }
            case N_FUN: {
                if(closeScope) {
                    scope->end = function->address + function->lines[function->lines.size()-1]->address;
                    closeScope = false;
                }
                function = interpretFun(str, sym->n_value);
                if(function) {
                    function->locals.push_back(scope = new Scope(0, function->address, symbols)); //there has to be a scope
                    functions.push_back(function);
                    symbols.clear();
                    closeScope = true;
                }
                break;
            }
            case N_RSYM: // register sym
			case N_PSYM: {
                Symbol *symbol = interpretSymbol(str, sym->n_value, sym->n_type);
                function->params.push_back(symbol);
                break;
            }
            case N_SLINE:
                function->addLine(sym->n_value, sym->n_desc, source);
                break;
            case N_LBRAC: {
                if(scope)
                    scope->children.push_back(scope = new Scope(scope, function->address + sym->n_value, symbols));
                symbols.clear();
                closeScope = false;
                break;
            }
            case N_RBRAC:
                if(scope) {
                    scope->end = function->address + sym->n_value;
                    scope = scope->parent;
                }
                // if(scope)
                //     scope->end = function->address + sym->n_value;
                if(scope && scope->parent == 0) {
                    scope->end = scope->children.size() ? scope->children[0]->end : function->lines.size() ? function->lines[function->lines.size()-1]->address : function->address;
                }
                break;
            case N_EXCL:
                doEXCL(sym, stab, stabstr, stabsize);
                break;
            default:
                break;
        }
        sym++;
    }
    // *_sym = sym;
    secondDone = true;
    progress.close();
}
string SourceObject::toString() {
    string result = name + "<SO> : [ " + patch::toString((void *)start) + "," + patch::toString((void *)end) + " ] --- {\n";
    for(vector<Type *>::iterator it = types.begin(); it != types.end(); it++)
        result += (*it)->toString() + "\n";
    for(vector<Symbol *>::iterator it = locals.begin(); it != locals.end(); it++)
        result += (*it)->toString() + "\n";
    for(vector<Symbol *>::iterator it = globals.begin(); it != globals.end(); it++)
        result += (*it)->toString() + "\n";
    for(vector<Function *>::iterator it = functions.begin(); it != functions.end(); it++)
        result += (*it)->toString() + "\n";
    return result + "}\n";
}
Symbol *SourceObject::findSymbolByName(string name) {
    for(vector<Symbol *>::iterator it = locals.begin(); it != locals.end(); it++) {
        if(!name.compare((*it)->name)) return (*it);
    }
    for(vector<Symbol *>::iterator it = globals.begin(); it != globals.end(); it++) {
        if(!name.compare((*it)->name)) return (*it);
    }
    return nullptr;
}
Binary::Binary(string name, SymtabEntry *stab, const char *stabstr, uint64_t stabsize) {
    addModule(name, stab, stabstr, stabsize);
    // ProgressWindow progress;
    // progress.open("Loading stabs...", (unsigned int)stabsize, 0);
    // this->name = name;
    // this->stab = stab;
    // this->stabstr = stabstr;
    // this->stabsize = stabsize;
	// SymtabEntry *sym = stab;
	// while ((uint32_t)sym < (uint32_t)stab + stabsize) {
    //     // cout << "SO: " << string(stabstr + sym->n_strx) << "\n";
    //     progress.updateLevel((uint32_t)sym - (uint32_t)stab);
	// 	switch (sym->n_type) {
    //         case N_SO:
    //             objects.push_back(new SourceObject(&sym, stab, stabstr, stabsize));
    //             continue;
    //         default:
    //             break;
    //     }
    //     sym++;
    // }
    // progress.close();
}
void Binary::addModule(string name, SymtabEntry *stab, const char *stabstr, uint64_t stabsize) {
    Module *module = new Module(name, stab, stabstr, stabsize);
	SymtabEntry *sym = stab;
	while ((uint32_t)sym < (uint32_t)stab + stabsize) {
        // cout << "SO: " << string(stabstr + sym->n_strx) << "\n";
        // progress.updateLevel((uint32_t)sym - (uint32_t)stab);
		switch (sym->n_type) {
            case N_SO:
                objects.push_back(new SourceObject(&sym, stab, stabstr, stabsize));
                continue;
            default:
                break;
        }
        sym++;
    }
    modules.push_back(module);
    // progress.close();
}
SourceObject *Binary::findSourceObject(uint32_t address) {
    // cout << "findSourceObject() address : " << (void*)address << "\n";
    for(vector<SourceObject *>::iterator it = objects.begin(); it != objects.end(); it++)
        if((*it)->start <= address && (*it)->end >= address) {
            // cout << "match\n";
            if(!(*it)->secondDone) (*it)->secondPass();
            return (*it);
        }
    return 0;
}
vector<string> Binary::getSourceNames() {
    vector<string> result;
    ProgressWindow progress;
    int fullSize = 0;
    for(vector<Module *>::iterator it = modules.begin(); it != modules.end(); it++)
        fullSize += (*it)->stabsize;
    progress.open("Fetching sources...", fullSize, 0);
    int p = 0;
    for(vector<Module *>::iterator it = modules.begin(); it != modules.end(); it++) {
        SymtabEntry *sym = (*it)->stab;
        while ((uint32_t)sym < (uint32_t)(*it)->stab + (uint32_t)(*it)->stabsize) {
            progress.updateLevel(p);
            string str((*it)->stabstr + sym->n_strx);
            switch (sym->n_type) {
                case N_SO: {
                    if(str.size()) result.push_back(str);
                    break;
                }
                case N_SOL: {
                    bool save = true;
                    for(vector<string>::iterator it = result.begin(); it != result.end(); it++)
                        if((*it).compare(str)) { save = false; break; }
                    if(save) result.push_back(str);
                }
                    break;
                default:
                    break;
            }
            sym++;
            p += sizeof(SymtabEntry);
        }
    }

    // uint32_t p = 0;
    // for(vector<SourceObject *>::iterator it = objects.begin(); it != objects.end(); it++) {
    //     progress.updateLevel(p++);

    //     result.push_back((*it)->name);
    //     for(vector<Function *>::iterator itf = (*it)->functions.begin(); itf != (*it)->functions.end(); itf++) {
    //         for(vector<Function::SLine *>::iterator its = (*itf)->lines.begin(); its != (*itf)->lines.end(); its++)
    //             if(!patch::contains(result, (*its)->source))
    //                 result.push_back((*its)->source);
    //     }
    // }
    progress.close();
    return result;
}
// uint32_t Binary::getLineAddress(string file, int line) {
//     for(int i = 0; i < objects.size(); i++) {
//         SourceObject *object = objects[i];
//         for(int j = 0; j < object->functions.size(); j++) {
//             Function *function = object->functions[j];
//             for(int k = 0; k < function->lines.size(); k++) {
//                 Function::SLine *sline = function->lines[k];
//                 if(!sline->source.compare(file) && sline->line == line) {
//                     return function->address + sline->address; }
//             }
//         }
//     }
//     return 0x0;
// }
vector<uint32_t> Binary::getLineAddresses(string file, int line) {
    vector<uint32_t> result;
    for(vector<Module *>::iterator it = modules.begin(); it != modules.end(); it++) {
        SymtabEntry *sym = (*it)->stab;
        bool on = false;
        uint32_t faddress = 0x0;
        while ((uint32_t)sym < (uint32_t)(*it)->stab + (*it)->stabsize) {
            string str((*it)->stabstr + sym->n_strx);
            switch (sym->n_type) {
                case N_SOL:
                case N_SO:
                    if(!str.compare(file)) on = true;
                    else on = false;
                    break;
                case N_FUN:
                    faddress = sym->n_value;
                    break;
                case N_SLINE:
                    if(on && line == sym->n_desc) result.push_back(faddress + sym->n_value);
                    break;
                default:
                    break;
            }
            sym++;
        }
    }
    // for(int i = 0; i < objects.size(); i++) {
    //     SourceObject *object = objects[i];
    //     for(int j = 0; j < object->functions.size(); j++) {
    //         Function *function = object->functions[j];
    //         for(int k = 0; k < function->lines.size(); k++) {
    //             Function::SLine *sline = function->lines[k];
    //             if(!sline->source.compare(file) && sline->line == line) {
    //                 // return function->address + sline->address;
    //                 result.push_back(function->address + sline->address);
    //             }
    //         }
    //     }
    // }
    return result;
}
void Binary::getLinesAndBreaks(string file, Breaks &b, vector<int> &lines, vector<int> &breaks) {
    for(vector<Module *>::iterator it = modules.begin(); it != modules.end(); it++) {
        SymtabEntry *sym = (*it)->stab;
        bool on = false;
        uint32_t faddress = 0x0;
        while ((uint32_t)sym < (uint32_t)(*it)->stab + (*it)->stabsize) {
            string str((*it)->stabstr + sym->n_strx);
            switch (sym->n_type) {
                case N_SOL:
                case N_SO:
                    if(!str.compare(file)) on = true;
                    else on = false;
                    break;
                case N_FUN:
                    faddress = sym->n_value;
                    break;
                case N_SLINE:
                    if(on) {
                        uint32_t addr = faddress + sym->n_value;
                        lines.push_back(sym->n_desc);
                        if(b.isBreak(addr)) breaks.push_back(sym->n_desc);
                    }
                    break;
                default:
                    break;
            }
            sym++;
        }
    }
}
Function *Binary::getFunction(uint32_t address) {

    SourceObject *object = findSourceObject(address);
    // for(int i = 0; i < objects.size(); i++) {
    //     SourceObject *object = objects[i];
    // cout << "getFunction object : " << (void*)object << "\n";
    if(object)
        for(int j = 0; j < object->functions.size(); j++) {
            // cout << "function j : " << j << "\n";
            Function *function = object->functions[j];
            Scope *scope = function->locals[0];
            if(scope && scope->begin <= address && scope->end >= address) {
                // cout << "match\n";
                return function;
            }
        }
    // }
    return 0;
}
Function::SLine *Binary::getLocation(uint32_t address) {
    Function *function = getFunction(address);
    if(function) for(int k = 0; k < function->lines.size(); k++) {
        Function::SLine *sline = function->lines[k];
        if(function->address + sline->address == address)
            return sline;
        if(function->address + sline->address > address) //relevant for return statements
            return function->lines[k-1];
    }
    return 0;
}
bool Binary::isLocation(uint32_t address) {
    Function *function = getFunction(address);
    if(function) for(int k = 0; k < function->lines.size(); k++) {
        Function::SLine *sline = function->lines[k];
        if(function->address + sline->address == address)
            return true;
    }
    return false;
}
bool Binary::isBinary(uint32_t address) {
    // for(vector<SourceObject *>::iterator it = objects.begin(); it != objects.end(); it++) {
    //     if(address >= (*it)->start && address <= (*it)->end) return true;
    // }
    SourceObject *object = findSourceObject(address);
    if(object) return true;
    return false;
}
bool Binary::isFunction(uint32_t address) {
    Function *function = getFunction(address);
    if(function) {
        // Function::SLine *sline = function->lines[0];
        // if(function->address + sline->address == address)
            return true;
    }
    return false;
}
string Binary::getFunctionName(uint32_t address) {
    Function *function = getFunction(address);
    if(function) return function->name;
    return string();
}
string Binary::getSourceFile(uint32_t address) {
    Function::SLine *line = getLocation(address);
    if (line) return line->source;
    return string();
}
int Binary::getSourceLine(uint32_t address) {
    Function::SLine *line = getLocation(address);
    if (line) return line->line;
    return 0;
}
// bool Binary::isLastLine(uint32_t address) {
//     Function *function = getFunction(address);
//     Function::SLine *line = getLocation(address);
//     if(function->lines.back() == line) return true;
//     return false;
// }
// uint32_t Binary::getFunctionAddress(string name) {
//     // for(int i = 0; i < objects.size(); i++) {
//     //     SourceObject *object = objects[i];
//         for(int j = 0; j < object->functions.size(); j++) {
//             Function *function = object->functions[j];
//             if(!function->name.compare(name))
//                 return function->address;
//         }
//     // }
//     return 0x0;
// }
vector<string> Binary::getContext(struct ExceptionContext *eContext, uint32_t ip, uint32_t sp) {
    vector<string> result;
    Function *function = getFunction(ip);
    if(!function) return result;

    /* parameters */
    for(int i = 0; i < function->params.size(); i++) {
        if (function->params[i]) {
            if(function->params[i]->symType == Symbol::S_Register) {
                // cout << "S_Register : address == " << function->params[i]->address << " gpr == " << (void *)eContext->gpr[function->params[i]->address] << "\n";
                // printf("string : %s\n", (const char *)eContext->gpr[function->params[i]->address]);
                // cout << "address of register : " << (void *)&eContext->gpr[function->params[i]->address] << "\n";
                vector<string> values = function->params[i]->values((uint32_t)&eContext->gpr[function->params[i]->address]);
                result.insert(result.end(), values.begin(), values.end());
            } else {
                vector<string> values = function->params[i]->values(sp);
                result.insert(result.end(), values.begin(), values.end());
            }
        } else {
            result.push_back("<no symbol info>");
        }
    }

    /* locals */
    Scope *scope = function->locals[0];
    if(scope) scope = scope->getScope(ip);
    while(scope) {
        for(int j = 0; j < scope->symbols.size(); j++) {
            vector<string> values;
            if(scope->symbols[j]->symType == Symbol::S_Absolute)
                values = scope->symbols[j]->values(0); // base == 0, so just the absolute address
            else values = scope->symbols[j]->values(sp);
            result.insert(result.end(), values.begin(), values.end());
        }
        scope = scope->parent;
    }
    return result;
}
vector<string> Binary::getGlobals(ElfSymbols &symbols) {
    ProgressWindow progress;
    progress.open("Globals...", (unsigned int)objects.size(), 0);
    vector<string> result;
    for(int i = 0; i < objects.size(); i++) {
        progress.updateLevel(i);
        SourceObject *object = objects[i];
        for(int j = 0; j < object->globals.size(); j++) {
            Symbol *symbol = object->globals[j];
            // if(symbol->symType == Symbol::S_Global) {
                if(symbol->address) {
                    vector<string> values = symbol->values(0);
                    result.insert(result.begin(), values.begin(), values.end());
                } else
                if(symbols.valueOf(symbol->name)) {
                    vector<string> values = symbol->values(symbols.valueOf(symbol->name));
                    result.insert(result.begin(), values.begin(), values.end());
                }
            // }
        }
    }
    progress.close();
    return result;
}
string Binary::toString() {
    string result = "<Binary> : {\n"; // [ STAB: 0x" + patch::toString((void*)stab) + " STABSTR: 0x" + patch::toString((void *)stabstr) + " STABSIZE: " + patch::toString((int)stabsize) + "] -- {\n";
    for(vector<SourceObject *>::iterator it = objects.begin(); it != objects.end(); it++)
        result += (*it)->toString();
    return result + "}\n";
}