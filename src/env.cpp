#include "env.h"
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

VarEnv initVarEnv()
{
    // add built-in functions
    VarEnv venv;
    venv.push(Symbol("print"), makeFuncEntry(VoidType(), 1, StringType()));
    venv.push(Symbol("printi"), makeFuncEntry(VoidType(), 1, IntType()));
    venv.push(Symbol("flush"), makeFuncEntry(VoidType()));
    venv.push(Symbol("getchar"), makeFuncEntry(StringType()));
    venv.push(Symbol("ord"), makeFuncEntry(IntType(), 1, StringType()));
    venv.push(Symbol("chr"), makeFuncEntry(StringType(), 1, IntType()));
    venv.push(Symbol("size"), makeFuncEntry(IntType(), 1, StringType()));
    venv.push(Symbol("substring"), makeFuncEntry(StringType(), 3, StringType(), IntType(), IntType()));
    venv.push(Symbol("concat"), makeFuncEntry(StringType(), 2, StringType(), StringType()));
    venv.push(Symbol("not"), makeFuncEntry(IntType(), 1, IntType()));
    venv.push(Symbol("exit"), makeFuncEntry(VoidType(), 1, IntType()));
    return venv;
}

TypeEnv initTypeEnv()
{
    TypeEnv tenv;
    tenv.push(Symbol("int"), new IntType());
    tenv.push(Symbol("string"), new StringType());
    return tenv;
}
