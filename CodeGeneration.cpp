#include "CodeGeneration.h"

std::ofstream out; // used to write in the file

// module that generates the code from a jack file to .vm file, accepts file name
CodeGeneration::CodeGeneration(std::string filename){ this->newFileVM = filename; }

// method that is used to write to the .vm file
// first all the commands are collected within a vector
// accepts the VMCommands (pop, push, call etc. ), var name, and offset
// puts the commands in form 
// COMMAND VARKIND OFFSET
// for example:
// ---- push argument 7
void CodeGeneration::printComToFile(VMCommands com, const std::string& var, const std::string& offset){
    Commmand tempCommand;

    // converts the enum to a string value of the command
    std::string s = ("%s", VMString[com].c_str());
    s = s + " " + var;

    tempCommand.command = s;

    // sets the offset of the command
    if(offset != "null"){
        tempCommand.num = offset;
    }

    // add the command to vector
    // later it will be put into file
    commands.push_back(tempCommand);    
}

// automatically creates a file when a non JACK OS API is made
// used to put generated code
void CodeGeneration::createFile(std::string classname){
    out.open(classname + ".vm"); // add .vm file

    if(!out.is_open())
    {
        std::cerr << "Error creating file";
        exit(1);
    }

    std::vector<Commmand>::size_type i;

    for(i = 0; i != commands.size(); i++){
        out << commands[i].command << " " << commands[i].num << "\n";
    }

    out.close(); // close file once done
}