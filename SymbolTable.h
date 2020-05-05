#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include "Symbol.h"
#include "Token.h"
#include "Logger.h"

#include <vector>
#include <string>
#include <iostream>

class SymbolTable {

    public:
    
        //give the name of the symbol table
        SymbolTable();
        SymbolTable(std::string nameSymbolTable, std::string type, std::string method_type);

        //store a vector of symbols 
        std::vector<Symbol> table;

        //store the number of arguments (paramlist operation)
        std::vector<std::string> argsTypeArray;

        //check whether symbol exists or not
        int findSymbol(std::string symbol);

        // returns the type of symbol
        std::string symbolType(std::string symbol);
        
        //add a symbol 
        //name, kind, type
        void addSymbol(Symbol symbol);

        //print the table when requested
        void printTable();

        // checks if the symbol is initialized
        std::string isSymbolInitialized(std::string symbol);

        // initialize the symbol
        void initializeSymbol(std::string symbol);

        // return the kind of the symbol table
        const std::string returnSymbolKind(SymbolKind val);

        void clearArray();

        // return the offset of symbol
        int returnSymbolOffset(std::string sym_name); 
       
        int functionOffset; // the number of the function

        int count; //number of symbol tables

        // method offset
        int symOffset_args = 0;
        int symOffset_var = 0;

        // class offsets
        int symOffset_feild = 0;
        int symOffset_static = 0;

        std::string parentClass; // class that the method is in

        // return kind of symbol
        std::string symKind(std::string sym_name);

        std::string nameSymbolTable; // name of the method
  
        std::string type; // type of the function

        std::string method_type; // kind of the function
        
        bool returnStatement = false; // initially, false, true when return statement is found

};


#endif
