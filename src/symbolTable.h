#pragma once

#include "symbol.h"
#include <memory>
#include <deque>

using namespace std;

template <typename T>
class SymbolTable
{
    typedef unordered_map<Symbol, T *, Symbol::Hasher> TableType;

public:
    SymbolTable();
    ~SymbolTable();
    T *findAll(Symbol id);
    T *findFront(Symbol id);
    T *findBack(Symbol id);
    void push(Symbol id, T *value);
    void pop(Symbol id);
    void enterScope();
    void quitScope();
    void resetScope();
    size_t countScopes();

protected:
    deque<TableType> stack;
};

template <typename T>
SymbolTable<T>::SymbolTable()
{
    enterScope();
}

template <typename T>
SymbolTable<T>::~SymbolTable()
{
    quitScope();
}

template <typename T>
T *SymbolTable<T>::findAll(Symbol id)
{
    for (auto s : stack)
        if (s.find(id) != s.end())
            return s[id];
    return NULL;
}

template <typename T>
T *SymbolTable<T>::findFront(Symbol id)
{
    if (stack.front().find(id) != stack.front().end())
        return stack.front()[id];
    return NULL;
}

template <typename T>
T *SymbolTable<T>::findBack(Symbol id)
{
    if (stack.back().find(id) != stack.back().end())
        return stack.back()[id];
    return NULL;
}

template <typename T>
void SymbolTable<T>::push(Symbol id, T *value)
{
    auto it = stack.front().find(id);
    if (it != stack.front().end())
        it->second = value;
    else
        stack.front().insert(make_pair(id, value));
    // stack.front()[id] = value;
}

template <typename T>
void SymbolTable<T>::pop(Symbol id)
{
    stack.front().erase(id);
}

template <typename T>
void SymbolTable<T>::enterScope()
{
    stack.push_front(TableType());
}

template <typename T>
void SymbolTable<T>::quitScope()
{
    stack.pop_front();
}

template <typename T>
void SymbolTable<T>::resetScope()
{
    stack.clear();
    enterScope();
}

template <typename T>
size_t SymbolTable<T>::countScopes()
{
    return stack.size();
}