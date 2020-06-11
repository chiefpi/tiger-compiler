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

Type *NExprList::traverse(Semant *analyzer)
{
    Type *ty = head->traverse(analyzer);
    if (next != NULL)
        ty = next->traverse(analyzer);
    return ty;
}

Type *NDeclList::traverse(Semant *analyzer)
{
    Type *ty = head->traverse(analyzer);
    if (next != NULL)
        ty = next->traverse(analyzer);
    return ty;
}

Type *NVarList::traverse(Semant *analyzer)
{
    Type *ty = head->traverse(analyzer);
    if (next != NULL)
        ty = next->traverse(analyzer);
    return ty;
}

Type *NFieldTypeList::traverse(Semant *analyzer)
{
    // PASS
    return NULL;
}

Type *NFieldExprList::traverse(Semant *analyzer)
{
    // PASS
    return NULL;
}

Type *NStrExpr::traverse(Semant *analyzer)
{
    return new StringType();
}

Type *NIntExpr::traverse(Semant *analyzer)
{
    return new IntType();
}

Type *NNilExpr::traverse(Semant *analyzer)
{
    return new NilType();
}

Type *NVarExpr::traverse(Semant *analyzer)
{
    return var->traverse(analyzer);
}

Type *NOpExpr::traverse(Semant *analyzer)
{
    Type *lType;
    if (lhs != NULL)
        lType = lhs->traverse(analyzer);
    Type *rType = rhs->traverse(analyzer);
    if (lhs == NULL)
    {
        assertpred(op == MINUS, "Invalid operation expression: empty lhs");
        assertpred(rType->type == Type::TInt, "Invalid Negative Number Format");
        return new IntType();
    }
    switch (op)
    {
    case PLUS:
    case MINUS:
    case MUL:
    case DIV:
        assertpred(lType->type == Type::TInt && rType->type == Type::TInt,
                   "Invalid operand type: int expected");
        break;
    case EQ:
    case NE:
        assertpred(lType->type != Type::TVoid && rType->type != Type::TVoid,
                   "Invalid operand type: unable to compare void");
        if (lType->type == Type::TNil)
        {
            assertpred(rType->type == Type::TRecord,
                       "Invalid operand type: unable to compare nil to non-record type");
        }
        if (rType->type == Type::TNil)
        {
            assertpred(lType->type == Type::TRecord,
                       "Invalid operand type: unable to compare nil to non-record type");
        }
        assertpred(analyzer->checkTypeEquiv(lType, rType), "Invalid operand type: unable to compare different types");
        break;
    case LT:
    case LE:
    case GT:
    case GE:
        assertpred((lType->type == Type::TInt && rType->type == Type::TInt) || (lType->type == Type::TString && rType->type == Type::TString),
                   "Invalid operand type: unable to compare types other then int and string");
        break;
    }
    return new IntType();
}

Type *NAssignExpr::traverse(Semant *analyzer)
{
    Type *t1 = var->traverse(analyzer);
    Type *t2 = rhs->traverse(analyzer);
    //assertpred(!t1->noAssign, "Invalid assignment: annot assign value to loop variables");
    assertpred(t2->type != Type::TVoid, "Invalid assignment: cannot assign void to variables");
    assertpred(analyzer->checkTypeEquiv(t1, t2), "Invalid assignment: unmatched type");
    return new VoidType();
}

Type *NRecordExpr::traverse(Semant *analyzer)
{
    Type *ty = analyzer->findType(*type);
    assertpred(ty != NULL && ty->type == Type::TRecord, "Invalid Record Expression: Undefined Record Type");
    NFieldExprList *thisField = fields;
    vector<RecordType::Field> vec_fields;
    while (thisField != NULL)
    {
        Type *thisTy = thisField->initValue->traverse(analyzer);
        RecordType::Field field = make_pair(thisField->id, thisTy);
        vec_fields.push_back(field);
        thisField = thisField->next;
    }

    vector<RecordType::Field> paramFields = ((RecordType *)ty)->fieldList;
    if (assertpred(vec_fields.size() == paramFields.size(), "Invalid Record Expression: Mismatched Parameter Number"))
    {
        for (size_t i = 0; i < vec_fields.size(); i++)
        {
            if (assertpred(analyzer->checkTypeEquiv(vec_fields[i].second, paramFields[i].second),
                           "Invalid Record Expression: Mismatched Parameter Type"))
                break;
            if (assertpred(vec_fields[i].first == paramFields[i].first,
                           "Invalid Record Expression: Mismatched Parameter Name"))
                break;
        }
    };

    return new RecordType(vec_fields);
}

