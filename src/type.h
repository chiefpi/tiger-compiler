#pragma once
#include "symbol.h"
#include <vector>

class Type
{
public:
    enum TypeKind
    {
        TInt,
        TString,
        TRecord,
        TArray,
        TNil,
        TVoid,
        TName
    };
    TypeKind type;
    bool noAssign;
    Type() {}
    Type(TypeKind type) : type(type){};
    Type(const Type &obj) { type = obj.type; }
    bool CoerceTo(Type *t)
    {
        return false; // TODO: check coerce
    };
};

class ArrayType : public Type
{
public:
    Type *elementType;
    ArrayType(Type *et) : elementType(et) { type = TArray; }
};

bool checkTypeEquiv(Type *a, Type *b);

class RecordType : public Type
{
public:
    typedef pair<Symbol *, Type *> Field;
    vector<Field> fieldList;

    RecordType(const vector<Field> &fields) : fieldList(fields) { type = TRecord; }
    bool isSame(RecordType *rt);
    bool checkField(vector<Type *> pts);
    bool findSymbol(Symbol *sym);
};

class NameType : public Type
{
public:
    Symbol name;
    Type *trueType;
    NameType(Symbol name, Type *tt = NULL) : name(name), trueType(tt) { type = TName; }
};

class IntType : public Type
{
public:
    IntType() { type = TInt; }
};

class StringType : public Type
{
public:
    StringType() { type = TString; }
};

class NilType : public Type
{
public:
    NilType() { type = TNil; }
};

class VoidType : public Type
{
public:
    VoidType() { type = TVoid; }
};
