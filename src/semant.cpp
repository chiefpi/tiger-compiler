#include "semant.h"
#include <cstdarg>

Entry *makeFuncEntry(Type retType, int count = 0, ...)
{
    vector<Type> *params = new vector<Type>();
    va_list args;
    va_start(args, count);
    for (int i = 0; i < count; i++)
        params->push_back(va_arg(args, Type));

    return new Entry(KFunc, retType, params);
}

bool isEquivType(Type* a, Type* b);


void Semant::initVarEnv()
{
    VEnv = new VarEnv();
    VEnv->push(Symbol("print"), makeFuncEntry(VoidType(), 1, StringType()));
    VEnv->push(Symbol("printi"), makeFuncEntry(VoidType(), 1, IntType()));
    VEnv->push(Symbol("flush"), makeFuncEntry(VoidType()));
    VEnv->push(Symbol("getchar"), makeFuncEntry(StringType()));
    VEnv->push(Symbol("ord"), makeFuncEntry(IntType(), 1, StringType()));
    VEnv->push(Symbol("chr"), makeFuncEntry(StringType(), 1, IntType()));
    VEnv->push(Symbol("size"), makeFuncEntry(IntType(), 1, StringType()));
    VEnv->push(Symbol("substring"), makeFuncEntry(StringType(), 3, StringType(), IntType(), IntType()));
    VEnv->push(Symbol("concat"), makeFuncEntry(StringType(), 2, StringType(), StringType()));
    VEnv->push(Symbol("not"), makeFuncEntry(IntType(), 1, IntType()));
    VEnv->push(Symbol("exit"), makeFuncEntry(VoidType(), 1, IntType()));
}

void Semant::initTypeEnv()
{
    TEnv = new TypeEnv();
    TEnv->push(Symbol("int"), new IntType());
    TEnv->push(Symbol("string"), new StringType());
}