Type *NArrayExpr::traverse(Semant *analyzer)
{
    Type *ty = analyzer->findType(*type);
    assertpred(ty != NULL && ty->type == Type::TArray, "Invalid Array Expression: Undefined Array Type");
    assertpred(size->traverse(analyzer)->type == Type::TInt, "Invalid Array Expression: Non-Integer Index");
    ArrayType *arrTy = (ArrayType *)ty;
    Type *initValueTy = initValue->traverse(analyzer);
    assertpred(analyzer->checkTypeEquiv(arrTy->elementType, initValueTy), "Invalid Array Expression: Unmatched InitValue Type");
    return new ArrayType(initValueTy);
}

Type *NCallExpr::traverse(Semant *analyzer)
{
    Entry *ent = analyzer->findEntry(*func);
    assertpred(ent != NULL && ent->kind == KFunc, "Invalid Function Call: Undefined Function");

    NExprList *thisArg = args;
    vector<Type *> vec_argTypes;

    while (thisArg != NULL)
    {
        vec_argTypes.push_back(thisArg->head->traverse(analyzer));
        thisArg = thisArg->next;
    }
    vector<Type *> paramTypes = *(ent->paramTypes);
    if (assertpred(vec_argTypes.size() == paramTypes.size(), "Invalid Function Call: Mismatched Parameter Number"))
    {
        for (size_t i = 0; i < vec_argTypes.size(); i++)
        {
            if (assertpred(analyzer->checkTypeEquiv(vec_argTypes[i], paramTypes[i]),
                           "Invalid Function Call: Mismatched Parameter Type"))
                break;
        }
    };
    return ent->type;
}

Type *NSeqExpr::traverse(Semant *analyzer)
{
    return exprs->traverse(analyzer);
}

Type *NIfExpr::traverse(Semant *analyzer)
{
    Type *testTy = test->traverse(analyzer);
    assertpred(testTy->type == Type::TInt, "Invalid If Expression: Non-Integer Test Clause");
    Type *thenTy = thenClause->traverse(analyzer);
    if (elseClause == NULL)
        assertpred(thenTy->type == Type::TVoid, "Invalid If Expression: Value Returned with no Else-Clause");
    else
    {
        Type *elseTy = elseClause->traverse(analyzer);
        assertpred(analyzer->checkTypeEquiv(thenTy, elseTy), "Invalid If Expression: Unmatched Clause Return Type");
    }
    return thenTy;
}

Type *NWhileExpr::traverse(Semant *analyzer)
{
    Type *testTy = test->traverse(analyzer);
    assertpred(testTy->type == Type::TInt, "Invalid While Expression: Non-Integer Test Clause");
    Type *bodyTy = body->traverse(analyzer);
    assertpred(bodyTy->type == Type::TVoid, "Invalid While Expression: Non-void Value Returned");

    return new VoidType();
}

Type *NForExpr::traverse(Semant *analyzer)
{
    Type *idTy = id->traverse(analyzer); // TODO: no-assign flag
    Type *highTy = high->traverse(analyzer);
    assertpred(idTy->type == Type::TInt && highTy->type == Type::TInt, "Invalid For Expression: Non-Integer init/termination value");
    analyzer->beginScope();
    analyzer->beginLoop();
    analyzer->pushVar(id->id->id, idTy);
    body->traverse(analyzer);
    analyzer->endScope();
    analyzer->endLoop();
    return new VoidType();
}

Type *NBreakExpr::traverse(Semant *analyzer)
{
    assertpred(analyzer->canBreak(), "Invalid Break Location: not in loop");
    return new VoidType();
}

