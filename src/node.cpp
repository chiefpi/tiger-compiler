#include "node.h"
#include "semant.h"
using namespace std;

void NExprList::print(int depth) const
{
    printIndent(depth);
    cout << "<ExprList>" << endl;
    head->print(depth + 1);
    if (next != NULL)
        next->print(depth + 1);
}

Type NExprList::traverse(Semant *analyzer)
{
    Type ty = head->traverse(analyzer);
    if (next != NULL)
        ty = next->traverse(analyzer);
    return ty;
}

void NDeclList::print(int depth) const
{
    printIndent(depth);
    cout << "<DeclList>" << endl;
    head->print(depth + 1);
    if (next != NULL)
        next->print(depth + 1);
}

Type NDeclList::traverse(Semant *analyzer)
{
    Type ty = head->traverse(analyzer);
    if (next != NULL)
        ty = next->traverse(analyzer);
    return ty;
}

void NVarList::print(int depth) const
{
    printIndent(depth);
    cout << "<VarList>" << endl;
    head->print(depth + 1);
    if (next != NULL)
        next->print(depth + 1);
}

Type NVarList::traverse(Semant *analyzer)
{
    Type ty = head->traverse(analyzer);
    if (next != NULL)
        ty = next->traverse(analyzer);
    return ty;
}

void NFieldTypeList::print(int depth) const
{
    printIndent(depth);
    cout << "<FieldTypeList>" << endl;
    id->print(depth + 1);
    type->print(depth + 1);
    if (next != NULL)
        next->print(depth + 1);
}

Type NFieldTypeList::traverse(Semant *analyzer)
{
    // PASS
    return Type();
}

void NFieldExprList::print(int depth) const
{
    printIndent(depth);
    cout << "<FieldExprList>" << endl;
    id->print(depth + 1);
    initValue->print(depth + 1);
    if (next != NULL)
        next->print(depth + 1);
}

Type NFieldExprList::traverse(Semant *analyzer)
{
    // TODO
    return Type();
}

void NStrExpr::print(int depth) const
{
    printIndent(depth);
    cout << "<StrExpr>" << endl;
    printIndent(depth + 1);
    cout << value << endl;
}

Type NStrExpr::traverse(Semant *analyzer)
{
    return StringType();
}

void NIntExpr::print(int depth) const
{
    printIndent(depth);
    cout << "<IntExpr>" << endl;
    printIndent(depth + 1);
    cout << value << endl;
}

Type NIntExpr::traverse(Semant *analyzer)
{
    return IntType();
}

void NNilExpr::print(int depth) const
{
    printIndent(depth);
    cout << "<NilExpr>" << endl;
    printIndent(depth + 1);
    cout << "nil" << endl;
}

Type NNilExpr::traverse(Semant *analyzer)
{
    return NilType();
}

void NVarExpr::print(int depth) const
{
    printIndent(depth);
    cout << "<VarExpr>" << endl;
    //printIndent(depth + 1);
    var->print(depth + 1);
}

Type NVarExpr::traverse(Semant *analyzer)
{
    // TODO
    return Type();
}

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

Type NOpExpr::traverse(Semant *analyzer)
{
    Type lType = lhs->traverse(analyzer);
    Type rType = rhs->traverse(analyzer);
    switch (op)
    {
    case PLUS:
    case MINUS:
    case MUL:
    case DIV:
        assert(lType.type == Type::TInt && rType.type == Type::TInt,
               "Invalid operand type: int expected");
        break;
    case EQ:
    case NE:
        assert(lType.type != Type::TVoid && rType.type != Type::TVoid,
               "Invalid operand type: unable to compare void");
        assert(!(lType.type == Type::TNil && rType.type != Type::TRecord) && !(lType.type == Type::TRecord && rType.type != Type::TNil),
               "Invalid operand type: unable to compare nil to non-record type");
        assert(checkTypeEquiv(&lType, &rType), "Invalid operand type: unable to compare different types");
        break;
    case LT:
    case LE:
    case GT:
    case GE:
        assert(lType.type == Type::TInt && rType.type == Type::TInt || lType.type == Type::TString && rType.type == Type::TString,
               "Invalid operand type: unable to compare types other then int and string");
        break;
    }
    return IntType();
}

void NAssignExpr::print(int depth) const
{
    printIndent(depth);
    cout << "<AssignExpr>" << endl;
    var->print(depth + 1);
    rhs->print(depth + 1);
}

