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

class RecordType : public Type
{
public:
    typedef pair<Symbol *, Type *> Field;
    vector<Field> fieldList;

    RecordType(const vector<Field> &fields) : fieldList(fields) { type = TRecord; }
    bool isSame(RecordType *rt)
    {
        return rt->fieldList == fieldList; // TODO: compare one by one
    }
    bool checkField(vector<Type *> pts)
    {
        if (pts.size() != fieldList.size())
            return false;
        for (int i = 0; i < pts.size(); i++)
        {
            if (!checkTypeEquiv(pts[i], fieldList[i].second))
                return false;
        }
        return true;
    }
    bool findSymbol(Symbol *sym)
    {
        for (Field field : fieldList)
        {
            if (*field.first == *sym)
                return true;
        }
        return false;
    }
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

bool checkTypeEquiv(Type *a, Type *b)
{
    if (a->type == b->type)
    {
        if (a->type == Type::TRecord)
        {
            if (((RecordType *)a)
                    ->isSame((RecordType *)b))
                return true;
            else
                return false;
        }

        else if (a->type == Type::TArray)
        {
            if (checkTypeEquiv(((ArrayType *)a)->elementType, ((ArrayType *)b)->elementType))
                return true;
            else
                return false;
        }

        else if (a->type == Type::TName)
        {
            if (checkTypeEquiv(((NameType *)a)->trueType, ((NameType *)b)->trueType))
                return true;
            else
                return false;
        }
        else
            return true;
    }

    if (a->CoerceTo(b))
        return true;
    return false;
}