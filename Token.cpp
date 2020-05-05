#include "Token.h"

//constructor 
Token::Token(){
    this->type = TokenType::keyword;
    this->lexeme = "EOF";
    this->lineNum = 1;
    this->tokenName = "<EOF>EOF</EOF>";
}

//initialization
Token::Token(TokenType type, std::string lexeme, int lineNum, std::string tokenName){
    this->type = type;
    this->lexeme = lexeme;
    this->lineNum = lineNum;
    this->tokenName = tokenName;
}

//returns the enum as a string of the token type
const std::string Token::stringToken(TokenType val){
    this->_type = val;

    return ("%s", TokenTypeString[val].c_str());
}



