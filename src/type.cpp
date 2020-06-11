
#include "type.h"

// bool RecordType::isSame(RecordType *rt)
// {
//     if (rt->fieldList.size() != fieldList.size())
//         return false;
//     for (int i = 0; i < fieldList.size(); ++i)
//     {
//         Symbol * thisName = fieldList[i].first;
//         Symbol * thatName = rt->fieldList[i].first;
//         if (!(*thisName == *thatName)) return false;
//         Type * thisTy = fieldList[i].second;
//         Type * thatTy = rt->fieldList[i].second;
//         if (!)
//     }
//     return rt->fieldList == fieldList; // TODO: compare one by one
// }

// bool RecordType::checkField(vector<Type *> pts)
// {
//     if (pts.size() != fieldList.size())
//         return false;
//     for (size_t i = 0; i < pts.size(); i++)
//     {
//         if (!checkTypeEquiv(pts[i], fieldList[i].second))
//             return false;
//     }
//     return true;
// }

Type * RecordType::findSymbolType(Symbol *sym)
{
    for (Field field : fieldList)
    {
        if (*field.first == *sym)
            return field.second;
    }
    return NULL;
}