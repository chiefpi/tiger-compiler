#pragma once

#include "symbol.h"
#include <memory>
#include <deque>

using namespace std;

template <typename T>
class SymbolTable
{
    typedef unordered_map<Symbol, T *> TableType;

public:
    SymbolTable();
    ~SymbolTable();
    T *findAll(Symbol const id);
    T *find(Symbol const id);
    void push(Symbol const id, T* const value);
    void pop(Symbol const id);
    void enterScope();
    void exitScope();
    void resetScope();

protected:
    deque<TableType> stack;
};

template <typename T>
SymbolTable<T>::SymbolTable()
{
    enter();
}

template <typename T>
SymbolTable<T>::~SymbolTable()
{
    exit();
}

template <typename T>
T *SymbolTable<T>::findAll(const Symbol id)
{
    for (auto s : stack)
        if (s[id])
            return s[id];
    return stack.front()[id];
}

template <typename T>
T *SymbolTable<T>::find(const Symbol id)
{
    if (stack.front()[id])
        return stack.front()[id];
    return stack.front()[id];
}

template <typename T>
void SymbolTable<T>::push(Symbol const id, T* const value)
{
    stack.front()[id] = value;
}

template <typename T>
void SymbolTable<T>::pop(Symbol const id)
{
    stack.front().erase(id);
}

template <typename T>
void SymbolTable<T>::enterScope()
{
    stack.push_front(TableType());
}

template <typename T>
void SymbolTable<T>::exitScope()
{
    stack.pop_front();
}

template <typename T>
void SymbolTable<T>::resetScope()
{
    stack.clear();
    enter();
}