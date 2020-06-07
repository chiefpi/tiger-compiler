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
    Type() {}
    Type(TypeKind type) : type(type){};
    bool CoerceTo(Type *t);
};

class ArrayType : public Type
{
public:
    Type *elementType;
    ArrayType(Type *et) : elementType(et) { type = TArray; }
};

class RecordType : public Type
{
public:
    typedef pair<Symbol, Type *> Field;
    vector<Field> fieldList;

    RecordType(const vector<Field> &fields) : fieldList(fields) { type = TRecord; }
    Type *getType(Symbol name);
    int getSize();
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
