#ifndef PARSER_H
#define PARSER_H
#include "Symbol.h"
#include "SymbolTable.h"
#include "Token.h"
#include "Lexer.h"
#include "Logger.h"
#include "AllClasses.h"
#include "ClassTable.h"
#include "CodeGeneration.h"
#include <string>

#include "Symbol.h"
#include "SymbolTable.h"
#include "Token.h"
#include "Lexer.h"
#include "Logger.h"
#include "AllClasses.h"
#include "ClassTable.h"
#include "CodeGeneration.h"

#include <string>

// stores the name of subroutine name
struct SubroutineName 
{
    std::string name; // name of method
    int line; // used for error checking
};


// checks that lhs and rhs
// are compatible in a statement
struct statementTypes 
{
    std::string methodName;
    std::string statementType; // gets the varname of statement
    int line_num; // used for error 
};

// used to creat vm file
struct vmFile 
{
    std::string filename; //file of vm 
    bool isJackOS; // only creates file if this is false
};

// object that contains parsing, grammar, semantic, and code generation functions
class Parser{
 
    public:
        //create a local lexer object
        Lexer *localToken;

        // set the incoming allclasses module locally
        AllClasses *GlobalSymbolTable;

        // used to generete code
        CodeGeneration *generate;

        //this is a class-scope symbol table
        SymbolTable *classScopeSymbolTable = new SymbolTable();

        //create a local classtable object
        //only one class per file
        ClassTable* pclasstable = new ClassTable();
        
        std::string filename; // gets file name

        //constructor
        //Parser();
        Parser(Lexer *token, AllClasses *globalsymboltable, CodeGeneration *generation, std::string filename);
                    //: localToken(token), GlobalSymbolTable(globalsymboltable) {};
        
        //add symbols from class
        //first find symbol table then add symbol table
        std::vector<SubroutineName> subroutineNames;
        std::vector<statementTypes> typeCheck;

        /* DEBUGGING */
        void printSymbolTable();

        // start parsing, called from compiler module
        void startParsing();

        //handles class declarations in file
        void classDecleration();

        // either static, feild or subroutine declaration
        void memberDeclaration(std::string symboltable_name);

        // handles class var declrations
        void classVariableDeclaration(std::string symboltable_name);

        // gets the type 
        void type();

        // subroutine declaration functions, adds it to symbol table
        void subroutineDeclaration();

        // called after do statements
        void subroutineCall(std::string name);
        
        // handles the subroutine body
        void subroutineBody(std::string name);

        // determines the type of statements
        void statements(std::string name);

        // let statement handling
        void letStatements(std::string name);

        // if statement handling
        void ifStatements(std::string name);

        // while statement handling
        void whileStatement(std::string name);

        // do statement handling
        void doStatement(std::string name);

        // return statement handling
        void returnStatement(std::string name);

        // var statement handling
        void variableDeclarationStatement(std::string name);

        //each expression has a type
        std::string expression(std::string scope_name, const std::string& letstate);

        // expression list
        void expressionList(std::string class_scope, std::string scope_namestd, std::string currentclass, std::string let);

        //called for operator funcs
        std::string relationalExpression(std::string scope_name, std::string let);

        // called for add/sub
        std::string arithmeticExpression(std::string scope_name, std::string let);

        // used for operand
        std::string term(std::string scope_name, std::string let);

        // used for mult/divide
        std::string factor(std::string scope_name, std::string let);
        
        //NAME      |     TYPE ->  int | char | boolean | identifier   |    KIND   |    #    |
        // accepts let statement to determine if the let statement varname is the same as the 
        // symbol name to initialize the symbol
        // and do code generation functions
        std::string operand(std::string scope_name, std::string let);

        // called in subroutine declaration
        void parameterList(std::string name);

        //check that the declaration of subroutine
        void checkSubroutineDeclaration();

        //check the subroutine name
        bool checkSubNames(std::string sub_name);

        //check the type of method
        void methodTypeCheck();

        vmFile vfile; // used for vmfile creating

        // create a vm file after parsing
        void createVMFile();
    
    private :

};

#endif