Type NAssignExpr::traverse(Semant *analyzer)
{
    Type t1 = var->traverse(analyzer);
    Type t2 = rhs->traverse(analyzer);
    assert(!t1.noAssign, "Invalid assignment: annot assign value to loop variables");
    assert(t2.type != Type::TVoid, "Invalid assignment: cannot assign void to variables");
    assert(checkTypeEquiv(&t1, &t2), "Invalid assignment: unmatched type");
    return VoidType();
}

void NRecordExpr::print(int depth) const
{
    printIndent(depth);
    cout << "<RecordExpr>" << endl;
    type->print(depth + 1);
    fields->print(depth + 1);
}

Type NRecordExpr::traverse(Semant *analyzer)
{
    Type *ty = analyzer->findType(*type);
    assert(ty != NULL && ty->type == Type::TRecord, "Invalid Record Expression: Undefined Record Type");
    NFieldExprList *thisField = fields;
    vector<RecordType::Field> vec_fields;
    while (thisField != NULL)
    {
        RecordType::Field field = make_pair(fields->id, &fields->initValue->traverse(analyzer));
        vec_fields.push_back(field);
        thisField = thisField->next;
    }

    vector<RecordType::Field> paramFields = ((RecordType *)ty)->fieldList;
    if (assert(vec_fields.size() == paramFields.size(), "Invalid Record Expression: Mismatched Parameter Number"))
    {
        for (int i = 0; i < vec_fields.size(); i++)
        {
            if (assert(checkTypeEquiv(vec_fields[i].second, paramFields[i].second),
                       "Invalid Record Expression: Mismatched Parameter Type"))
                break;
            if (assert(vec_fields[i].first == paramFields[i].first,
                       "Invalid Record Expression: Mismatched Parameter Name"))
                break;
        }
    };

    return RecordType(vec_fields);
}

void NArrayExpr::print(int depth) const
{
    printIndent(depth);
    cout << "<ArrayExpr>" << endl;
    type->print(depth + 1);
    size->print(depth + 1);
    initValue->print(depth + 1);
}

Type NArrayExpr::traverse(Semant *analyzer)
{
    Type *ty = analyzer->findType(*type);
    assert(ty != NULL && ty->type == Type::TArray, "Invalid Array Expression: Undefined Array Type");
    assert(size->traverse(analyzer).type == Type::TInt, "Invalid Array Expression: Non-Integer Index");
    assert(checkTypeEquiv(ty, &initValue->traverse(analyzer)), "Invalid Array Expression: Unmatched InitValue Type");
    return ArrayType(ty);
}

void NCallExpr::print(int depth) const
{
    printIndent(depth);
    cout << "<CallExpr>" << endl;
    func->print(depth + 1);
    if (args != NULL)
        args->print(depth + 1);
}

Type NCallExpr::traverse(Semant *analyzer)
{
    Entry *ent = analyzer->findEntry(*func);
    assert(ent != NULL && ent->kind == KFunc, "Invalid Function Call: Undefined Function");

    NExprList *thisArg = args;
    vector<Type> vec_argTypes;

    while (thisArg != NULL)
    {
        vec_argTypes.push_back(thisArg->head->traverse(analyzer));
        thisArg = thisArg->next;
    }
    vector<Type> paramTypes = *(ent->paramTypes);
    if (assert(vec_argTypes.size() == paramTypes.size(), "Invalid Function Call: Mismatched Parameter Number"))
    {
        for (int i = 0; i < vec_argTypes.size(); i++)
        {
            if (assert(checkTypeEquiv(&vec_argTypes[i], &paramTypes[i]),
                       "Invalid Function Call: Mismatched Parameter Type"))
                break;
        }
    };
    return ent->type;
}

void NSeqExpr::print(int depth) const
{
    printIndent(depth);
    cout << "<SeqExpr>" << endl;
    exprs->print(depth + 1);
}

Type NSeqExpr::traverse(Semant *analyzer)
{
    return exprs->traverse(analyzer);
}

void NIfExpr::print(int depth) const
{
    printIndent(depth);
    cout << "<IfExpr>" << endl;
    test->print(depth + 1);
    thenClause->print(depth + 1);
    if (elseClause != NULL)
        elseClause->print(depth + 1);
}

