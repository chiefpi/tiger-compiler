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
    Type *type;
    vector<Type *> *paramTypes;
    Entry(Kind kind, Type *type, vector<Type *> *pts = NULL) : kind(kind), type(type), paramTypes(pts) {}
};

typedef SymbolTable<Entry> VarEnv;
typedef SymbolTable<Type> TypeEnv;

VarEnv initVarEnv()
{
    // add built-in functions
    VarEnv venv;
    venv.push(Symbol("print"), &Entry(KFunc, NULL, &vector<Type *>{&StringType()}));
    venv.push(Symbol("printi"), &Entry(KFunc, NULL, &vector<Type *>{&IntType()}));
    venv.push(Symbol("flush"), &Entry(KFunc, NULL, NULL));
    venv.push(Symbol("getchar"), &Entry(KFunc, &StringType(), NULL));
    venv.push(Symbol("ord"), &Entry(KFunc, &IntType(), &vector<Type *>{&StringType()}));
    venv.push(Symbol("chr"), &Entry(KFunc, &StringType(), &vector<Type *>{&IntType()}));
    venv.push(Symbol("size"), &Entry(KFunc, &IntType(), &vector<Type *>{&StringType()}));
    venv.push(Symbol("substring"), &Entry(KFunc, &StringType(), &vector<Type *>{&StringType(), &IntType(), &IntType()}));
    venv.push(Symbol("concat"), &Entry(KFunc, &StringType(), &vector<Type *>{&StringType(), &StringType()}));
    venv.push(Symbol("not"), &Entry(KFunc, &IntType(), &vector<Type *>{&IntType()}));
    venv.push(Symbol("exit"), &Entry(KFunc, NULL, &vector<Type *>{&IntType()}));
    return venv;
};

TypeEnv initTypeEnv()
{
    TypeEnv tenv;
    tenv.push(Symbol("int"), &IntType());
    tenv.push(Symbol("string"), &StringType());
    return tenv;
};