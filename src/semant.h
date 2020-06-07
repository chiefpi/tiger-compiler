#pragma once
#include "node.h"

Entry *makeFuncEntry(Type, int, ...);

class Semant
{
public:
    Semant()
    {
        initVarEnv();
        initTypeEnv();
    }
    void analyze(NExpr *root)
    {
        root->traverse(VEnv, TEnv);
    }

private:
    VarEnv *VEnv;
    TypeEnv *TEnv;
    void enterScope()
    {
        VEnv->enterScope();
        TEnv->enterScope();
    }
    void quitScope()
    {
        VEnv->quitScope();
        TEnv->quitScope();
    }
    void initTypeEnv();
    void initVarEnv();
};