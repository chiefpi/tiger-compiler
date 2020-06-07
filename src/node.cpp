#include "node.h"
#include "type.h"

using namespace std;

void NExprList::print(int depth) const
{
    printIndent(depth);
    cout << "<ExprList>" << endl;
    head->print(depth + 1);
    if (next != NULL)
        next->print(depth + 1);
}

// Type NExprList::traverse(VarEnv *VEnv, TypeEnv *TEnv)
// {
//     Type ty = head->traverse(VEnv, TEnv);
//     if (next != NULL)
//         ty = next->traverse(VEnv, TEnv);
//     return ty;
// }

void NDeclList::print(int depth) const
{
    printIndent(depth);
    cout << "<DeclList>" << endl;
    head->print(depth + 1);
    if (next != NULL)
        next->print(depth + 1);
}

// Type NDeclList::traverse(VarEnv* VEnv, TypeEnv* TEnv)
// {
//     Type ty = head->traverse(VEnv, TEnv);
//     if (next != NULL)
//         ty = next->traverse(VEnv, TEnv);
//     return ty;
// }

void NVarList::print(int depth) const
{
    printIndent(depth);
    cout << "<VarList>" << endl;
    head->print(depth + 1);
    if (next != NULL)
        next->print(depth + 1);
}

// Type NVarList::traverse(VarEnv* VEnv, TypeEnv* TEnv)
// {
//     Type ty = head->traverse(VEnv, TEnv);
//     if (next != NULL)
//         ty = next->traverse(VEnv, TEnv);
//     return ty;
// }

void NFieldTypeList::print(int depth) const
{
    printIndent(depth);
    cout << "<FieldTypeList>" << endl;
    id->print(depth + 1);
    type->print(depth + 1);
    if (next != NULL)
        next->print(depth + 1);
}

// Type NFieldTypeList::traverse(VarEnv* VEnv, TypeEnv* TEnv)
// {
//     return Type();
// }

void NFieldExprList::print(int depth) const
{
    printIndent(depth);
    cout << "<FieldExprList>" << endl;
    id->print(depth + 1);
    initValue->print(depth + 1);
    if (next != NULL)
        next->print(depth + 1);
}

// Type NFieldExprList::traverse(VarEnv* VEnv, TypeEnv* TEnv)
// {
//     return Type();
// }

void NStrExpr::print(int depth) const
{
    printIndent(depth);
    cout << "<StrExpr>" << endl;
    printIndent(depth + 1);
    cout << value << endl;
}

// Type NStrExpr::traverse(VarEnv* VEnv, TypeEnv* TEnv)
// {
//     return Type();
// }

void NIntExpr::print(int depth) const
{
    printIndent(depth);
    cout << "<IntExpr>" << endl;
    printIndent(depth + 1);
    cout << value << endl;
}

// Type NIntExpr::traverse(VarEnv* VEnv, TypeEnv* TEnv)
// {
//     return Type();
// }

void NNilExpr::print(int depth) const
{
    printIndent(depth);
    cout << "<NilExpr>" << endl;
    printIndent(depth + 1);
    cout << "nil" << endl;
}

// Type NNilExpr::traverse(VarEnv* VEnv, TypeEnv* TEnv)
// {
//     return Type();
// }

void NVarExpr::print(int depth) const
{
    printIndent(depth);
    cout << "<VarExpr>" << endl;
    //printIndent(depth + 1);
    var->print(depth + 1);
}

// Type NVarExpr::traverse(VarEnv* VEnv, TypeEnv* TEnv)
// {
//     return Type();
// }

void NOpExpr::print(int depth) const
{
    static string op_name[10] =
        {"PLUS", "MINUS", "MUL", "DIV", "EQ", "NE", "LT", "LE", "GT", "GE"};
    printIndent(depth);
    cout << "<OpExpr>" << endl;
    lhs->print(depth + 1);
    printIndent(depth + 1);
    cout << "<op>" << endl;
    printIndent(depth + 2);
    cout << op_name[op] << endl;
    rhs->print(depth + 1);
}

// Type NOpExpr::traverse(VarEnv* VEnv, TypeEnv* TEnv)
// {
//     return Type();
// }

void NAssignExpr::print(int depth) const
{
    printIndent(depth);
    cout << "<AssignExpr>" << endl;
    var->print(depth + 1);
    rhs->print(depth + 1);
}

// Type NAssignExpr::traverse(VarEnv* VEnv, TypeEnv* TEnv)
// {
//     return Type();
// }

void NRecordExpr::print(int depth) const
{
    printIndent(depth);
    cout << "<RecordExpr>" << endl;
    type->print(depth + 1);
    fields->print(depth + 1);
}

// Type NRecordExpr::traverse(VarEnv* VEnv, TypeEnv* TEnv)
// {
//     return Type();
// }

void NArrayExpr::print(int depth) const
{
    printIndent(depth);
    cout << "<ArrayExpr>" << endl;
    type->print(depth + 1);
    size->print(depth + 1);
    initValue->print(depth + 1);
}

// Type NArrayExpr::traverse(VarEnv* VEnv, TypeEnv* TEnv)
// {
//     return Type();
// }

