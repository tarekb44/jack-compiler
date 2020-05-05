#include <fstream>
#include <iostream>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <string>
#include <ctype.h>
#include <cstdlib>
#include <iterator>
#include <vector>
#include <sstream>
#include "Lexer.h"
#include "Parser.h"
#include "Logger.h"
#include <functional>
#include <cstdlib>

FILE *inputFile; // input the file by its name from directory
int position = 0;

Token token; // creates a token object

//constructor
Lexer::Lexer(std::string nameFile){ this->_nameFile = nameFile; }

//sets the file name locally
void Lexer::set_filename(std::string nameFile) {_nameFile = nameFile;}

//function that produces the tokens from a given file
void Lexer::create_tokens(){
   //std::ifstream inputFile;
    std::string filename = _nameFile;
    inputFile = fopen(filename.c_str(), "r");

    //check if the file is open
    if(inputFile == NULL){
        perror ("Error opening file!");
        exit(1);
    }

    //start from line number 1
    int lineNumber = 1;

    //store the current character read
    int c;
    int state = 0;

    //string that will store the variable (keyworrd, identifier, boolean)
    std::string variable;
    //store the variable keyword
    std::string varType;

    //store the integers
    std::string number;
    //store the integer tokens
    std::string numberToken;

    //store the literal string
    std::string literalString;

    //store the symbol value
    std::string symValue;
    //store the symbol token
    std::string symToken;
   
    //while file charachter is not EOF
    while((c = getc(inputFile)) != EOF)
    {
        //skip over white spaces
        if(isspace(c))
        {
            //if this is a new line then increment
            if(c == '\n'){
                lineNumber++;
            }

        }

        //if we encounter a potential comment then ignore
        if(c == '/')
        {
            char curChar = c;

            //get next character
            c = getc(inputFile);

            // single line commmetn
            if(c == '/')
            {
                //while character is not a new line, consume
                while((c = getc(inputFile)) != '\n') {}

                //increment line number
                lineNumber++;

            // this is multi line
            } else if(c == '*'){
                int q;

                c = getc(inputFile);

                // keep consuming while we dont find '*/'
                do{
                    if(c == '\n'){
                        lineNumber++;
                    }
                    q = c;

                    c = getc(inputFile);

                } while(c!= '/' || q != '*');
                //consume the '/'
                c = getc(inputFile);

            //else this is just a '/'
            } else {
                Token current_token;

                current_token.lexeme = "/";
                current_token.type = TokenType::symbol;
                current_token.lineNum = lineNumber;
                current_token.tokenName = "<symbol>/</symbol>";

                token.token_array.push_back({current_token.type, current_token.lexeme, current_token.lineNum, current_token.tokenName});
            }
        }
            
        //we have encountered a character
        if(isalpha(c) || c == '_'){

            //store into a local variable
            variable += c;

            //while there is a character read
            while((isalpha((c = fgetc(inputFile)))) || isdigit(c) && !(ispunct(c)))
            {
                //add to variable
                variable += c;

            }

            //Token object that handles the token 
            Token currentToken0;

            //determine the type of variable
            varType = keyword(variable);

            //if token returned is a keyword then store it as a keyword
            if(varType == "<keyword>"+variable+"</keyword>") {
                 //set the type to identifier
                currentToken0.type = TokenType::keyword;
                
                //store the token
                currentToken0.tokenName = varType;

                //store the lexeme 
                currentToken0.lexeme = variable;

                //store the line number
                currentToken0.lineNum = lineNumber;

            //else if token returned is boolean
            } else if(varType == "<boolean>"+variable+"</boolean>") {
                //set the type to boolean
                currentToken0.type = TokenType::boolean;
                
                //store the token
                currentToken0.tokenName = varType;

                //store the lexeme 
                currentToken0.lexeme = variable;

                //store the line number
                currentToken0.lineNum = lineNumber;

            //else if token is an identifier
            } else if(varType == "<identifier>"+variable+"</identifier>") {
                //set the type to identifier
                currentToken0.type = TokenType::identifier;
                
                //store the token
                currentToken0.tokenName = varType;

                //store the lexeme 
                currentToken0.lexeme = variable;

                //store the line number
                currentToken0.lineNum = lineNumber;
            }  

            //store the token into the array
            token.token_array.push_back({currentToken0.type, currentToken0.lexeme, currentToken0.lineNum, currentToken0.tokenName});
              
            //clear the variables so we can use them again in the next iteration
            variable.clear();
            varType.clear();

        //we have encountered a digit
        } else if(isdigit(c))
        {
            //used for pushing the token array
            Token currentToken1;

            //store the first character
            number += c;
            
            //if the next character is a a variable, then this is an error
            if(isalpha(c = fgetc(inputFile))){
                std::cout << "Error! You cannot have a number infront of a variable at line: " + lineNumber;
                exit(1);
            }

            //return the character
            ungetc(c, inputFile);

            //add to the number variable
            number += c;

            //while we recieve digits then store to variable
            while (isdigit((c = fgetc(inputFile))))
            {
                if(ispunct(c)){        
                    //LOG_ERROR("Lexer", "No symbols allowed in integer constant", "Main.jack", lineNumber);

                }
                //add to local variable
                number += c;
            }

            //create token
            numberToken = "<integerConstant>"+number+"</numberConstant>";

            //set the token to integer
            currentToken1.type = TokenType::integerConstant;
            currentToken1.tokenName = numberToken;
            currentToken1.lexeme = number;

            //store the line number
            currentToken1.lineNum = lineNumber;

            //store the token into the array
            token.token_array.push_back({currentToken1.type, currentToken1.lexeme, currentToken1.lineNum, currentToken1.tokenName});
             
            //clear the temporary variables for future use
            number.clear();
            numberToken.clear();
    
        }

        //if this is a symbol
        if(ispunct(c)) 
        {
            //if we encounter a stringLiteral
            if (c == '"')
            {
                Token currentToken2;

                //ignore the first symbol, while we do not encounter another '"', then store           
                while((c = fgetc(inputFile)) != '"'){
                    //store into local variable;
                    literalString += c;

                }

                //set the token
                currentToken2.type = TokenType::stringLiteral;
                currentToken2.tokenName = string_constant(literalString);
                currentToken2.lexeme = literalString;

                //store the line number
                currentToken2.lineNum = lineNumber;

                //store the token into the array
                token.token_array.push_back({currentToken2.type, currentToken2.lexeme, currentToken2.lineNum, currentToken2.tokenName});
                
                //clear the local variable for future use
                literalString.clear();

                continue;

            } 

            //used for handling the tokens
            Token currentToken3;
            
            //turn char into string
            std::string s(1, c);

            //call the symbol token function
            symToken = symbol(s);

            //check if this is a valid symbol
            if(symToken == "-1"){
                //throw an error message and exit
                std::cout << "ERROR: Lexer: '" << c<< "' is an invalid symbol at line: " << lineNumber;

                //clear the local variable
                symToken.clear();

                exit(1);
            }

            //set the token
            currentToken3.type = TokenType::symbol;
            currentToken3.tokenName = symToken;
            currentToken3.lexeme = s;

            //store the line number
            currentToken3.lineNum = lineNumber;

            //store the token into the array
            token.token_array.push_back({currentToken3.type, currentToken3.lexeme, currentToken3.lineNum, currentToken3.tokenName});
             
            //clear the variables
            literalString.clear();
            symToken.clear();
            s.clear();
        }
    }

    Token eof1;
    eof1.type = TokenType::stringLiteral;
    eof1.tokenName = "<eof>eof</eof>";
    eof1.lexeme = "EOF";
    eof1.lineNum = lineNumber;

    //reached EOF
    token.token_array.push_back({eof1.type, eof1.lexeme, eof1.lineNum, eof1.tokenName});

    if(token.token_array[0].lexeme == "/"){
       token.token_array.erase(token.token_array.begin()+0);
    }
}