Type *NLetExpr::traverse(Semant *analyzer)
{
    analyzer->beginScope();
    decls->traverse(analyzer);
    Type *bodyTy = body->traverse(analyzer);
    analyzer->endScope();
    return bodyTy;
}

Type *NFuncDecl::traverse(Semant *analyzer)
{
    // parse the function declaration, put all funcnames in the funclist
    //             to the VEnv before any of the function's body is parsed
    vector<RecordType::Field> *first_vec_fields;
    Type *first_retTypeTy;
    int count = 0;
    for (NFuncDecl *thisFunc = this; thisFunc != NULL; thisFunc = thisFunc->next)
    {
        assertpred(!analyzer->checkFuncRedeclare(*(thisFunc->id)),
                   "Invalid Function Declaration: Function Redeclaration");
        NFieldTypeList *thisParam = thisFunc->params;
        vector<Type *> *vec_params = new vector<Type *>();
        vector<RecordType::Field> *vec_fields = new vector<RecordType::Field>();

        while (thisParam != NULL)
        {
            Type *typeTy = analyzer->findType(*thisParam->type);
            assertpred(typeTy != NULL, "Undefined Type in Record");
            RecordType::Field field = make_pair(thisParam->id, typeTy);
            vec_params->push_back(typeTy);
            vec_fields->push_back(field);
            thisParam = thisParam->next;
        }
        Type *retTypeTy = new VoidType();
        if (thisFunc->retType != NULL)
        {
            retTypeTy = thisFunc->retType->traverse(analyzer);
            retTypeTy = analyzer->getActualType(retTypeTy);
        }
        analyzer->pushFunc(*(thisFunc->id), retTypeTy, vec_params);
        if (count == 0)
        {
            first_retTypeTy = retTypeTy;
            first_vec_fields = vec_fields;
        }
        ++count;
    }

    // parse the function body
    analyzer->beginScope();
    for (RecordType::Field field : *first_vec_fields)
        analyzer->pushVar(*field.first, field.second);
    Type *realRetTypeTy = body->traverse(analyzer);
    assertpred(analyzer->checkTypeEquiv(realRetTypeTy, first_retTypeTy),
               "Invalid Function Declaration: Unmatched Return Value Type");
    analyzer->endScope();

    if (next != NULL)
        next->traverse(analyzer, true);

    return new VoidType();
}

Type *NFuncDecl::traverse(Semant *analyzer, bool notHead)
{
    NFieldTypeList *thisParam = params;
    vector<RecordType::Field> *vec_fields = new vector<RecordType::Field>();

    while (thisParam != NULL)
    {
        Type *typeTy = analyzer->findType(*thisParam->type);
        assertpred(typeTy != NULL, "Undefined Type in Record");
        RecordType::Field field = make_pair(thisParam->id, typeTy);
        vec_fields->push_back(field);
        thisParam = thisParam->next;
    }
    Type *retTypeTy = new VoidType();
    if (retType != NULL)
    {
        retTypeTy = retType->traverse(analyzer);
        retTypeTy = analyzer->getActualType(retTypeTy);
    }

    // not the first in the funclist, parse body only
    analyzer->beginScope();
    for (RecordType::Field field : *vec_fields)
        analyzer->pushVar(*field.first, field.second);
    Type *realRetTypeTy = body->traverse(analyzer);
    assertpred(analyzer->checkTypeEquiv(realRetTypeTy, retTypeTy),
               "Invalid Function Declaration: Unmatched Return Value Type");
    analyzer->endScope();

    if (next != NULL)
        next->traverse(analyzer, true);

    return new VoidType();
}