Type NIfExpr::traverse(Semant *analyzer)
{
    Type testTy = test->traverse(analyzer);
    assert(testTy.type == Type::TInt, "Invalid If Expression: Non-Integer Test Clause");
    Type thenTy = thenClause->traverse(analyzer);
    if (elseClause == NULL)
        assert(thenTy.type == Type::TVoid, "Invalid If Expression: Value Returned with no Else-Clause");
    else
    {
        Type elseTy = elseClause->traverse(analyzer);
        assert(checkTypeEquiv(&thenTy, &elseTy), "Invalid If Expression: Unmatched Clause Return Type");
    }
    return thenTy;
}

void NWhileExpr::print(int depth) const
{
    printIndent(depth);
    cout << "<WhileExpr>" << endl;
    test->print(depth + 1);
    body->print(depth + 1);
}

Type NWhileExpr::traverse(Semant *analyzer)
{
    Type testTy = test->traverse(analyzer);
    assert(testTy.type == Type::TInt, "Invalid While Expression: Non-Integer Test Clause");
    Type bodyTy = body->traverse(analyzer);
    assert(bodyTy.type == Type::TVoid, "Invalid While Expression: Non-void Value Returned");

    return VoidType();
}

void NForExpr::print(int depth) const
{
    printIndent(depth);
    cout << "<ForExpr>" << endl;
    id->print(depth + 1);
    high->print(depth + 1);
    body->print(depth + 1);
}

Type NForExpr::traverse(Semant *analyzer)
{
    Type idTy = id->traverse(analyzer); // TODO: no-assign flag
    Type highTy = high->traverse(analyzer);
    assert(idTy.type == Type::TInt && highTy.type == Type::TInt, "Invalid For Expression: Non-Integer init/termination value");
    analyzer->beginScope();
    analyzer->beginLoop();
    analyzer->pushVar(id->id->id, idTy);
    Type bodyTy = body->traverse(analyzer);
    analyzer->endScope();
    analyzer->endLoop();
    return VoidType();
}

void NBreakExpr::print(int depth) const
{
    printIndent(depth);
    cout << "<BreakExpr>" << endl;
}

Type NBreakExpr::traverse(Semant *analyzer)
{
    assert(analyzer->canBreak(), "Invalid Break Location: not in loop");
    return VoidType();
}

void NLetExpr::print(int depth) const
{
    printIndent(depth);
    cout << "<LetExpr>" << endl;
    decls->print(depth + 1);
    body->print(depth + 1);
}

Type NLetExpr::traverse(Semant *analyzer)
{
    analyzer->beginScope();
    decls->traverse(analyzer);
    Type bodyTy = body->traverse(analyzer);
    analyzer->endScope();
    return bodyTy;
}

void NFuncDecl::print(int depth) const
{
    printIndent(depth);
    cout << "<FuncDecl>" << endl;
    id->print(depth + 1);
    params->print(depth + 1);
    body->print(depth + 1);
    if (retType != NULL)
        retType->print(depth + 1);
    // if (next != NULL)
    //     next->print(depth + 1);
}

Type NFuncDecl::traverse(Semant *analyzer)
{
    // check redeclaration
    assert(!analyzer->checkFuncRedeclare(*id),
           "Invalid Function Declaration: Function Redeclaration");
    NFieldTypeList *thisParam = params;
    vector<Type> *vec_params = new vector<Type>();
    vector<RecordType::Field> *vec_fields = new vector<RecordType::Field>();

    while (thisParam != NULL)
    {
        Type *typeTy = analyzer->findType(*thisParam->type);
        assert(typeTy != NULL, "Undefined Type in Record");
        RecordType::Field field = make_pair(thisParam->id, typeTy);
        vec_params->push_back(*typeTy);
        vec_fields->push_back(field);
        thisParam = thisParam->next;
    }
    Type retTypeTy = retType->traverse(analyzer);
    analyzer->pushFunc(*id, retTypeTy, vec_params);

    analyzer->beginScope();
    for (RecordType::Field field : *vec_fields)
        analyzer->pushVar(*field.first, *field.second);
    Type realRetTypeTy = body->traverse(analyzer);
    assert(checkTypeEquiv(&realRetTypeTy, &retTypeTy),
           "Invalid Function Declaration: Unmatched Return Value Type");
    analyzer->endScope();

    return VoidType();
}

void NTypeDecl::print(int depth) const
{
    printIndent(depth);
    cout << "<TypeDecl>" << endl;
    id->print(depth + 1);
    type->print(depth + 1);
    // if (next != NULL)
    //     next->print(depth + 1);
}

