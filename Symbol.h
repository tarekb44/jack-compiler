#ifndef SYMBOL_H
#define SYMBOL_H
#include <string>
#include "Token.h"

//each symbol has a symbol kind
enum SymbolKind{    
    //used in code generation
    Static, //class
    Feild, //class
    Argument, //method
    Local, //method
    Constant // describe integer constants, string constants etc.
};

//returns the type as a string value
const std::string SymbolKindString[5] = {"static", "feild" , "arg" ,
"local", "constant"};

class Symbol {
    public:
    //      NAME      |     Type      |       Kind        |         #           |
    
    //constructor
    Symbol();
    Symbol(std::string type, SymbolKind kind, std::string name, int offset, std::string isInitialized);

    //to store the type 
    std::string type;

    //symbol kind
    SymbolKind kind;

    //to store the name
    std::string symbolName;

    //running index of the symbol
    int offset;

    std::string isInitialized;

};

#endif
