#include "symbolTable.h"
#pragma once
#include "type.h"
#include "env.h"
#include "node.h"

class Semant
{
public:
    Semant()
    {
        VEnv = initVarEnv();
        TEnv = initTypeEnv();
    }
    void analyze(NExpr *root)
    {
        root->traverse(&VEnv, &TEnv);
    }

private:
    static VarEnv VEnv;
    static TypeEnv TEnv;
    void enterScope()
    {
        VEnv.enterScope();
        TEnv.enterScope();
    }
    void quitScope()
    {
        VEnv.quitScope();
        TEnv.quitScope();
    }
};