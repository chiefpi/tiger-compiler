#pragma once
#include "node.h"

Entry *makeFuncEntry(Type, int, ...);

class Semant
{
public:
    Semant();
    void beginScope();
    void endScope();
    bool checkTypeRedeclare(Symbol id);
    bool checkFuncRedeclare(Symbol id);
    Type *findType(Symbol id);
    Entry *findEntry(Symbol id);
    void pushType(Symbol id, Type *type);
    void pushFunc(Symbol id, Type retType, vector<Type> *paramTypes);
    void pushVar(Symbol id, Type type);
    void beginLoop();
    void endLoop();
    bool canBreak();

private:
    VarEnv *VEnv;
    TypeEnv *TEnv;
    static int loopCount;
    void initTypeEnv();
    void initVarEnv();
    vector<Type> *makeParamTypes(Type *, Type *, Type *);
};