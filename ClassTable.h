#ifndef CLASSTABLE_H
#define CLASSTABLE_H

#include "SymbolTable.h"
#include "Symbol.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>

// this class holds all the symbol tables within a .jack file
// it also has semantic functions that check for functionality relative to 
// the current class
class ClassTable {

    public:
        struct MethodName 
        {
        std::string method_type; // (constructor, method, function)
        std::string function_type; // int, boolean etc.
        };

    public:

        // each class table object has a name
        ClassTable();
        ClassTable(std::string classname);

        //name of the class-scope symbol table
        std::string className;

        //make a vector of pointers
        std::vector<SymbolTable *> classtables;

        //search for a symbol table within a class
        //returns the index of the ST in vector
        int findSymbolTable(std::string table);

        // search for a symbol and return the index within the class tables array
        int searchForSymbol(std::string symbol, std::string scope_name);
        
        //adds a symbol table to the array of symbol tables
        void addSymbolTable(SymbolTable *name);

        //check if all the expression lists matches with the subroutines' arguments list
        void expressionListValid();
        
        // returns the type and kinf of function
        MethodName symbolTable(std::string st_name);

        //this function determines if a symbol has been initialized
        std::string initSymbol(std::string symbol, std::string scope);

        //this function initializes the symbols (in let statements etc.)
        void symbolInitialize(std::string symbol, std::string scope);

        // return the symbol kind of symbol
        std::string symtable_kind(std::string name, std::string method);
        
        //create a symbol table for the method scope within the current class table
        //this first symbol will always be
        void createMethodSymbolTable(std::string st_name, std::string functionType, std::string m_type, std::string parentClass, int offset);

        // return the function type of function (int, boolean etc.)
        std::string returnFunctionType(std::string st_name);
        
        // returns the function type (constructor, method, function)
        // useful for code generation purposes
        std::string methodType(std::string st_name);
        
        //access a symbol table within a scope using name
        void accessSymbolTable(std::string name, Symbol symbol);

        // return the symbol type by searching for the symbol within a scope
        std::string returnSymbolType(std::string symbolName, std::string scope);

        // add the argument types in an order to a symbol table, later used to 
        // check that expression lists are valid
        void addArgsSymbolTable(std::string args_type, std::string scope_name);

        //all methods need a return statement, this function ensures this
        void methodHasReturnStatement(std::string sym_name);

        // tells whether a function has a return statement or not
        bool hasReturnStatement(std::string sym_name);
        
        bool isJackOSAPI; // used to determine whether the current class is a JACK OS API

        // prints all the tables
        /* DEBUGGING */
        void printTables();

        // delete the tables from a class table
        void deleteObjs();

        // returns the offset of the symbol by searching for the symbol table 
        // and finding the symbol 
        int returnSymOffset(std::string st_name, std::string sym_name);

        // symbol kind
        std::string returnSymKind(std::string st_name, std::string sym_name);
        
        int numTables = 0; // increments every time a method declaration has been found

};

#endif