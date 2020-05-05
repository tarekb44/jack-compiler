#include "Compiler.h"
#include "Lexer.h"
#include "Parser.h"
#include "Lexer.h"

// COMPILER PROCESS:
// 1. Compiler calls Lexer, it then creates tokens
// 2. Compiler calls parser
// 3. Parser does semantic analysis
// 4. Parser does code generation


/*
* TO COMPILE JACK OS API :
* put all JACK OS API files in the same directory
* as the JACK program
* 
* This module only accepts directory names
*/

// main module that compiles the program
Compiler::Compiler() {}

// opens the directory from the command prompt file name
void Compiler::processFile(char* directory){
    DIR *direct; //pointer to directory
    struct dirent *files; //files in dir
    struct stat dir_info; //directory info
    AllClasses allclassessymboltable; //one global symbol table

    // set a pointer to the AllClasses module
    AllClasses *globalstptr;

    // set the pointer to reference
    globalstptr = &allclassessymboltable;

    std::string filepath, dir; // get the file path

    dir = std::string(directory); // set the dir to string of the directory

    direct = opendir(directory); // open directory that is entered
    if(!(direct)){
        std::cout << direct << " directory does not exist";
        exit(1);
    }

    //process
    while((files = readdir(direct)) != NULL){
        if(files->d_name[0] != '.'){
            filepath = dir + "/" + files->d_name;
            startCompiling(filepath, globalstptr); // compile every file in directory

            //determine if it is a directory
            stat(filepath.c_str(), &dir_info);
            if(S_ISDIR(dir_info.st_mode)){
                // recursively call processFile
                // start the process again
                processFile(directory);

            }
        }
    }
    
    /* DEBUGGING CALLS */
    // Uncomment the code if you want to
    // see the symbol tables
        //globalstptr->printClasses();
        //globalstptr->printargs();

    //once all the files compiled 
    //check for appropriate subroutine semantics

    // check that the method has been declared in a statement
    globalstptr->checkMethodDeclaration();

    // check that all expression lists are valid
    globalstptr->checkArgs();

    // check that the lhs and rhs of each statement is valid
    // (for functions)
    globalstptr->checkStatCompatible();

    // delete the data from all classes table
    // avoid memeory leaks
    globalstptr->deleteData();

    closedir(direct); // close the directory

}

// start the compiling each file in a directory
// accepts the name of file, opened up by the lexer object
// accepts one AllClasses module since there will be only one per program
void Compiler::startCompiling(std::string filename, AllClasses *globalst){
    //create a code generation module for each .jack file
    CodeGeneration generation(filename);
    
    CodeGeneration *generptr; // set the pointer

    generptr = &generation; // set it eqaul to the reference

    // lexer creates a token object and fills it with tokens
    // one lexer object for each file
    Lexer newLexer(filename);

    //assign a pointer to class Lexer
    Lexer *lexerptr;
    
    //save the address of the lexer object
    lexerptr = &newLexer;

    //access and call the function that creates tokens
    newLexer.create_tokens();

    /* DEBUGGING CALL */
    // Uncomment the next line to see the tokens
        // newLexer.print();
    
    //call the parser and input the lexer object and a global symbol table
    Parser parser(lexerptr, globalst, generptr, filename);

    //start parsing the tokens
    parser.startParsing();
    
    /* DEBUGGING CALL */
    // uncomment to see the symbol tables
        //parser.printSymbolTable();

    // create a vm file once the tokens have been processed
    parser.createVMFile(); // used for code generation

    // delete the tokens
    // memory leaks
    newLexer.clearTokens();

}

