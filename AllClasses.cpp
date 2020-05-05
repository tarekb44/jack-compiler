#include "AllClasses.h"

// Object that holds all symbol tables of a program
// Contains functions that checks for remaining semantics 
// after a program has been fully compiled
AllClasses::AllClasses(){}

// Adds symbol tables that are contained within a particular class
// Accepts a pointer object of the class table module
void AllClasses::addClassSymbolTable(ClassTable* classname) {
    //push the incoming table to the vector of classtables
    classSymbolTables.push_back(classname);
}

// Whenever an initialization is used in a return, let statements,
// for example, this function checks that they are compatible and 
// have the same type after the program has been compiled 
void AllClasses::checkStatCompatible(){
    // checks that the types are equal
    // this vector contains all the different types
    std::vector<StatementsCheck>::size_type it;

    // iterates through all statements, check if they have equal type
    // throws semantic error if incompatible
    for(it = 0; it != statcompatibility.size(); it++){
        if(returnSymbolTable(statcompatibility[it].methodName)->type == statcompatibility[it].lhsType) {
           LOG_SUCCESS("Semantic Analayzer", statcompatibility[it].methodName + " is compatible in subroutine", exprlist_array[it].filename, exprlist_array[it].linenum);

        } else {
            LOG_ERROR("Semantic Analayzer", "'" + statcompatibility[it].methodName + "' is not compatible with in the 'let' statement", exprlist_array[it].filename, statcompatibility[it].linenum);

        }
    }
}

// checks if the incoming class table name already exists 
// primarily used for checking class redeclaration
bool AllClasses::classSymbolTableExists(std::string name_table){
    std::vector<std::string>::size_type it;

    // if found exit the funtion
    for(it = 0; it != classnames.size(); it++){
        if(classnames[it] == name_table){
            return true;
        }
    }

    return false;
}

// after program has been compiled
// delete data to avoid memory leaks
void AllClasses::deleteData(){
    std::vector<ClassTable *>::iterator obj;
    for(obj = classSymbolTables.begin(); obj != classSymbolTables.end(); obj++){
        delete *obj;
    }

    classSymbolTables.clear();
}

// functions used to ensure that the arguments match
// with the subroutine declaration when used in an
// expression
void AllClasses::checkArgs(){
    //list of all declared expression arguments
    std::vector<ExpressionList>::size_type it;

    //list of all class tables 
    std::vector<ClassTable*>::size_type x;

    //list of symbol tables in class
    std::vector<SymbolTable*>::size_type y;

    // loop through the arguments array
    for(it = 0; it != exprlist_array.size(); it++){
        // loop through the current global symbol table
        for(x = 0; x != classSymbolTables.size(); x++){                
                // once the subroutine is found, compare that
                // they have equal argument declartion
                for(y = 0; y != classSymbolTables[x]->classtables.size(); y++){

                    //if this is a subroutine name and the same class
                    if(classSymbolTables[x]->classtables[y]->nameSymbolTable == exprlist_array[it].subroutine_name 
                    && classSymbolTables[x]->classtables[y]->parentClass == exprlist_array[it].className){

                        // check if they are equal using algorithm library
                        if(std::equal(std::begin(classSymbolTables[x]->classtables[y]->argsTypeArray), 
                        std::end(classSymbolTables[x]->classtables[y]->argsTypeArray),
                        std::begin(exprlist_array[it].expr_type)) && (classSymbolTables[x]->classtables[y]->argsTypeArray.size() 
                        == exprlist_array[it].expr_type.size())){       
                            LOG_SUCCESS("Semantic Analayzer", exprlist_array[it].subroutine_name + " has been proper args (classname)", exprlist_array[it].filename, exprlist_array[it].linenum);

                        } else {
                            LOG_ERROR("Semantic Analayzer", "'" + exprlist_array[it].subroutine_name + "' subroutine must have the same number and type of arguments as its declaration", exprlist_array[it].filename, exprlist_array[it].linenum);

                        }
                    }  
                }
            }
        }
}

// Print the arguments used in an expression
/* DEBUGGUING FUNCTION */
void AllClasses::printargs(){
    //print the expression list
    std::vector<ExpressionList>::size_type it;
    std::vector<std::string>::size_type x;

    //print the args stored in each symbol table
    std::vector<ClassTable *>::size_type y;
    std::vector<SymbolTable *>::size_type v;
    std::vector<std::string>::size_type b;

    // print the expression list 
    for(it = 0; it != exprlist_array.size(); it++){
        std::cout << exprlist_array[it].subroutine_name;
        for(x = 0; x != exprlist_array[it].expr_type.size(); x++){
                std::cout << exprlist_array[it].expr_type[x];
        }
        break;
    }

    std::cout << "\n\n\n";

    // print the arguments from a subroutine declaration
    for(y = 0; y != classSymbolTables.size(); y++){
        for(v = 0; v != classSymbolTables[y]->classtables.size(); v++){
            std::cout << classSymbolTables[y]->classtables[v]->nameSymbolTable;
            for( b = 0; b != classSymbolTables[y]->classtables[v]->argsTypeArray.size(); b++){
                if(classSymbolTables[y]->classtables[v]->nameSymbolTable == "gcd"){
                std::cout << classSymbolTables[y]->classtables[v]->argsTypeArray[b];

                }

            }
        }

        break;
    }

}

