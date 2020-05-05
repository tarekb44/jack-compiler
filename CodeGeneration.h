#ifndef CODEGENERATION_H
#define CODEGENERATION_H

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>

struct Commmand 
{
    std::string command; // store the command : pop arguments for example
    std::string num; // offset
};

enum VMCommands {
    add = 0,
    sub = 1, 
    AND = 2, 
    neg = 3, 
    eq = 4,
    OR = 5, 
    gt = 6, 
    lt = 7, 
    NOT = 8, 
    pop = 9,
    push = 10,
    LABEL = 11, 
    GOTO = 12, 
    IFGOTO = 13,
    function = 14,
    call = 15,
    RETURN = 16
};

// stringify the enum values of VMCommands
const std::string VMString[18] = {"add", "sub", "AND" , "neg" ,
"eq", "or", "gt" , "lt" , "NOT", "pop", "push", "label", "goto", "if-goto", "function", "call", "return"};


// for every code generation, create the respective .vm file
// code generation for each .jack file
class CodeGeneration {

    public:
        //file name used to generate name of the file
        CodeGeneration();
        CodeGeneration(std::string filename);

        // first all the commands are stored into a vector
        std::vector<Commmand> commands;

        const std::string vmStringifier(VMCommands commands);

        //gets the name of the file
        void getFileName();

        //creates the .vm file
        void createFile(std::string classname);

        // put each line of the vm file into
        // command var offset
        // var and offset can be null
        void printComToFile(VMCommands com, const std::string& var, const std::string& offset);

        std::string vmFile; // name of vm file

    public:
        std::string newFileVM;


};

#endif