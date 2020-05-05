#include "Compiler.h"
#include "Lexer.h"
#include "Parser.h"

#include <iostream>
#include <cstring>

int main(int argc, char *argv[]){

    // note that the JACK OS API files 
    // need to be in the same directory as the 
    // jack files to be executed
    
    // must only be two arguments
    // execution file and directory name to be compiled
    if (argc == 1) {
        std::cerr << "\nError! Need to supply .jack directory \n";
        return 1;
    }
    
    // main module
    // one per compilation
    Compiler compiler;

    // input the directory name to be processed
    compiler.processFile(argv[1]); 
}