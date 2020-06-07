#pragma once
#include <iostream>
#include <unordered_map>
#include <string>

using namespace std;

class Symbol
{
private:
    string id = nullptr;

public:
    Symbol() : id(nullptr) {}
    Symbol(string name) : id(name)
    {
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
    bool operator==(const Symbol &other) const
    {
        return !id.compare(other.id);
    }
    struct Hasher
    {
        size_t operator()(const Symbol &s) const
        {
            size_t res = 17;
            res = res * 31 + hash<string>()(s.id);
            return res;
        }
    };
};