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
    T *find(Symbol id);
    void push(Symbol id, T *value);
    void pop(Symbol id);
    void enterScope();
    void quitScope();
    void resetScope();

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
        if (s[id])
            return s[id];
    return stack.front()[id];
}

template <typename T>
T *SymbolTable<T>::find(Symbol id)
{
    if (stack.front()[id])
        return stack.front()[id];
    return stack.front()[id];
}

template <typename T>
void SymbolTable<T>::push(Symbol id, T *value)
{
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