#ifndef DEFINITIONS02_HPP
#define DEFINITIONS02_HPP

#include "Strings.hpp"
#include "LowLevel.hpp"
#include "Symbols.hpp"
#include "symtabs.h"

#include <string>
#include <vector>
#include <limits.h>
using namespace std;
using namespace patch;
class SourceObject;
class Scope;
class Type {
public:
    class TypeNo {
    private:
        int t1, t2;
    public:
        bool equals(TypeNo &other) {
            return (t1 == other.t1 && t2 == other.t2);
        }
        TypeNo(int t1, int t2) {
            this->t1 = t1;
            this->t2 = t2;
        }
        TypeNo() { this->t1 = -1; this->t2 = -1; }
        TypeNo(astream &str)
        : TypeNo()
        {
            if(str.peek() != '(') str.skip();
            if(str.peek() == '(') {
                str.skip();
                t1 = str.getInt();
                if(str.peek() == ',')
                    str.skip();
                t2 = str.getInt();
                if(str.peek() == ')')
                    str.skip();
            } else t2 = str.getInt();
        }
        string toString() {
            return "(" + patch::toString(t1) + "," + patch::toString(t2) + ")";
        }
    };
public:
    typedef enum {
        T_Void,
        T_Range,
        T_Ref,
        T_Array,
        T_Struct,
        T_Union,
        T_Enum,
        T_Pointer,
        T_ConformantArray,
        T_Function
    } TypeClass;
    TypeClass typeClass;
    TypeNo no;
public:
    Type(TypeClass typeClass, TypeNo no) {
        this->typeClass = typeClass;
        this->no = no;
    }
    Type(TypeClass typeClass) {
        this->typeClass = typeClass;
        this->no = TypeNo();
    }
    virtual ~Type();
    virtual string toString() = 0;
    virtual uint32_t byteSize() = 0;
    virtual vector<string> values(uint32_t base, int generation, int maxGeneration) = 0;
    virtual Type *resolve(int *pointer) = 0;
};
class Ref : public Type {
public:
    Type *ref;
public:
    Ref(Type::TypeNo no, Type *ref)
    : Type(T_Ref, no)
    {
        this->ref = ref;
    }
    // Ref(SourceObject *o, TypeNo no, astream &str);
    string toString() {
        return ref ? ref->toString() : string();
    }
    uint32_t byteSize() {
        return ref->byteSize();
    }
    vector<string> values(uint32_t base, int generation, int maxGeneration) {
        return ref ? ref->values(base, generation, maxGeneration) : vector<string>();
    };
    Type *resolve(int *pointer) { return ref; }
};
class Void : public Type {
public:
    Void(Type::TypeNo no)
    : Type(T_Void, no)
    {}
public:
    string toString() {
        return "<void>";
    }
    uint32_t byteSize() {
        return 0;
    }
    vector<string> values(uint32_t base, int generation, int maxGeneration) {
        vector<string> result;
        result.push_back("<void>");
        return result;
    };
    Type *resolve(int *pointer) { return this; }
};
class Range : public Type {
public:
    typedef enum {
        R_Int64,
        R_UInt64,
        R_Float32,
        R_Float64,
        R_Float128,
        R_Complex8,
        R_Complex16,
        R_Complex32,
        R_Defined
    } RangeType;
    RangeType rangeType;
    int64_t lower, upper;
    Type *ref;
    Range(Type::TypeNo no, Type *ref)
    : Type(T_Range, no)
    { this->ref = ref; }
    Range(Type::TypeNo no, astream &str)
    : Type(T_Range, no), ref(0)
    {
        str.peekSkip('=');
        char c = str.peek();
        if(c == 'r' || c == '(' || c == ';') {
            if(c == 'r') str.skip();
            if(str.peek() == '(')
                TypeNo newNo(str);
            str.peekSkip(';');
            lower = str.getInt();
            str.peekSkip(';');
            upper = str.getInt();
            str.peekSkip(';');
            if(lower == 0 && upper == -1) {
                rangeType = R_UInt64;
                lower = 0;
                upper = UINT_MAX;
            } else if (lower == 1 && upper == 0) {
                rangeType = R_Int64;
                lower = -INT_MAX-1;
                upper = INT_MAX;
            } else if (upper == 0) {
                if(lower == 4) {
                    rangeType = R_Float32;
                } else if (lower == 8) {
                    rangeType = R_Float64;
                } else if (lower == 16) {
                    rangeType = R_Float128;
                }
            } else {
                rangeType = R_Defined;
            }
        } else if(c == 'R') {
            str.skip();
            uint32_t kind = str.getInt();
            str.peekSkip(';');
            uint32_t bytes = str.getInt();
            str.peekSkip(';');
            str.getInt(); //swallow
            str.peekSkip(';');
            switch(kind) {
                case 1: //NF_SINGLE
                    rangeType = R_Float32;
                    break;
                case 2: //NF_DOUBLE
                    rangeType = R_Float64;
                    break;
                case 3: //NF_COMPLEX
                    rangeType = R_Complex8;
                    break;
                case 4: //NF_COMPLEX16
                    rangeType = R_Complex16;
                    break;
                case 5: //NF_COMPLEX32
                    rangeType = R_Complex32;
                    break;
                case 6: //NF_LDOUBLE
                    rangeType = R_Float128;
                    break;
            }
            if(bytes == 16 && rangeType == R_Complex8)
                rangeType = R_Complex16;
            if(bytes == 32 && rangeType == R_Complex8)
                rangeType = R_Complex32;
        } else {

        }
    }
    uint32_t byteSize() {
        switch(rangeType) {
            case R_UInt64:
                return 8;
            case R_Float32:
                return 4;
            case R_Float64:
                return 8;
            case R_Float128:
                return 16;
            case R_Complex8:
                return 8;
            case R_Complex16:
                return 16;
            case R_Complex32:
                return 32;
            case R_Defined:
                if(upper <= 255)
                    return 1;
                else if (upper <= 65535)
                    return 2;
                else if (upper <= 4294967295)
                    return 4;
                else if (upper <= 0xffffffffffffffff)
                    return 8;
            default:
                return 0;
        }
    }
    string toString() {
        string result("r" + no.toString() + " : ");
        switch(rangeType) {
            case R_Int64:
                result += "<Int64>";
                break;
            case R_UInt64:
                result += "<UInt64>";
                break;
            case R_Float32:
                result += "<Float32>";
                break;
            case R_Float64:
                result += "<Float64>";
                break;
            case R_Float128:
                result += "<Float128>";
                break;
            case R_Complex8:
                result += "<Complex8>";
                break;
            case R_Complex16:
                result += "<Complex16>";
                break;
            case R_Complex32:
                result += "<Complex32>";
                break;
            case R_Defined:
                result += "Def(" + patch::toString((int)lower) + "," + patch::toString((int)upper) + ")";
                break;
        }
        return result; // + patch::toString((unsigned int)byteSize());
    }
    vector<string> values(uint32_t base, int generation, int maxGeneration) {
        if(ref) return ref->values(base, generation, maxGeneration);

        vector<string> result;
        if(!base || !is_readable_address_st(base)) {
            result.push_back("<no access>");
            return result;
        }

        switch(rangeType) {
            case R_Int64: {
		        result.push_back(printStringFormat("%lld", *(uint64_t *)base));
                break;
            }
            case R_UInt64: {
		        result.push_back(printStringFormat("%llu", *(uint64_t *)base));
                break;
            }
            case R_Float32:
                result.push_back(patch::toString(*(float *)base));
                break;
            case R_Float64:
                result.push_back(patch::toString(*(double *)base));
                break;
            case R_Float128:
                result.push_back(patch::toString(*(long double *)base));
                break;
            case R_Complex8:
                result.push_back("real : " + patch::toString(*(float *)base));
                result.push_back("imag : " + patch::toString(*(float *)(base + 4)));
                break;
            case R_Complex16:
                result.push_back("real : " + patch::toString(*(double *)base));
                result.push_back("imag : " + patch::toString(*(double *)(base + 8)));
                break;
            case R_Complex32:
                result.push_back("real : " + patch::toString(*(long double *)base));
                result.push_back("imag : " + patch::toString(*(long double *)(base + 16)));
                break;
            case R_Defined:
                cout << "lower :" << lower << "\n";
                cout << "byteSize : " << byteSize() << "\n";
                if(lower >= 0) {
                    switch(byteSize()) {
                        case 1:
                            result.push_back(patch::toString(*(unsigned char *)base));
                            break;
                        case 2:
                            result.push_back(patch::toString(*(unsigned short *)base));
                            break;
                        case 4:
                            result.push_back(patch::toString(*(unsigned int *)base));
                            break;
                        case 8: {
            		        result.push_back(printStringFormat("%llu", *(uint64_t *)base));
                            break;
                        }
                        default:
                            result.push_back("<unknown>");
                            break;
                    }
                } else {
                    switch(byteSize()) {
                        case 1:
                            result.push_back(patch::toString(*(char *)base));
                            break;
                        case 2:
                            result.push_back(patch::toString(*(short *)base));
                            break;
                        case 4:
                            result.push_back(printStringFormat("%ld", *(int *)base));
                            break;
                        case 8: {
            		        result.push_back(printStringFormat("%lld", *(int64_t *)base));
                            break;
                        }
                        default:
                            result.push_back("<unknown>");
                            break;
                    }
                }
                break;
            default:
                result.push_back("<unknown>");
                break;
        }
        return result;
    }
    Type *resolve(int *pointer) { return ref ? ref->resolve(pointer) : this; }
};
class Array : public Type {
public:
    Range *range;
    uint64_t lower, upper;
    Type *type;
public:
    Array(SourceObject *object, astream &str);
    string toString() {
        return "a" + no.toString() + " [over: " + (range ? range->toString() : "<n>") + ";" + patch::toString((int)lower) + "," + patch::toString((int)upper)+ "] of " + (type ? type->toString() : "<n>");
    }
    uint32_t byteSize() {
        return type->byteSize() * (upper - lower);
    }
    vector<string> formatChar(uint32_t base) {
        vector<string> result;
        uint32_t place = lower;
        int left = upper - lower;
        while(left) {
            if(left < 8) {
                string r = printStringFormat("[0x%x] : ", place);
                while(left) {
                    char c[3] = { *(char *)(base + place), 0, 0 };
                    if(c[0] == '\0') { c[0] = '\\'; c[1] = '0'; }
                    string p = printStringFormat(" \'%s\' ", c);
                    r.append(p);
                    left--;
                    place++;
                }
                result.push_back(r);
            } else {
                char *s = (char *)(base + place);
                char c[24] = {*s, 0, 0, *(s+1), 0, 0, *(s+2), 0, 0, *(s+3), 0, 0, *(s+4), 0, 0, *(s+5), 0, 0, *(s+6), 0, 0, *(s+7), 0, 0 };
                for(int i = 0; i < 8; i++) {
                    if(c[i*3] == '\0') { c[i*3] = '\\'; c[i*3+1] = '0'; }
                }
                string r = printStringFormat("[0x%x] : \'%s\' \'%s\' \'%s\' \'%s\' \'%s\' \'%s\' \'%s\' \'%s\'",
                            place, c, c+3, c+6, c+9, c+12, c+15, c+18, c+21);
                result.push_back(r);
                left -= 8;
                place += 8;
            }
        }
        return result;
    }
    vector<string> values(uint32_t base, int generation, int maxGeneration) {
        vector<string> result;

        uint32_t address = base;
        uint32_t place = lower;

        if(!type) {
            result.push_back("<typeless array>");
            return result;
        }
        if(!is_readable_address_st(address)) {
            result.push_back("<no access to array>");
            return result;
        }
        if(generation <= maxGeneration) {
            if(type->byteSize() == 1) return formatChar(base);
            while(place <= upper) {
                vector<string> v = type->values(address, generation+1, maxGeneration);
                if(v.size() == 1) result.push_back(printStringFormat("[0x%x] : %s", place, v[0].c_str()));
                else {
                    result.push_back(printStringFormat("[0x%x] :", place));
                    result.insert(result.end(), v.begin(), v.end());
                }
                address += type->byteSize();
                place++;
            }
        }
        return result;
    }
    Type *resolve(int *pointer) { return this; }
};
class Struct : public Type { //applies to union
public:
    struct Entry {
        string name;
        Type *type;
        uint64_t bitOffset, bitSize;
        Entry(string name, Type *type, uint64_t bitOffset, uint64_t bitSize) {
            this->name = name;
            this->type = type;
            this->bitOffset = bitOffset;
            this->bitSize = bitSize;
        }
        string toString() {
            return name + " : [" + patch::toString((int)bitOffset) + "," + patch::toString((int)bitSize) + "] of " + (type ? type->toString() : "<no type>");
        }
        vector<string> defaultValues(uint32_t address, int generation, int maxGeneration) {
            vector<string> result;
            switch(bitSize) {
                case 8:
                    result.push_back(patch::toString(*(unsigned char *)address));
                        break;
                case 16:
                    result.push_back(patch::toString(*(unsigned short *)address));
                        break;
                case 32:
                    result.push_back(patch::toString(*(unsigned int *)address));
                    break;
                case 64:
                    result.push_back(printStringFormat("%llu", *(uint64_t *)address));
                    break;
                default:
                    result.push_back("<unreadable>");
                    break;
            }
            return result;
        }
    };
    vector<Entry *> entries;
    void addEntry(string name, Type *type, uint64_t bitOffset, uint64_t bitSize) {
        // cout << "struct add entry : " << name << " " << (type != nullptr ? type->no.toString() : string("<void>")) << (type != nullptr ? type->toString() : string("void")) << " " << bitOffset << " " << bitSize << "\n";
        entries.push_back(new Entry(name, type, bitOffset, bitSize));
    }
    uint64_t size;
public:
    Struct(SourceObject *object, astream &str);
    string toString() {
        string result("s" + patch::toString((int)size) + " {\n");
        for(vector<Entry *>::iterator it = entries.begin(); it != entries.end(); it++)
            result += (*it)->toString() + "\n";
        return result + "}";
    }
    uint32_t byteSize() {
        return size;
    }
    vector<string> values(uint32_t base, int generation, int maxGeneration) {
        vector<string> result;
        if(generation <= maxGeneration) {
            for(int i = 0; i < entries.size(); i++) {
                uint32_t offset = entries[i]->bitOffset / 8;
                vector<string> v;
                if(entries[i]->type) v = entries[i]->type->values(base + offset, generation + 1, maxGeneration);
                else v = entries[i]->defaultValues(base + offset, generation + 1, maxGeneration);
                if(v.size() == 1)
                    result.push_back(entries[i]->name + " : " + v[0]);
                else {
                    result.push_back(entries[i]->name + " : {");
                    result.insert(result.end(), v.begin(), v.end());
                    result.push_back("}");
                }
            }
        }
        return result;
    }
    Type *resolve(int *pointer) { return this; }
};
class Enum : public Type {
public:
    struct Entry {
        string name;
        int64_t value;
        Entry(string name, uint64_t value) {
            this->name = name;
            this->value = value;
        }
        string toString() {
            return name + " : " + patch::toString((int)value);
        }
    };
    vector<Entry *> entries;
    void addEntry(string name, uint64_t value) {
        entries.push_back(new Entry(name, value));
    }
public:
    Enum(astream &str)
    : Type(T_Enum, no)
    {
        str.peekSkip('e');
        string name;
        do {
            name = str.get(':');
            bool negative = str.peek() == '-';
            int _value = str.getInt();
            int64_t value = negative ? _value : (unsigned int)_value; 
            str.peekSkip(',');
            addEntry(name, value);
            if(str.peek() == ';') {
                str.skip();
                break;
            }
        } while(name.size());
    }
    string toString() {
        string result("e {\n");
        for(vector<Entry *>::iterator it = entries.begin(); it != entries.end(); it++)
            result += (*it)->toString() + "\n";
        return result + "}";
    }
    uint32_t byteSize() {
        return 4; //is this coherent?
    }
    vector<string> values(uint32_t base, int generation, int maxGeneration) {
        vector<string> result;
        if(is_readable_address_st(base)) {
            int value = *(int *)base;
            for(int i = 0; i < entries.size(); i++)
                if(entries[i]->value == value) {
                    result.push_back(entries[i]->name + "(" + patch::toString(value) + ")");
                    break;
                }
        } else {
            result.push_back("<no access>");
        }
        return result;
    }
    Type *resolve(int *pointer) { return this; }
};
class Pointer : public Type {
public:
    Type *pointsTo;
public:
    Pointer(Type::TypeNo no, Type *type)
    : Type(T_Pointer, no),
    pointsTo(type)
    { }
    // Pointer(SourceObject *object, Type::TypeNo no, astream &str);
    string toString() {
        return "(*) " + (pointsTo ? pointsTo->toString() : "");
    }
    uint32_t byteSize() {
        return sizeof(void*);
    }
    vector<string> values(uint32_t base, int generation, int maxGeneration) {
        vector<string> result;

        uint32_t address = 0x0;
        if(base && is_readable_address_st(base)) { address = *(uint32_t *)base; }
        if(!is_readable_address_st(address)) {
            result.push_back("<no access>");
            return result;
        }
        int pointers = 0;
        if(pointsTo && pointsTo->resolve(&pointers) && pointers == 0) {
            if(pointsTo->resolve(&pointers)->typeClass == T_Range && pointsTo->byteSize() == 1) {
                 // we have a string, perhaps?
                result.push_back(printStringFormat("(char *) (0x%x) \"%s\"", address, address));
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
    Type *resolve(int *pointer) { pointer++; return pointsTo->resolve(pointer); }
};
class ConformantArray : public Type {
private:
    SourceObject *object;
    string name;
public:
    ConformantArray(SourceObject *o, astream &str)
    : Type(T_ConformantArray), object(o)
    {
        str.skip('x');
        char c = str.get();
        switch(c) {
            case 's':
            case 'u':
            case 'e': {
                name = str.get(':');
                char c = str.get();
                break;
            }
            default: break;
        }
    }
    string toString() {
        return "<conformant> " + name + " " + no.toString() + ": "; // + dummy->toString();
    }
    uint32_t byteSize() {
        return 0;
    }
    vector<string> values(uint32_t base, int generation, int maxGeneration); // defined in Binary.cpp
    Type *resolve(int *pointer);
};
class FunctionType : public Type {
private:
    Type *pointsTo;
public:
    FunctionType(Type::TypeNo no)
    : Type(T_Function, no), pointsTo(0)
    { }
    FunctionType(Type::TypeNo no, Type *retValue)
    : Type(T_Function, no)
    {
        pointsTo = retValue;
    }
    // FunctionType(SourceObject *o, astream &str);
    string toString() { return "f" + no.toString() + (pointsTo ? " pointsTo " + pointsTo->toString() : ""); }
    uint32_t byteSize() { return sizeof(void *); }
    vector<string> values(uint32_t base, int generation, int maxGeneration) {
        vector<string> result;
        // if(pointsTo) result = pointsTo->values(base, generation, maxGeneration);
        result.push_back("<function>");
        return result;
    }
    Type *resolve(int *pointer) { return this; /* todo: What about return code? */ }
};
class Symbol {
public:
    typedef enum {
        S_Typedef = 1,
        S_Local,
        S_Param,
        S_Register,
        S_Global,
        S_Function,
        S_Bracket
    } SymType;
    SymType symType;
    string name;
    uint32_t address; //location, depends on symType
    Type *type;
    Symbol(SymType symType, string name, Type *type, uint32_t address = 0) {
        this->symType = symType;
        this->name = name;
        this->type = type;
        this->address = address;
    }
    string typeString() {
        switch(symType) {
            case S_Typedef:
                return "<Typedef>";
            case S_Local:
                return "<Local>";
            case S_Param:
                return "<Param>";
            case S_Register:
                return "<Register>";
            case S_Global:
                return "<Global>";
            case S_Function:
                return "<Function>";
        }
        return "";
    }
    virtual string toString() {
        return name + " " + typeString() + " [addr: " + patch::toString((void *)address) + "] " + (type ? type->toString() : "");
    }
    vector<string> values(uint32_t base) {
        vector<string> result;
        if(symType == S_Typedef) return result;
        vector<string> v;
        int pointers = 0;
        // if(symType == S_Register) {cout << "<register> type == " << type->toString() << type->resolve(&pointers)->toString() << pointers << "\n";
        // cout << "base : " << (void *)base << "\n";
        // if(!is_readable_address_st(base)) cout << "Unreadable.\n"; 
        // if(type) v = type->values(base, 1, 10); } else
        if(type) v = type->values(base + (symType == S_Register ? 0 : address), 1, 10);
        if(v.size() == 1)
            result.push_back(name + " " + typeString() + " : " + v[0]);
        else if(v.size()) {
            result.push_back(name + " " + typeString() + " : {");
            result.insert(result.end(), v.begin(), v.end());
            result.push_back("}");
        }
        return result;
    }
};
class Scope {
public:
    Scope *parent;
    uint64_t begin, end;
    vector<Symbol *> symbols;
    vector<Scope *> children;
    Scope(Scope *parent, uint64_t begin, vector<Symbol *> symbols) {
        this->parent = parent;
        this->begin = begin;
        this->symbols = symbols;
    }
    string toString() {
        string result = "LBRAC [0x" + patch::toString((void *)begin) + "] -- {\n";
        for(vector<Symbol *>::iterator it = symbols.begin(); it != symbols.end(); it++)
            result += (*it)->toString() + "\n";
        for(vector<Scope *>::iterator it = children.begin(); it != children.end(); it++)
            result += (*it)->toString();
        return result + "} RBRAC [0x" + patch::toString((void *)end) + "] --\n";
    }
    Scope *getScope(uint32_t address) {
        for(int i = 0; i < children.size(); i++) {
            Scope *s = children[i];
            if(s->begin <= address && s->end >= address)
                return s->getScope(address);
        }
        return this;
    }
};
class Function : public Symbol {
public:
    struct SLine {
        uint64_t address;
        int line;
        string source;
        SLine(uint64_t address, int line, string source) {
            this->address = address;
            this->line = line;
            this->source = source;
        }
        string toString() {
            return "SLINE [" + patch::toString(line) + "] 0x" + patch::toString((void *)address) + " " + source;
        }
    };
    vector<SLine *> lines;
    void addLine(uint64_t address, int line, string source) {
        lines.push_back(new SLine(address, line, source));
    }
    vector<Symbol *> params;
    vector<Scope *> locals;
    Function(string name, Type *type, uint64_t address)
    : Symbol(S_Function, name, type, address)
    { }
    string toString() {
        string result = name + ": FUN [" + patch::toString((void *)address) + " ] of " + (type ? type->toString() : "<n>") + "\n";
        for(vector<SLine *>::iterator it = lines.begin(); it != lines.end(); it++)
            result += (*it)->toString() + "\n";
        for(vector<Symbol *>::iterator it = params.begin(); it != params.end(); it++)
            result += "PARAM: " + (*it)->toString() + "\n";
        for(vector<Scope *>::iterator it = locals.begin(); it != locals.end(); it++)
            result += (*it)->toString() + "\n";
        return result + "}\n";
    }
};
class Symbol;
class Function;
class SourceObject {
public:
    string name;
    uint64_t start, end;
    vector<Type *> types;
    vector<Symbol *> locals;
    vector<Symbol *> globals;
    vector<Function *> functions;
public:
    Type *findType(Type::TypeNo &no) {
        for(int i = types.size()-1; i >= 0; i--)
            if(types[i]->no.equals(no)
            && types[i]->typeClass != Type::T_ConformantArray
            && types[i]->typeClass != Type::T_Function
            ) {
                return types[i];
            }
        return 0;
    }
    void addType(Type *type) {
        types.push_back(type);
    }
public:
    SourceObject(SymtabEntry **sym, SymtabEntry *stab, const char *stabstr, uint64_t stabsize);
    // Type *interpretType(Type::TypeNo no, astream &str);
    Type *interpretType(astream &str);
    Type *resolveType(Type::TypeNo no, int *pointers);
    Symbol *interpretSymbol(astream &str, uint64_t address, unsigned char type);
    Function *interpretFun(astream &str, uint64_t address);
    void doEXCL(SymtabEntry *_sym, SymtabEntry *stab, const char *stabstr, uint64_t stabsize);
    string toString();
};
class Binary {
public:
    string name;
    SymtabEntry *stab;
    const char *stabstr;
    uint64_t stabsize;
    vector<SourceObject *> objects;
public:
    Binary(string name, SymtabEntry *stab, const char *stabstr, uint64_t stabsize);
    vector<string> getSourceNames();
    uint32_t getLineAddress(string file, int line);
    Function *getFunction(uint32_t address);
    Function::SLine *getLocation(uint32_t address);
    bool isLocation(uint32_t address);
    bool isBinary(uint32_t address);
    bool isFunction(uint32_t address);
    string getFunctionName(uint32_t address);
    string getSourceFile(uint32_t address);
    int getSourceLine(uint32_t address);
    // bool isLastLine(uint32_t address);
    uint32_t getFunctionAddress(string name);
    vector<string> getContext(struct ExceptionContext *eContext, uint32_t ip, uint32_t sp);
    vector<string> getGlobals(ElfSymbols &symbols);
    string toString();
};
#endif //DEFINITIONS_HPP