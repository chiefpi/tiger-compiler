#include "node.h"
#include "semant.h"
using namespace std;

int printCount = 0;

void NExprList::print(int depth) const
{
    printIndent(depth);
    // cout << "node" << printCount++ << "["
    //      << "label = \"ExprList\"];" << endl;
    cout << "<ExprList>" << endl;
    if (head != NULL)
        head->print(depth + 1);
    if (next != NULL)
        next->print(depth + 1);
}
void NDeclList::print(int depth) const
{
    printIndent(depth);
    // cout << "node" << printCount++ << "["
    //      << "label = \"DeclList\"];" << endl;
    cout << "<DeclList>" << endl;
    if (head != NULL)
    {
        head->print(depth + 1);
    }
    if (next != NULL)
        next->print(depth + 1);
}
void NVarList::print(int depth) const
{
    printIndent(depth);
    // cout << "node" << printCount++ << "["
    //      << "label = \"VarList\"];" << endl;

    cout << "<VarList>" << endl;
    if (head != NULL)
    {
        head->print(depth + 1);
    }
    if (next != NULL)
        next->print(depth + 1);
}
void NFieldTypeList::print(int depth) const
{
    printIndent(depth);
    // cout << "node" << printCount++ << "["
    //      << "label = \"FieldTypeList\"];" << endl;

    cout << "<FieldTypeList>" << endl;
    id->print(depth + 1);
    type->print(depth + 1);
    if (next != NULL)
        next->print(depth + 1);
}
void NFieldExprList::print(int depth) const
{
    printIndent(depth);
    // cout << "node" << printCount++ << "["
    //      << "label = \"FieldExprList\"];" << endl;

    cout << "<FieldExprList>" << endl;
    id->print(depth + 1);
    initValue->print(depth + 1);
    if (next != NULL)
        next->print(depth + 1);
}
void NStrExpr::print(int depth) const
{
    printIndent(depth);
    // cout << "node" << printCount++ << "["
    //      << "label = \"StrExpr\"];" << endl;
    cout << "<StrExpr>" << endl;
    printIndent(depth + 1);
    // cout << "node" << printCount++ << "["
    //      << "label = \"" << value << "\"];" << endl;
    cout << value << endl;
}
void NIntExpr::print(int depth) const
{
    printIndent(depth);
    // cout << "node" << printCount++ << "["
    //      << "label = \"IntExpr\"];" << endl;
    cout << "<IntExpr>" << endl;
    printIndent(depth + 1);
    // cout << "node" << printCount++ << "["
    //      << "label = \"" << value << "\"];" << endl;
    cout << value << endl;
}
void NNilExpr::print(int depth) const
{
    printIndent(depth);
    // cout << "node" << printCount++ << "["
    //      << "label = \"NilExpr\"];" << endl;
    cout << "<NilExpr>" << endl;
    printIndent(depth + 1);
    // cout << "node" << printCount++ << "["
    //      << "label = \"nil\"];" << endl;
    cout << "nil" << endl;
}
void NVarExpr::print(int depth) const
{
    printIndent(depth);
    // cout << "node" << printCount++ << "["
    //      << "label = \"VarExpr\"];" << endl;
    cout << "<VarExpr>" << endl;
    var->print(depth + 1);
}
void NOpExpr::print(int depth) const
{
    static string op_name[12] =
        {"PLUS", "MINUS", "MUL", "DIV", "EQ", "NE", "LT", "LE", "GT", "GE", "AND",
         "OR"};
    printIndent(depth);
    // cout << "node" << printCount++ << "["
    //      << "label = \"OpExpr\"];" << endl;
    cout << "<OpExpr>" << endl;
    if (lhs != NULL)
        lhs->print(depth + 1);
    printIndent(depth + 1);
    // cout << "node" << printCount++ << "["
    //      << "label = \"op\"];" << endl;
    cout << "<op>" << endl;
    printIndent(depth + 2);
    // cout << "node" << printCount++ << "["
    //      << "label = \"" << op_name[op] << "\"];" << endl;
    cout << op_name[op] << endl;
    rhs->print(depth + 1);
}
void NAssignExpr::print(int depth) const
{
    printIndent(depth);
    // cout << "node" << printCount++ << "["
    //      << "label = \"AssignExpr\"];" << endl;
    cout << "<AssignExpr>" << endl;
    var->print(depth + 1);
    rhs->print(depth + 1);
}
void NRecordExpr::print(int depth) const
{
    printIndent(depth);
    // cout << "node" << printCount++ << "["
    //      << "label = \"RecordExpr\"];" << endl;
    cout << "<RecordExpr>" << endl;
    type->print(depth + 1);
    fields->print(depth + 1);
}
void NArrayExpr::print(int depth) const
{
    printIndent(depth);
    // cout << "node" << printCount++ << "["
    //      << "label = \"ArrayExpr\"];" << endl;
    cout << "<ArrayExpr>" << endl;
    type->print(depth + 1);
    size->print(depth + 1);
    initValue->print(depth + 1);
}
void NCallExpr::print(int depth) const
{
    printIndent(depth);
    // cout << "node" << printCount++ << "["
    //      << "label = \"CallExpr\"];" << endl;
    cout << "<CallExpr>" << endl;
    func->print(depth + 1);
    if (args != NULL)
        args->print(depth + 1);
}
void NSeqExpr::print(int depth) const
{
    printIndent(depth);
    // cout << "node" << printCount++ << "["
    //      << "label = \"SeqExpr\"];" << endl;
    cout << "<SeqExpr>" << endl;
    exprs->print(depth + 1);
}
void NIfExpr::print(int depth) const
{
    printIndent(depth);
    // cout << "node" << printCount++ << "["
    //      << "label = \"IfExpr\"];" << endl;
    cout << "<IfExpr>" << endl;
    test->print(depth + 1);
    thenClause->print(depth + 1);
    if (elseClause != NULL)
        elseClause->print(depth + 1);
}
void NWhileExpr::print(int depth) const
{
    printIndent(depth);
    // cout << "node" << printCount++ << "["
    //      << "label = \"WhileExpr\"];" << endl;
    cout << "<WhileExpr>" << endl;
    test->print(depth + 1);
    body->print(depth + 1);
}
void NForExpr::print(int depth) const
{
    printIndent(depth);
    // cout << "node" << printCount++ << "["
    //      << "label = \"ForExpr\"];" << endl;
    cout << "<ForExpr>" << endl;
    id->print(depth + 1);
    high->print(depth + 1);
    body->print(depth + 1);
}
void NBreakExpr::print(int depth) const
{
    printIndent(depth);
    // cout << "node" << printCount++ << "["
    //      << "label = \"BreakExpr\"];" << endl;
    cout << "<BreakExpr>" << endl;
}
void NLetExpr::print(int depth) const
{
    printIndent(depth);
    // cout << "node" << printCount++ << "["
    //      << "label = \"LetExpr\"];" << endl;
    cout << "<LetExpr>" << endl;
    decls->print(depth + 1);
    body->print(depth + 1);
}
void NFuncDecl::print(int depth) const
{
    printIndent(depth);
    // cout << "node" << printCount++ << "["
    //      << "label = \"FuncDecl\"];" << endl;
    cout << "<FuncDecl>" << endl;
    if (id != NULL)
    {
        id->print(depth + 1);
    }
    if (params != NULL)
    {
        params->print(depth + 1);
    }
    if (body != NULL)
    {
        body->print(depth + 1);
    }
    if (retType != NULL)
        retType->print(depth + 1);
    if (next != NULL)
        next->print(depth + 1);
}
void NTypeDecl::print(int depth) const
{
    printIndent(depth);
    // cout << "node" << printCount++ << "["
    //      << "label = \"TypeDecl\"];" << endl;
    cout << "<TypeDecl>" << endl;
    id->print(depth + 1);
    type->print(depth + 1);
    if (next != NULL)
        next->print(depth + 1);
}
void NVarDecl::print(int depth) const
{
    printIndent(depth);
    // cout << "node" << printCount++ << "["
    //      << "label = \"VarDecl\"];" << endl;
    cout << "<VarDecl>" << endl;
    id->print(depth + 1);
    if (type != NULL)
        type->print(depth + 1);
    initValue->print(depth + 1);
}
void NArrayType::print(int depth) const
{
    printIndent(depth);
    // cout << "node" << printCount++ << "["
    //      << "label = \"ArrayType\"];" << endl;
    cout << "<ArrayType>" << endl;
    id->print(depth + 1);
}
void NRecordType::print(int depth) const
{
    printIndent(depth);
    // cout << "node" << printCount++ << "["
    //      << "label = \"RecordType\"];" << endl;
    cout << "<RecordType>" << endl;
    fields->print(depth + 1);
}
void NNameType::print(int depth) const
{
    printIndent(depth);
    // cout << "node" << printCount++ << "["
    //      << "label = \"NameType\"];" << endl;
    cout << "<NameType>" << endl;
    id->print(depth + 1);
}
void NSimpleVar::print(int depth) const
{
    printIndent(depth);
    // cout << "node" << printCount++ << "["
    //      << "label = \"SimpleVar\"];" << endl;
    cout << "<SimpleVar>" << endl;
    id->print(depth + 1);
}
void NFieldVar::print(int depth) const
{
    printIndent(depth);
    // cout << "node" << printCount++ << "["
    //      << "label = \"FieldVar\"];" << endl;
    cout << "<FieldVar>" << endl;
    var->print(depth + 1);
    id->print(depth + 1);
}
void NSubscriptVar::print(int depth) const
{
    printIndent(depth);
    // cout << "node" << printCount++ << "["
    //      << "label = \"SubscriptVar\"];" << endl;
    cout << "<SubscriptVar>" << endl;
    var->print(depth + 1);
    sub->print(depth + 1);
}
