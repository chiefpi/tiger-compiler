#include "symbolTable.h"
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
        checkExpr(root);
    }
    Type *checkExpr(NExpr *node);
    Type *checkDecl(NExpr *node);
    Type *checkVar(NExpr *node);
    Type *checkType(NExpr *node);

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
}