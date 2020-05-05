#ifndef ALLCLASSES_H
#define ALLCLASSES_H

#include "SymbolTable.h"
#include "ClassTable.h"
#include <vector>
#include <algorithm>
#include <iterator>

// when an expression list is encountered
// add it to the expression type array
struct ExpressionList 
{
    //used to check later that it is valid
    std::vector<std::string> expr_type;
    std::string className;
    std::string subroutine_name;

    //used for throwing errors
    int linenum;
    std::string filename;
};

// check a method exists within a
// declared class
struct ClassMethod
{
    std::string className;
    std::string methodName;

    //used for throwing errors
    int linenumber;
    std::string filename;

    //if the first identifier is a variable
    //it must be declared in the current class
    std::string currentclass;
};

// check that let statements are compatible
struct StatementsCheck 
{
    //make sure that lhs matches rhs
    std::string lhsType;
    std::string methodName;
    
    //error reporting
    std::string filename;
    int linenum;
};

//this class holds the symbol tables of all the classes
class AllClasses {
    
    public:

        AllClasses();

        //stores all the class symbol tables
        std::vector <ClassTable *> classSymbolTables;

        std::vector<ClassMethod> classmethodarray;

        // keeps track of all class names
        std::vector <std::string> classnames;

        // keeps track of all expression lists
        std::vector<ExpressionList> exprlist_array;

        // keeps track of all statemeent types
        std::vector<StatementsCheck> statcompatibility;

        //prints arguments in statements used
        /* DEBUGGING */ 
        void printargs();

        //add a class table to the vector of class tables
        void addClassSymbolTable(ClassTable* classname);

        // deletes objects once we are done with them
        void clearObjects();
        
        //check if the class already exists
        bool classSymbolTableExists(std::string classtable);

        // checks if the method exists within a class, can throw error
        void checkMethodDeclaration();

        // only checks if method exists, within a class table name and method name
        bool methodExists(std::string classTable, std::string methodname);

        // gets class name as input, and checks if it exists, can throw error
        void classExists(std::string &classname);

        //checks if some let statements or return statements are compatible
        void checkStatCompatible();

        //this function checks if let and return statements are compatible if
        //there is a method
        void checkCompatibleTypes();

        // print all class names in AllClasses
        // used to create a VM file
        void printClasses();

        // check if the expression list in a method is correct in 
        // an expression
        void checkArgs();

        //delete data from vector
        void deleteData();

        //returns first symbol table found within classes
        SymbolTable * returnSymbolTable(std::string &symbol_table_name);

        // finds a symbol from class, searches all symbol tables from AllClasses, returns the name
        std::string returnSymbolFromClass(std::string classname, std::string symbol_name);

};

#endif