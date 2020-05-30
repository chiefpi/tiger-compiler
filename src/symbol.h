#pragma once

#include <iostream>
#include <unordered_map>
#include <string>

using namespace std;

class Symbol
{
private:
    string id;

public:
    Symbol() {}
    Symbol(string s) : id(s) {}
    Symbol symbol(string s)
    {
        return Symbol(s);
    }
    void print(int depth) const
    {
        for (int i = 0; i < depth; i++)
            cout << "	";
        cout << "<id>" << endl;
        for (int i = 0; i < depth + 1; i++)
            cout << "	";
        cout << id << endl;
    }
};