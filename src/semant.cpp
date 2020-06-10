#include "semant.h"
#include <cstdarg>
void Semant::initVarEnv()
{
    VEnv = new VarEnv();
    pushFunc(Symbol("print"), VoidType(), makeParamTypes(new StringType()));
    pushFunc(Symbol("printi"), VoidType(), makeParamTypes(new IntType()));
    pushFunc(Symbol("flush"), VoidType(), makeParamTypes());
    pushFunc(Symbol("getchar"), StringType(), makeParamTypes());
    pushFunc(Symbol("ord"), IntType(), makeParamTypes(new StringType()));
    pushFunc(Symbol("chr"), StringType(), makeParamTypes(new IntType()));
    pushFunc(Symbol("size"), IntType(), makeParamTypes(new StringType()));
    pushFunc(Symbol("substring"), StringType(), makeParamTypes(new StringType(), new IntType(), new IntType()));
    pushFunc(Symbol("concat"), StringType(), makeParamTypes(new StringType(), new StringType()));
    pushFunc(Symbol("not"), IntType(), makeParamTypes(new IntType()));
    pushFunc(Symbol("exit"), VoidType(), makeParamTypes(new IntType()));
}

vector<Type> *Semant::makeParamTypes(Type *a, Type *b, Type *c)
{
    vector<Type> *vec_paramTypes = new vector<Type>;
    if (a != NULL)
        vec_paramTypes->push_back(*a);
    if (b != NULL)
        vec_paramTypes->push_back(*b);
    if (c != NULL)
        vec_paramTypes->push_back(*c);
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
void Semant::pushFunc(Symbol id, Type retType, vector<Type> *paramTypes)
{
    Entry *ent = new Entry(KFunc, retType, paramTypes);
    VEnv->push(id, ent);
}
void Semant::pushVar(Symbol id, Type type)
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
