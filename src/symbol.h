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
};