// Checks if the subroutine exists whithin a class
// and that it has been declared.
// Loops through entire AllClasses module to see if
// the subroutine is found from ClassMethod
// if not found, then throws a Semantic error
void AllClasses::checkMethodDeclaration(){
    std::vector<ClassMethod>::size_type it;
    std::vector<ClassTable *>::size_type j;

    // loops through the expressions subroutine calls
    for(it = 0; it != classmethodarray.size(); it++){
        // First checks if the class exists when used in identifier.identifier() etc.
        // The first identifier is a class name
        if(classSymbolTableExists(classmethodarray[it].className)){
            LOG_SUCCESS("Semantic Analayzer", classmethodarray[it].className + " has been declared (classname)", classmethodarray[it].filename ,classmethodarray[it].linenumber);

        //else this might be a user defined variable, so check var type in class tables
        } else if(classSymbolTableExists(classmethodarray[it].currentclass)) {
            LOG_SUCCESS("Semantic Analayzer", classmethodarray[it].className + " has been declared (classname)", classmethodarray[it].filename,classmethodarray[it].linenumber);
        
        // does not exists
        } else {
            LOG_ERROR("Semantic Analayzer", "'" + classmethodarray[it].className + "' class does not exist", classmethodarray[it].filename, classmethodarray[it].linenumber);
 
        }

        //now check if the method exists within class name
        if(methodExists(classmethodarray[it].className, classmethodarray[it].methodName)){
            LOG_SUCCESS("Semantic Analayzer", classmethodarray[it].methodName + " has been declared (subroutine)", classmethodarray[it].filename,classmethodarray[it].linenumber);

        //else check with the variable class 
        } else if (methodExists(classmethodarray[it].currentclass, classmethodarray[it].methodName)){
            LOG_SUCCESS("Semantic Analayzer", classmethodarray[it].className + " has been declared (classname)", classmethodarray[it].filename,classmethodarray[it].linenumber);

        // if not found throw error
        } else {
            LOG_ERROR("Semantic Analayzer", classmethodarray[it].methodName + " does not exist", classmethodarray[it].filename, classmethodarray[it].linenumber);

        }
    }
}

// searches the entire AllClasses module 
// to find the symbol
std::string AllClasses::returnSymbolFromClass(std::string classname, std::string symbol_name){
    std::vector<ClassTable *>::size_type j;
    std::vector<SymbolTable *>::size_type it;

    //symbol type
    std::string type;

    for(j = 0; j != classSymbolTables.size(); j++){
        //found class symbol table
        if(classSymbolTables[j]->className == classname){
            for(it = 0; it != classSymbolTables[j]->classtables.size(); ++it){
                type = classSymbolTables[it]->returnSymbolType(symbol_name, classname);
                
                //now loop through all the symbol tables, return the first symbol found
                if(type != "null"){
                    return type;
                }
            }
            
        }
    }

    // If not found return null
    return "null";
}

//check if a method exists when used in an expression
//(let a = a.print()) -> check for print in a->Fraction etc.
//when encountering a -> first check if it it is a class name 
//if not then check the variable type
bool AllClasses::methodExists(std::string classtable, std::string methodname){
    std::vector<ClassTable*>::size_type it;
    for(it = 0; it != classSymbolTables.size(); it++){
        if(classSymbolTables[it]->className == classtable){
            if(classSymbolTables[it]->findSymbolTable(methodname) == -1){
                return false;
            } else {
                return true;
            }
        }
    }
}

//check if class exists when used as a variable type (var (functionclass) a, b, c)
void AllClasses::classExists(std::string &classname){
    std::vector<std::string>::size_type it;

    // Throw error if class name cannot be found in AllClasses
    for(it = 0; it != classnames.size(); it++){
            if(classSymbolTableExists(classnames[it])){
                LOG_SUCCESS("Semantic Analayzer", classnames[it] + " has been declared (subroutine)", classmethodarray[it].filename,classmethodarray[it].linenumber);
            } else {
                LOG_ERROR("Semantic Analayzer", "'" + classnames[it] + "' does not exist", classmethodarray[it].filename, classmethodarray[it].linenumber);
        }
    }
}

// returns the symbol table from AllClasses
// searched entire scope
// accepts the symbol table name that user wants to find
SymbolTable * AllClasses::returnSymbolTable(std::string &symboltable){
    std::vector<ClassTable*>::size_type it;
    std::vector<SymbolTable*>::size_type x;

    for(it = 0; it != classSymbolTables.size(); it++){
        for(x = 0; x != classSymbolTables[it]->classtables.size(); x++){
            
            //return table and exit
            if(classSymbolTables[it]->classtables[x]->nameSymbolTable == symboltable){
                return classSymbolTables[it]->classtables[x];
            }
        }
    }
}

// prints the entire classes symbol tables in AllClasses
/* DEBUGGING FUNCTION */
void AllClasses::printClasses(){
    std::vector<ClassTable*>::size_type it;

    for(it = 0; it != classSymbolTables.size(); it++){
        std::cout << classSymbolTables[it]->className << "\n";
    }
}
