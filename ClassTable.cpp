#include "ClassTable.h"

// Main object that keeps all the symbol tables
// Contains some semantic functions, for example,
// returning the method type of a function
ClassTable::ClassTable(){
    this->className = "Main";
}

// constructor
ClassTable::ClassTable(std::string className){
    this->className = className;
}

// accepts a symbol table name, and checks if it contains a return 
// statement. This helps ensure that all functions contain
// a return statement as part of the JACK rules
bool ClassTable::hasReturnStatement(std::string st_name){
    std::vector<SymbolTable *>::size_type it;

    //search for symbol table by name
    for(it = 0; it != classtables.size(); it++){
        if(this->classtables[it]->nameSymbolTable == st_name){
           return classtables[it]->returnStatement;
        }
    }
}

// when encountering a return statement, this function sets
// confirms that it has a return statement
// function is used later for checking 
void ClassTable::methodHasReturnStatement(std::string st_name){
    std::vector<SymbolTable *>::size_type it;

    //search for symbol table by name
    for(it = 0; it != classtables.size(); it++){
        if(this->classtables[it]->nameSymbolTable == st_name){

            //set the return statement to true
           classtables[it]->returnStatement = true;
        }
    }
}

// this create a symbol table and adds it to the array of symbol tables
// in this module, each table needs a name , type, a method type, a class that it is in, and the offset 
// which is used later in code generation
void ClassTable::createMethodSymbolTable(std::string st_name, std::string st_type, std::string m_type, std::string parentClass, int offset){
    //create an object locally
    SymbolTable *symTPtr = new SymbolTable();
   
    //give it a name
    symTPtr->nameSymbolTable = st_name;

    //set the type-> void, int etc.
    symTPtr->type = st_type;

    // set the type (constructor, method or function)
    symTPtr->method_type = m_type;

    // class that the method is declared in
    symTPtr->parentClass = parentClass;

    // used in code generation
    symTPtr->functionOffset = offset;

    //add a new symbol table to the array
    classtables.push_back(symTPtr);
}

//returns constructor, method, function etc.
std::string ClassTable::methodType(std::string st_name){
   std::vector<SymbolTable>::size_type it;

    //search for symbol table by name
    for(it = 0; it != classtables.size(); it++){
        if(this->classtables[it]->nameSymbolTable == st_name){
           return classtables[it]->method_type;
        }
    }

    //if not found return null
    return "null";
}

// adds symbol to a symbol table
// accepts the name of the symbol table, and the symbol,
// then adds a symbol to it
void ClassTable::accessSymbolTable(std::string name, Symbol symbol){
    std::vector<SymbolTable>::size_type it;

    //search for symbol table by name
    for(it = 0; it != classtables.size(); it++){
        if(this->classtables[it]->nameSymbolTable == name){
            //add a symbol to the symbol table
            this->classtables[it]->addSymbol(symbol);
        }
    }
}

// add symbol table to the current class object symbol tables
void ClassTable::addSymbolTable(SymbolTable *symboltable_name){
    //add the symbol table to the vector
    classtables.push_back(symboltable_name);
}

// searches for symbol tables with just its name
// when found returnt the index of it in the class symbol tables
// vector
int ClassTable::findSymbolTable(std::string name_table){
    std::vector<SymbolTable>::size_type it;

    //search for symbol table by name
    for(it = 0; it != classtables.size(); it++){
        if(this->classtables[it]->nameSymbolTable == name_table){
            //return index
            return it;
        }
    }

    //if not found return '-1' (ie false)
    return -1;
}

// search for symbol by its name in a particular symbol table that is within
// the current class it is in
int ClassTable::searchForSymbol(std::string s, std::string scope_name){
   std::vector<SymbolTable *>::size_type it;

    //search for symbol table by name
    for(it = 0; it != classtables.size(); it++){
        if(this->classtables[it]->nameSymbolTable == scope_name){
            if(classtables[it]->findSymbol(s) == 1){
                return 1;
            }
        }
    }

    return -1;
}

// returns the type of the symbol within a method scope
std::string ClassTable::returnSymbolType(std::string symbolName, std::string scope){
    //vector of symbol tables
    std::vector<SymbolTable *>::size_type it;
    std::string symtype;

    //search for symbol table by name
    for(it = 0; it != classtables.size(); it++){
        if(this->classtables[it]->nameSymbolTable == scope){
            symtype =  classtables[it]->symbolType(symbolName);
            
            return symtype;
        }
    }  

    return "null";
}

