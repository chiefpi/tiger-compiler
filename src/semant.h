#pragma once
#include "env.h"
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
    void pushFunc(Symbol id, Type *retType, vector<Type *> *paramTypes);
    void pushVar(Symbol id, Type *type);
    void beginLoop();
    void endLoop();
    bool canBreak();
    Type *getActualType(Type *t);
    bool checkTypeEquiv(Type *a, Type *b);
    bool isSameRecord(RecordType *a, RecordType *b);

private:
    VarEnv *VEnv;
    TypeEnv *TEnv;
    int loopCount;
    void initTypeEnv();
    void initVarEnv();
    vector<Type *> *makeParamTypes(Type *a = NULL, Type *b = NULL, Type *c = NULL);
};