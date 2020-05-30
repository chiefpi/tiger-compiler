#include <iostream>
#include <unordered_map>
using namespace std;

class Symbol
{
private:
    string id;
    // int scope;
    static unordered_map<string, Symbol> dict;

public:
    Symbol() {}
    Symbol(string s) : id(s) {}
    Symbol symbol(string s)
    {
        Symbol sym;
        if (dict.find(s) != dict.end())
        {
            sym = dict[s];
        }
        else
            sym = Symbol(s);
        dict.insert({s, sym});
        return sym;
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
