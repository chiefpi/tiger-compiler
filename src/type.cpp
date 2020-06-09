
#include "type.h"

bool checkTypeEquiv(Type *a, Type *b)
{
    if (a->type == b->type)
    {
        if (a->type == Type::TRecord)
        {
            if (((RecordType *)a)
                    ->isSame((RecordType *)b))
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

        else if (a->type == Type::TName)
        {
            if (checkTypeEquiv(((NameType *)a)->trueType, ((NameType *)b)->trueType))
                return true;
            else
                return false;
        }
        else
            return true;
    }

    if (a->CoerceTo(b))
        return true;
    return false;
}

bool RecordType::isSame(RecordType *rt)
{
    return rt->fieldList == fieldList; // TODO: compare one by one
}

bool RecordType::checkField(vector<Type *> pts)
{
    if (pts.size() != fieldList.size())
        return false;
    for (size_t i = 0; i < pts.size(); i++)
    {
        if (!checkTypeEquiv(pts[i], fieldList[i].second))
            return false;
    }
    return true;
}

bool RecordType::findSymbol(Symbol *sym)
{
    for (Field field : fieldList)
    {
        if (*field.first == *sym)
            return true;
    }
    return false;
}