Type *NTypeDecl::traverse(Semant *analyzer)
{
    // deal with recursive type declaration
    for (NTypeDecl *thisType = this; thisType != NULL; thisType = thisType->next)
    {
        assertpred(!analyzer->checkTypeRedeclare(*(thisType->id)),
                   "Invalid Type Declaration: Type Redeclaration");
        analyzer->pushType(*(thisType->id), new NameType(*(thisType->id)));
    }
    // TODO: check circular definition
    Type *typeTy = type->traverse(analyzer);
    Type *actualType = analyzer->getActualType(typeTy);
    if (actualType->type == Type::TRecord)
    {
        for (RecordType::Field &field : ((RecordType *)actualType)->fieldList)
        {
            if (field.second->type == Type::TName)
                field.second = analyzer->getActualType(field.second);
        }
    }
    analyzer->pushType(*id, actualType);
    if (next != NULL)
        next->traverse(analyzer, true);
    return new VoidType();
}

Type *NTypeDecl::traverse(Semant *analyzer, bool notHead)
{
    Type *typeTy = type->traverse(analyzer);
    Type *actualType = analyzer->getActualType(typeTy);
    if (actualType->type == Type::TRecord)
    {
        for (RecordType::Field &field : ((RecordType *)actualType)->fieldList)
        {
            if (field.second->type == Type::TName)
                field.second = analyzer->getActualType(field.second);
        }
    }
    analyzer->pushType(*id, actualType);
    if (next != NULL)
        next->traverse(analyzer, true);
    return new VoidType();
}

Type *NVarDecl::traverse(Semant *analyzer)
{
    if (assertpred(initValue != NULL, "Invalid Variable Declaration: initValue Expected"))
    {

        Type *initValueTy = initValue->traverse(analyzer);

        if (type != NULL)
        {
            Type *typeTy = type->traverse(analyzer);
            typeTy = analyzer->getActualType(typeTy);
            if (assertpred(analyzer->checkTypeEquiv(initValueTy, typeTy),
                           "Invalid Variable Declaration: Unmatched Type"))
            {
                analyzer->pushVar(*id, typeTy); // explicit
                return typeTy;
            };
        }
        else
        {
            if (assertpred(initValueTy->type != Type::TNil,
                           "Invalid Variable Declaration: Nil Cannot be Assigned to implicit variables"))
            {
                analyzer->pushVar(*id, initValueTy); // implicit
                return initValueTy;
            };
        }
    }
    return new VoidType();
}

Type *NArrayType::traverse(Semant *analyzer)
{
    Type *idTy = analyzer->findType(*id);
    assertpred(idTy != NULL, "Undefined Type");
    return new ArrayType(idTy);
}

Type *NRecordType::traverse(Semant *analyzer)
{
    NFieldTypeList *thisType = fields;
    vector<RecordType::Field> vec_fields;
    while (thisType != NULL)
    {
        Type *typeTy = analyzer->findType(*thisType->type);
        assertpred(typeTy != NULL, "Undefined Type in Record");
        RecordType::Field field = make_pair(thisType->id, typeTy);
        vec_fields.push_back(field);
        thisType = thisType->next;
    }
    return new RecordType(vec_fields);
}

Type *NNameType::traverse(Semant *analyzer)
{
    Type *idTy = analyzer->findType(*id);
    assertpred(idTy != NULL, "Undefined Type");
    return new NameType(*id, idTy);
}

Type *NSimpleVar::traverse(Semant *analyzer)
{
    Entry *varEt = analyzer->findEntry(*id);
    assertpred(varEt != NULL && varEt->kind == KVar, "Undefined Variable");
    return varEt->type;
}

Type *NFieldVar::traverse(Semant *analyzer)
{
    Type *varTy = var->traverse(analyzer);
    Type *fieldTy = NULL;
    if (assertpred(varTy->type == Type::TRecord, "Undefined Variable: Record Expected"))
    {
        fieldTy = ((RecordType *)varTy)->findSymbolType(id);
        assertpred(fieldTy != NULL,
                   "Undefined Variable: No Correspondence in Field List");
    }
    return fieldTy;
}

Type *NSubscriptVar::traverse(Semant *analyzer)
{
    Type *varTy = var->traverse(analyzer);
    if (assertpred(varTy->type == Type::TArray, "Undefined Variable: Array Expected"))
    {
        Type *subTy = sub->traverse(analyzer);
        assertpred(subTy->type == Type::TInt, "Invalid Subscript: Int Expected");
    };
    return varTy;
}
