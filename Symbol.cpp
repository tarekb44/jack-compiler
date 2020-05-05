#include <iostream>
#include "Symbol.h"

//initializer
Symbol::Symbol(){
    this->type = "null";
    this->kind = SymbolKind::Local;
    this->symbolName = "this";
    this->offset = 0;
}

//Object used to fill a symbol table
Symbol::Symbol(std::string type, SymbolKind kind, std::string symbolName, int offset, std::string isInitialized){
    //type of the symbol (int, String etc.)
    this->type = type;

    //kind of symbol (argument, var etc.)
    this->kind = kind;
    
    //name of symbol
    this->symbolName = symbolName;

    //position in the symbol table
    this->offset = offset;

    //is it initialized (in a let statement etc.)
    this->isInitialized = isInitialized;
}

    


