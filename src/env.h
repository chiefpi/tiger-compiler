#pragma once
#include "symbolTable.h"
#include "type.h"
#include <vector>
typedef enum Kind
{
    KVar,
    KFunc
} Kind;

class Entry
{
public:
    Kind kind;
    Type type;
    vector<Type> *paramTypes;
    Entry(Kind kind, Type type, vector<Type> *pts = NULL) : kind(kind), type(type), paramTypes(pts) {}
};

typedef SymbolTable<Entry> VarEnv;
typedef SymbolTable<Type> TypeEnv;