void NCallExpr::print(int depth) const
{
    printIndent(depth);
    cout << "<CallExpr>" << endl;
    func->print(depth + 1);
    if (args != NULL)
        args->print(depth + 1);
}

// Type NCallExpr::traverse(VarEnv* VEnv, TypeEnv* TEnv)
// {
//     return Type();
// }

void NSeqExpr::print(int depth) const
{
    printIndent(depth);
    cout << "<SeqExpr>" << endl;
    exprs->print(depth + 1);
}

// Type NSeqExpr::traverse(VarEnv* VEnv, TypeEnv* TEnv)
// {
//     return Type();
// }

void NIfExpr::print(int depth) const
{
    printIndent(depth);
    cout << "<IfExpr>" << endl;
    test->print(depth + 1);
    thenClause->print(depth + 1);
    if (elseClause != NULL)
        elseClause->print(depth + 1);
}

// Type NIfExpr::traverse(VarEnv* VEnv, TypeEnv* TEnv)
// {
//     return Type();
// }

void NWhileExpr::print(int depth) const
{
    printIndent(depth);
    cout << "<WhileExpr>" << endl;
    test->print(depth + 1);
    body->print(depth + 1);
}

// Type NWhileExpr::traverse(VarEnv* VEnv, TypeEnv* TEnv)
// {
//     return Type();
// }

void NForExpr::print(int depth) const
{
    printIndent(depth);
    cout << "<ForExpr>" << endl;
    id->print(depth + 1);
    high->print(depth + 1);
    body->print(depth + 1);
}

// Type NForExpr::traverse(VarEnv* VEnv, TypeEnv* TEnv)
// {
//     return Type();
// }

void NBreakExpr::print(int depth) const
{
    printIndent(depth);
    cout << "<BreakExpr>" << endl;
}

// Type NBreakExpr::traverse(VarEnv* VEnv, TypeEnv* TEnv)
// {
//     return Type();
// }

void NLetExpr::print(int depth) const
{
    printIndent(depth);
    cout << "<LetExpr>" << endl;
    decls->print(depth + 1);
    body->print(depth + 1);
}

// Type NLetExpr::traverse(VarEnv* VEnv, TypeEnv* TEnv)
// {
//     return Type();
// }

void NFuncDecl::print(int depth) const
{
    printIndent(depth);
    cout << "<FuncDecl>" << endl;
    id->print(depth + 1);
    params->print(depth + 1);
    body->print(depth + 1);
    if (retType != NULL)
        retType->print(depth + 1);
    if (next != NULL)
        next->print(depth + 1);
}

// Type NFuncDecl::traverse(VarEnv* VEnv, TypeEnv* TEnv)
// {
//     return Type();
// }

void NTypeDecl::print(int depth) const
{
    printIndent(depth);
    cout << "<TypeDecl>" << endl;
    id->print(depth + 1);
    type->print(depth + 1);
    if (next != NULL)
        next->print(depth + 1);
}

// Type NTypeDecl::traverse(VarEnv* VEnv, TypeEnv* TEnv)
// {
//     return Type();
// }

void NVarDecl::print(int depth) const
{
    printIndent(depth);
    cout << "<VarDecl>" << endl;
    id->print(depth + 1);
    if (type != NULL)
        type->print(depth + 1);
    initValue->print(depth + 1);
}

// Type NVarDecl::traverse(VarEnv* VEnv, TypeEnv* TEnv)
// {
//     return Type();
// }

void NArrayType::print(int depth) const
{
    printIndent(depth);
    cout << "<ArrayType>" << endl;
    id->print(depth + 1);
}

// Type NArrayType::traverse(VarEnv* VEnv, TypeEnv* TEnv)
// {
//     return Type();
// }

void NRecordType::print(int depth) const
{
    printIndent(depth);
    cout << "<RecordType>" << endl;
    fields->print(depth + 1);
}

void NNameType::print(int depth) const
{
    printIndent(depth);
    cout << "<NameType>" << endl;
    id->print(depth + 1);
}

// Type NNameType::traverse(VarEnv* VEnv, TypeEnv* TEnv)
// {
//     return Type();
// }

void NSimpleVar::print(int depth) const
{
    printIndent(depth);
    cout << "<SimpleVar>" << endl;
    id->print(depth + 1);
}

// Type NSimpleVar::traverse(VarEnv* VEnv, TypeEnv* TEnv)
// {
//     return Type();
// }

void NFieldVar::print(int depth) const
{
    printIndent(depth);
    cout << "<FieldVar>" << endl;
    var->print(depth + 1);
    id->print(depth + 1);
}

// Type NFieldVar::traverse(VarEnv* VEnv, TypeEnv* TEnv)
// {
//     return Type();
// }

void NSubscriptVar::print(int depth) const
{
    printIndent(depth);
    cout << "<SubscriptVar>" << endl;
    var->print(depth + 1);
    sub->print(depth + 1);
}

// Type NSubscriptVar::traverse(VarEnv* VEnv, TypeEnv* TEnv)
// {
//     return Type();
// }
