#ifndef TOKEN_H
#define TOKEN_H

#include "Logger.h"

#include <string>
#include <map>
#include <vector>
#include <iostream>

//all the different types of a token
enum TokenType{
    keyword = 0,
    symbol = 1,
    identifier = 2,
    integerConstant = 3,
    stringLiteral = 4,
    boolean = 5, 
    charConstant = 6
};

//returns the type as a string
const std::string TokenTypeString[9] = {"keyword", "symbol", "identifier" , "integer" ,
"stringLiteral", "boolean", "char" , "eof"};

// lexer created a token object to be filled, used in Parser
class Token{

    public:
        // constructors
        Token();
        Token(TokenType type, std::string lexeme, int lineNum, std::string tokenName);

        //accessors
        TokenType type;
        std::string lexeme;
        int lineNum;

        //tokens put into the formats: <'non-terminals'>terminal</'non-terminal'>
        std::string tokenName;

        //store the tokens from lexer
        std::vector<Token> token_array;

        //return the string of the enum (tokentype)
        const std::string stringToken(TokenType val);

    //member variables
    private:
        int _position;
        TokenType _type;

};

#endif