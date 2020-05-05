#include "SymbolTable.h"

SymbolTable::SymbolTable(){
    this->nameSymbolTable = "main";
    this->type = "void";
}

// holds all the symbols of a method
// has semantic functions
// constructor
SymbolTable::SymbolTable(std::string nameSymbolTable, std::string type, std::string method_type){
    this->nameSymbolTable = nameSymbolTable;
    this->type = type;
    this->method_type = method_type;
    count = 0;

    //size of array is automatically zero (args)
    argsTypeArray.clear();
}

//adds symbol to table
void SymbolTable::addSymbol(Symbol symbol){

    // determine the offset of the symbol
    // increment the offset value
    switch (symbol.kind)
    {
        case Argument:    
            symbol.offset = symOffset_args++;
            break;
        
        case Local:
            symbol.offset = symOffset_var++;
            break;

        case Feild:
            symbol.offset = symOffset_feild++;
            break;

        case Static:
            symbol.offset = symOffset_static++;
            break;
    }

    //push the symbol to the vector
    table.push_back(symbol);
}

// get the type of symbol, used in semantics
std::string SymbolTable::symbolType(std::string symbol){
    std::vector<Symbol>::size_type it;

    //look for the symbol by name
    for(it = 0; it != table.size(); it++){
        if(table[it].symbolName == symbol){
            //return the index of the symbol
            return table[it].type;
        }
    }

    return "null";
}

//checks if symbol exists
int SymbolTable::findSymbol(std::string symbol){
    std::vector<Symbol>::size_type it;

    //look for the symbol by name
    for(it = 0; it != table.size(); it++){
        if(table[it].symbolName == symbol){

            //return the index of the symbol
            return 1;
        }
    }
    
    //if not available, then return -1 (false)
    return -1;
}

//finds symbol and returns offset, used for code generation
int SymbolTable::returnSymbolOffset(std::string c){
    std::vector<Symbol>::size_type it;

    for(it = 0; it != table.size(); it++){
        if(table[it].symbolName == c){
            //return state of symbol
            return table[it].offset;
        }
    }

    return -1;
}

// checks if a symbol has used in a let statements
std::string SymbolTable::isSymbolInitialized(std::string symb){
    std::vector<Symbol>::size_type it;

    //look for the symbol by name
    for(it = 0; it != table.size(); it++){
        if(table[it].symbolName == symb){
            //return state of symbol
            return table[it].isInitialized;
        }
    }

    //if the symbol has not been found, return false
    return "false";
} 

// when used in a let statement set the initialization to true
void SymbolTable::initializeSymbol(std::string symbol){
    std::vector<Symbol>::size_type it;

    //look for the symbol by name
    for(it = 0; it != table.size(); it++){
        if(table[it].symbolName == symbol){
            //return state of symbol
            table[it].isInitialized = "true";
        }
    }
}

// returns the symbol kind (argument, feild etc.)
// used in code generation
std::string SymbolTable::symKind(std::string sym){
    std::vector<Symbol>::size_type it;
    
    for(it = 0; it != table.size(); it++){
        if(table[it].symbolName == sym){

            //get the string value of the kind
            return returnSymbolKind(table[it].kind);
        }
    }

    return "null";
}


//prints the symbol table to command line
/* DEBUGGING FUNCTION */
void SymbolTable::printTable(){
    std::vector<Symbol>::size_type it;
    
    for(it = 0; it != table.size(); it++){
        std::cout << "Symbol Name: " << table[it].symbolName 
        << "\tSymbol kind: " << returnSymbolKind(table[it].kind) << "\tSymbol type: " << table[it].type << " Symbol offset: " << 
        table[it].offset << "\n";
    }

}

// return the symbol kind as a string
const std::string SymbolTable::returnSymbolKind(SymbolKind val){
    return ("%s", SymbolKindString[val].c_str());
}

// clear the argument array 
// used for memory management
void SymbolTable::clearArray(){
    argsTypeArray.clear();
}