//returns the index of the next token
Token Lexer::GetNextToken(){
   if(position < token.token_array.size()){
    return token.token_array[position++];

   } else {
        //throws error and exits if the token is unavailable 
        //LOG_ERROR("Lexer", "GetNextToken() is unavailable, no more available tokens", "Main.jack", 0);
   }
}

//return the next index of the array
Token Lexer::PeekNextToken(){
    if(position + 1 < token.token_array.size() && position + 1 >= 0){
        return token.token_array[position];
    } else {
        //throw error message and exit if no more available tokens 
        //LOG_ERROR("Lexer", "PeekNextToken() is unavailable, no more available tokens", "Main.jack", 0);
    }
}

//prints the tokens at the lexer stage
void Lexer::print(){
    for(int i=0; i< token.token_array.size(); ++i){
        std::cout << token.token_array[i].lexeme << "\t line: " << token.token_array[i].lineNum << "\n"; 
    }
}

//returns the string literal token
std::string Lexer::string_constant(std::string str2){
    //returns the string token
    return "<stringLiteral>" + str2 + "</stringLiteral>";
}

//determines if the string is a keyword, identifier, or boolean
std::string Lexer::keyword(std::string str){
    if(str == "class" || str == "constructor" || str == "function" || str == "method" ||
       str == "feild" || str == "static" || str == "var" ||
       str == "void" || str == "null" || str == "this" ||
       str == "let" || str == "do" || str == "if" || str == "else" || str == "while" || str == "return" ){
        
        //if the string matches then this is a keyword
        return "<keyword>"+ str+"</keyword>";

    } else if(str == "true" || str == "false"){
        //else if it is 'true' or 'false' then this is a boolean
        return "<boolean>"+str+"</boolean>";

    } else{

        //else this is just an identifier
        return "<identifier>" + str + "</identifier>";
    }
}

//determines if the symbol is valid, if not it will return false
std::string Lexer::symbol(std::string str2){
    if(str2 == "." || str2 == ">"|| str2 == ")"|| str2 == "," || str2 =="{" || str2 == "}" ||
       str2 == ";" || str2 == "[" || str2 == "+" || str2 == "(" || str2 == "-" || str2 == "*" || str2 == "/" ||
       str2 == "<" || str2 == "=" || str2 == "]"  || str2 == "~" || str2 == "&" || str2 == "|" || str2 == "\"" || str2 == "!" || str2 == ":" || str2 == "'"){
     
        //returns the symbol token
        return "<symbol>" + str2 + "</symbol>";

    } else {
        //return "-1" if invalid
        return "-1";
    }
}

// return the integer token format
std::string Lexer::integer_constant(std::string x){
    return "<integerConstant>" + x + "</integerConstant>";
}

// clear all the tokens
void Lexer::clearTokens(){
    position = 0;
    token.token_array.clear();
}

// number of tokens
int Lexer::tokenSize(){
    position = 0;
    return token.token_array.size();
}
