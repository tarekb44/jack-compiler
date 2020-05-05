#ifndef COMPILER_H
#define COMPILER_H
#include "Token.h"
#include <dirent.h>
#include <sys/stat.h>
#include "AllClasses.h"

#include <string>


// main module that compiles the jack files
class Compiler{

    public:
        Compiler();
        char* directory; // this should be the args[1] from command prompt

        // opens the directory and processes each file
        void processFile(char *directory);

        // compiles each directory
        void startCompiling(std::string filename, AllClasses*globalst);     
        
};

#endif
