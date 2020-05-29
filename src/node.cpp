#include "node.h"

using namespace std;

void NExprList::print(int depth) const
{
    printIndent(depth);
    cout << "<ExprList>" << endl;
    head.print(depth + 1);
    if (next != NULL)
        next.print(depth + 1);
}

void NDeclList::print(int depth) const
{
    printIndent(depth);
    cout << "<DeclList>" << endl;
    head.print(depth + 1);
    if (next != NULL)
        next.print(depth + 1);
}

void NVarList::print(int depth) const
{
    printIndent(depth);
    cout << "<VarList>" << endl;
    head.print(depth + 1);
    if (next != NULL)
        next.print(depth + 1);
}

void NFieldTypeList::print(int depth) const
{
    printIndent(depth);
    cout << "<FieldTypeList>" << endl;
    id.print(depth + 1);
    type.print(depth + 1);
    if (next != NULL)
        next.print(depth + 1);
}

void NFieldExprList::print(int depth) const
{
    printIndent(depth);
    cout << "<FieldExprList>" << endl;
    id.print(depth + 1);
    initValue.print(depth + 1);
    if (next != NULL)
        next.print(depth + 1);
}

void NStrExpr::print(int depth) const
{
    printIndent(depth);
    cout << "<StrExpr>" << endl;
    printIndent(depth + 1);
    cout << value << endl;
}

void NIntExpr::print(int depth) const
{
    printIndent(depth);
    cout << "<IntExpr>" << endl;
    printIndent(depth + 1);
    cout << value << endl;
}

void NNilExpr::print(int depth) const
{
    printIndent(depth);
    cout << "<NilExpr>" << endl;
    printIndent(depth + 1);
    cout << "nil" << endl;
}

void NOpExpr::print(int depth) const
{
    static string op_name[10] =
        {"PLUS", "MINUS", "MUL", "DIV", "EQ", "NE", "LT", "LE", "GT", "GE"};
    printIndent(depth);
    cout << "<OpExpr>" << endl;
    lhs.print(depth + 1);
    printIndent(depth + 1);
    cout << op_name[op] << endl;
    rhs.print(depth + 1);
}

void NAssignExpr::print(int depth) const
{
    printIndent(depth);
    cout << "<AssignExpr>" << endl;
    var.print(depth + 1);
    rhs.print(depth + 1);
}

void NRecordExpr::print(int depth) const
{
    printIndent(depth);
    cout << "<RecordExpr>" << endl;
    type.print(depth + 1);
    fields.print(depth + 1);
}

void NArrayExpr::print(int depth) const
{
    printIndent(depth);
    cout << "<ArrayExpr>" << endl;
    type.print(depth + 1);
    size.print(depth + 1);
    initValue.print(depth + 1);
}

void NCallExpr::print(int depth) const
{
    printIndent(depth);
    cout << "<CallExpr>" << endl;
    func.print(depth + 1);
    if (args != NULL)
        args.print(depth + 1);
}

void NSeqExpr::print(int depth) const
{
    printIndent(depth);
    cout << "<SeqExpr>" << endl;
    exprs.print(depth + 1);
}

void NIfExpr::print(int depth) const
{
    printIndent(depth);
    cout << "<IfExpr>" << endl;
    test.print(depth + 1);
    thenClause.print(depth + 1);
    if (elseClause != NULL)
        elseClause.print(depth + 1);
}

void NWhileExpr::print(int depth) const
{
    printIndent(depth);
    cout << "<WhileExpr>" << endl;
    test.print(depth + 1);
    body.print(depth + 1);
}

void NForExpr::print(int depth) const
{
    printIndent(depth);
    cout << "<ForExpr>" << endl;
    id.print(depth + 1);
    high.print(depth + 1);
    body.print(depth + 1);
}

void NBreakExpr::print(int depth) const
{
    printIndent(depth);
    cout << "<BreakExpr>" << endl;
}

void NLetExpr::print(int depth) const
{
    printIndent(depth);
    cout << "<Letxpr>" << endl;
    decls.print(depth + 1);
    body.print(depth + 1);
}

void NFuncDecl::print(int depth) const
{
    printIndent(depth);
    cout << "<FuncDecl>" << endl;
    type.print(depth + 1);
    params.print(depth + 1);
    body.print(depth + 1);
    if (retType != NULL)
        retType.print(depth + 1);
    if (next != NULL)
        next.print(depth + 1);
}

void NTypeDecl::print(int depth) const
{
    printIndent(depth);
    cout << "<TypeDecl>" << endl;
    id.print(depth + 1);
    type.print(depth + 1);
    if (next != NULL)
        next.print(depth + 1);
}

void NVarDecl::print(int depth) const
{
    printIndent(depth);
    cout << "<VarDecl>" << endl;
    id.print(depth + 1);
    if (type != NULL)
        type.print(depth + 1);
    initValue.print(depth + 1);
}

void NArrayType::print(int depth) const
{
    printIndent(depth);
    cout << "<ArrayType>" << endl;
    id.print(depth + 1);
}

void NRecordType::print(int depth) const
{
    printIndent(depth);
    cout << "<RecordType>" << endl;
    fields.print(depth + 1);
}

void NNameType::print(int depth) const
{
    printIndent(depth);
    cout << "<NameType>" << endl;
    id.print(depth + 1);
}

void NSimpleVar::print(int depth) const
{
    printIndent(depth);
    cout << "<SimpleVar>" << endl;
    id.print(depth + 1);
}

void NFieldVar::print(int depth) const
{
    printIndent(depth);
    cout << "<FieldVar>" << endl;
    var.print(depth + 1);
    id.print(depth + 1);
}

void NSubscriptVar::print(int depth) const
{
    printIndent(depth);
    cout << "<SubscriptVar>" << endl;
    var.print(depth + 1);
    index.print(depth + 1);
}