Type NTypeDecl::traverse(Semant *analyzer)
{
    assert(analyzer->checkTypeRedeclare(*id),
           "Invalid Type Declaration: Type Redeclaration");
    Type *typeTy = &type->traverse(analyzer);
    // TODO: check circular definition
    analyzer->pushType(*id, typeTy);
    if (next != NULL)
        next->traverse(analyzer);
    return VoidType();
}

void NVarDecl::print(int depth) const
{
    printIndent(depth);
    cout << "<VarDecl>" << endl;
    id->print(depth + 1);
    if (type != NULL)
        type->print(depth + 1);
    initValue->print(depth + 1);
}

Type NVarDecl::traverse(Semant *analyzer)
{
    if (!assert(initValue != NULL, "Invalid Variable Declaration: initValue Expected"))
    {
        Type *initValueTy = &initValue->traverse(analyzer);

        if (type != NULL)
        {
            Type *typeTy = &type->traverse(analyzer);
            if (assert(checkTypeEquiv(initValueTy, typeTy),
                       "Invalid Variable Declaration: Unmatched Type"))
            {
                analyzer->pushType(*id, typeTy); // explicit
            };
        }
        else
        {
            if (assert(initValueTy->type != Type::TNil,
                       "Invalid Variable Declaration: Nil Cannot be Assigned to implicit variables"))
            {
                analyzer->pushType(*id, initValueTy); // implicit
            };
        }
    }
    return VoidType();
}

void NArrayType::print(int depth) const
{
    printIndent(depth);
    cout << "<ArrayType>" << endl;
    id->print(depth + 1);
}

Type NArrayType::traverse(Semant *analyzer)
{
    Type *idTy = analyzer->findType(*id);
    assert(idTy != NULL, "Undefined Type");
    return *idTy;
}

void NRecordType::print(int depth) const
{
    printIndent(depth);
    cout << "<RecordType>" << endl;
    fields->print(depth + 1);
}

Type NRecordType::traverse(Semant *analyzer)
{
    NFieldTypeList *thisType = fields;
    vector<RecordType::Field> vec_fields;
    while (thisType != NULL)
    {
        Type *typeTy = analyzer->findType(*thisType->type);
        assert(typeTy != NULL, "Undefined Type in Record");
        RecordType::Field field = make_pair(thisType->id, typeTy);
        vec_fields.push_back(field);
        thisType = thisType->next;
    }
    return RecordType(vec_fields);
}

void NNameType::print(int depth) const
{
    printIndent(depth);
    cout << "<NameType>" << endl;
    id->print(depth + 1);
}

Type NNameType::traverse(Semant *analyzer)
{
    Type *idTy = analyzer->findType(*id);
    assert(idTy != NULL, "Undefined Type");
    return *idTy;
}

void NSimpleVar::print(int depth) const
{
    printIndent(depth);
    cout << "<SimpleVar>" << endl;
    id->print(depth + 1);
}

Type NSimpleVar::traverse(Semant *analyzer)
{
    Entry *varEt = analyzer->findEntry(*id);
    assert(varEt != NULL && varEt->kind == KVar, "Undefined Variable");
    return varEt->type;
}

void NFieldVar::print(int depth) const
{
    printIndent(depth);
    cout << "<FieldVar>" << endl;
    var->print(depth + 1);
    id->print(depth + 1);
}

Type NFieldVar::traverse(Semant *analyzer)
{
    Type varTy = var->traverse(analyzer);
    if (assert(varTy.type == Type::TRecord, "Undefined Variable: Record Expected"))
    {
        assert(((RecordType *)&varTy)->findSymbol(id),
               "Undefined Variable: No Correspondence in Field List");
    }
    return varTy;
}

void NSubscriptVar::print(int depth) const
{
    printIndent(depth);
    cout << "<SubscriptVar>" << endl;
    var->print(depth + 1);
    sub->print(depth + 1);
}

Type NSubscriptVar::traverse(Semant *analyzer)
{
    Type varTy = var->traverse(analyzer);
    if (assert(varTy.type == Type::TArray, "Undefined Variable: Array Expected"))
    {
        Type subTy = sub->traverse(analyzer);
        assert(subTy.type == Type::TInt, "Invalid Subscript: Int Expected");
    };
    return varTy;
}