// this function tells the compiler if the symbol has been initialized so 
// that it can be used in an expression
std::string ClassTable::initSymbol(std::string symbol, std::string scope){
    //vector of symbol tables
    std::vector<SymbolTable *>::size_type it;
    std::string symtype;

    //search for symbol table by name
    for(it = 0; it != classtables.size(); it++){
        if(this->classtables[it]->nameSymbolTable == scope){
            symtype = classtables[it]->isSymbolInitialized(symbol);
            
            return symtype;
        }
    }  
}

// whenever a let statement is encountered the symbol is initialized
// so that it can be used in an expression
void ClassTable::symbolInitialize(std::string symbolname, std::string scope){
     //vector of symbol tables
    std::vector<SymbolTable *>::size_type it;
    bool symtype;

    //search for symbol table by name
    for(it = 0; it != classtables.size(); it++){
        if(this->classtables[it]->nameSymbolTable == scope){
            classtables[it]->initializeSymbol(symbolname);
        }
    }
}

// returns a struct object of MethodName
// returms the type of the fucntion and the kind (constructor, method, function)
ClassTable::MethodName ClassTable::symbolTable(std::string st_name){
    MethodName localtable;

    std::vector<SymbolTable>::size_type it;

    //search for symbol table by name
    for(it = 0; it != classtables.size(); it++){
        if(classtables[it]->nameSymbolTable == st_name){
           localtable.method_type = classtables[it]->method_type;
           localtable.function_type = classtables[it]->type;
           return localtable;
        }
    }
}

// return the function type of method, used in statement compatibility
// and to ensure that return statements are valid within a symbol table
std::string ClassTable::returnFunctionType(std::string st_name){
    std::vector<SymbolTable>::size_type it;

    //search for symbol table by name
    for(it = 0; it != classtables.size(); it++){
        if(this->classtables[it]->nameSymbolTable == st_name){
           return classtables[it]->type;
        }
    }

    //if not found return null
    return "null";
}

// return the offset of a symbol from a symbol table
// used primarily in code generation phase to indicate the symbol offset
// accepts the symbol table name and the symbol name
int ClassTable::returnSymOffset(std::string st_name, std::string sym_name){
    std::vector<SymbolTable *>::size_type it;

    for(it = 0; it != classtables.size(); it++){
        if(classtables[it]->nameSymbolTable == st_name){

            //return offset from ST used for code generation
            return classtables[it]->returnSymbolOffset(sym_name);
        }
    }
    
    //if sym is not found in current symbol table, return false
    return -1;
}

// returns the symbol kind of a particular symbol (argument, feild etc.)
// this is used in code generation to determine the offset and if it is 
// a class or method level variable
// accepts the symbol table name and the symbol name so that it can be found
// within a particular class object
std::string ClassTable::returnSymKind(std::string st, std::string sym){
    std::vector<SymbolTable>::size_type it;

    //print the table name and the contents
    for(it = 0; it != classtables.size(); it++){
       if(classtables[it]->nameSymbolTable == st){

           return classtables[it]->symKind(sym);
       }
    }

    return "null";
}

// print all the tables in a class object
/* DEBUGGING FUNCTION */
void ClassTable::printTables(){
    std::vector<SymbolTable>::size_type it;

    //print the table name and the contents
    for(it = 0; it != classtables.size(); it++){

        /* METHOD SYMBOL TABLE : Name  
        * TABLE
        */
        std::cout << "\nMethod Symbol Table Name\t"  << this->classtables[it]->nameSymbolTable << "\n\n";
        this->classtables[it]->printTable();
        std::cout << "\n\n";
    }
}

// whenever we are in an expression list, add the the argument type
// this is used later to check that the arguments in the expression lists
// are valid, and are compatible with the types
// every symbol table has an argument list (even if it is null)
void ClassTable::addArgsSymbolTable(std::string args_type, std::string scope_name){
    std::vector<SymbolTable *>::size_type it;

    //search for symbol table by name
    //add argument to symbol table
    for(it = 0; it != classtables.size(); it++){
        if(this->classtables[it]->nameSymbolTable == scope_name){

            //add it the the symbol table argument list
           this->classtables[it]->argsTypeArray.push_back(args_type);
        }
    }
}

// delete objects in the array to avoid memory leaks
void ClassTable::deleteObjs(){
    std::vector<SymbolTable *>::iterator obj;
        for(obj = classtables.begin(); obj != classtables.end(); obj++){
            delete *obj;
        }

        classtables.clear();
        
}