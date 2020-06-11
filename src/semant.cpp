#include "semant.h"
#include <cstdarg>
void Semant::initVarEnv()
{
    VEnv = new VarEnv();
    pushFunc(Symbol("print"), new VoidType(), makeParamTypes(new StringType()));
    pushFunc(Symbol("printi"), new VoidType(), makeParamTypes(new IntType()));
    pushFunc(Symbol("flush"), new VoidType(), makeParamTypes());
    pushFunc(Symbol("getchar"), new StringType(), makeParamTypes());
    pushFunc(Symbol("ord"), new IntType(), makeParamTypes(new StringType()));
    pushFunc(Symbol("chr"), new StringType(), makeParamTypes(new IntType()));
    pushFunc(Symbol("size"), new IntType(), makeParamTypes(new StringType()));
    pushFunc(Symbol("substring"), new StringType(), makeParamTypes(new StringType(), new IntType(), new IntType()));
    pushFunc(Symbol("concat"), new StringType(), makeParamTypes(new StringType(), new StringType()));
    pushFunc(Symbol("not"), new IntType(), makeParamTypes(new IntType()));
    pushFunc(Symbol("exit"), new VoidType(), makeParamTypes(new IntType()));
}

vector<Type *> *Semant::makeParamTypes(Type *a, Type *b, Type *c)
{
    vector<Type *> *vec_paramTypes = new vector<Type *>;
    if (a != NULL)
        vec_paramTypes->push_back(a);
    if (b != NULL)
        vec_paramTypes->push_back(b);
    if (c != NULL)
        vec_paramTypes->push_back(c);
    return vec_paramTypes;
}

void Semant::initTypeEnv()
{
    TEnv = new TypeEnv();
    TEnv->push(Symbol("int"), new IntType());
    TEnv->push(Symbol("string"), new StringType());
}

Semant::Semant()
{
    loopCount = 0;
    initVarEnv();
    initTypeEnv();
}
void Semant::beginScope()
{
    VEnv->enterScope();
    TEnv->enterScope();
}
void Semant::endScope()
{
    VEnv->quitScope();
    TEnv->quitScope();
}
bool Semant::checkTypeRedeclare(Symbol id)
{
    Type *typeTy = TEnv->findFront(id);
    return (typeTy != NULL);
}
bool Semant::checkFuncRedeclare(Symbol id)
{
    Entry *funcEt = VEnv->findFront(id);
    return ((funcEt != NULL && funcEt->kind == KFunc) || VEnv->findBack(id));
}
Type *Semant::findType(Symbol id)
{
    return TEnv->findAll(id);
}
Entry *Semant::findEntry(Symbol id)
{
    return VEnv->findAll(id);
}
void Semant::pushType(Symbol id, Type *type)
{
    TEnv->push(id, type);
}
void Semant::pushFunc(Symbol id, Type *retType, vector<Type *> *paramTypes)
{
    Entry *ent = new Entry(KFunc, retType, paramTypes);
    VEnv->push(id, ent);
}
void Semant::pushVar(Symbol id, Type *type)
{
    Entry *ent = new Entry(KVar, type);
    VEnv->push(id, ent);
}
void Semant::beginLoop()
{
    ++loopCount;
}
void Semant::endLoop()
{
    --loopCount;
}
bool Semant::canBreak()
{
    return loopCount != 0;
}

Type *Semant::getActualType(Type *qt)
{
    if (qt->type == Type::TName)
    {
        NameType *nameTy = (NameType *)qt;
        return findType(nameTy->name);
    }
    else
        return qt;
}
bool Semant::isSameRecord(RecordType *a, RecordType *b)
{

    if (a->fieldList.size() != b->fieldList.size())
        return false;
    if (a->fieldList == b->fieldList)
        return true;
    for (int i = 0; i < a->fieldList.size(); ++i)
    {
        Symbol *aName = a->fieldList[i].first;
        Symbol *bName = b->fieldList[i].first;

        if (!(*aName == *bName))
            return false;

        Type *aType = a->fieldList[i].second;
        Type *bType = b->fieldList[i].second;

        if (aType->type == Type::TName)
            aType = getActualType(aType);
        if (bType->type == Type::TName)
            bType = getActualType(bType);

        if (!checkTypeEquiv(aType, bType))
            return false;
    }
    return true;
}

bool Semant::checkTypeEquiv(Type *a, Type *b)
{
    if (a == b)
        return true;
    if (a->type == Type::TName)
        a = getActualType(a);
    if (b->type == Type::TName)
        b = getActualType(b);

    if (a->type == Type::TArray)
        a = ((ArrayType *)a)->elementType;
    if (b->type == Type::TArray)
        b = ((ArrayType *)b)->elementType;

    if (a->type == Type::TNil && b->type == Type::TRecord ||
        b->type == Type::TNil && a->type == Type::TRecord)
        return true;

    if (a->type == b->type)
    {
        if (a->type == Type::TRecord)
        {
            if (isSameRecord((RecordType *)a, (RecordType *)b))
                return true;
            else
                return false;
        }

        else if (a->type == Type::TArray)
        {
            if (checkTypeEquiv(((ArrayType *)a)->elementType, ((ArrayType *)b)->elementType))
                return true;
            else
                return false;
        }

        // else if (a->type == Type::TName)
        // {
        //     if (checkTypeEquiv(((NameType *)a)->trueType, ((NameType *)b)->trueType))
        //         return true;
        //     else
        //         return false;
        // }
        else
            return true;
    }

    if (a->CoerceTo(b))
        return true;
    return false;
}