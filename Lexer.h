#ifndef LEXER_H
#define LEXER_H
#include <string>
#include <vector>

#include "Token.h"

// Lexer object that tokenizes each file
// throws error if invalid symbol is found
class Lexer {

    public:

        //accessors
        //name of the file
        std::string nameFile;

        //constructor
        Lexer();
        Lexer(std::string nameFile);

        //mutators - methods
        void set_filename(std::string nameFile);

        //copy elements from source file into vector
        void copySourceFileCharacters(std::string nameFile);
    
        //returns the next token, consumes the previous token
        Token GetNextToken();

        //peek token without consuming it
        Token PeekNextToken();

        //prints the tokens to terminal
        void print();
    
        //verifies whether a symbol is valid
        bool isAcceptableSymbol(char symbol);
    
        //checks if any parantheses are balanced
        bool validParantheses(char openP, char closeP);
    
        //tokenizes the source file
        void create_tokens();
    
        //tokenizes keyword
        std::string keyword(std::string str);
    
        //tokenizes symbols
        std::string symbol(std:: string c);
    
        //tokenizes integer constants
        std::string integer_constant(std::string x);
    
        //tokenize string constants
        std::string string_constant(std::string str2);
    
        //tokenizes identifiers     
        std::string identifier(std::string id);
        void callParser();

        // clears the tokens once done
        void clearTokens();

        // number of tokens in a file
        int tokenSize();


    private:
        std::string _nameFile;
};

#endif