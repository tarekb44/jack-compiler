#include "Parser.h"
#include "SymbolTable.h"
#include "Symbol.h"
#include <iostream>

Parser::Parser(Lexer *token, AllClasses *globalsymboltable, CodeGeneration *generation, std::string filename){
    this->localToken = token;
    this->GlobalSymbolTable = globalsymboltable;
    this->filename = filename;    
    this->generate = generation;
}

//function that starts parsing the .jack file
void Parser::startParsing(){
    classDecleration();
}

//classDeclar - class identifier { {memberDeclar} }
void Parser::classDecleration(){
    //create a local token object to reveal lexer
    Token tempToken;

    std::string classname;

    //get the next token
    tempToken = localToken->GetNextToken();

    //expecting a class declaration
    if(tempToken.lexeme == "class"){  
        //log as successfull  
        LOG_SUCCESS("Parser", tempToken.lexeme, filename, tempToken.lineNum);

    } else {
        //terminate if error is encountered
        LOG_ERROR("Parser", "Expected a 'class' declaration", filename, tempToken.lineNum);
    }

    //consume the next token is an identifier for the class name
    tempToken = localToken->GetNextToken();

    //expecting a class name
    if(tempToken.type == TokenType::identifier){
        //if there is an identifier than this is good
        LOG_SUCCESS("Parser", tempToken.lexeme, filename, tempToken.lineNum);
        
        //check if the class already exists
        if(GlobalSymbolTable->classSymbolTableExists(tempToken.lexeme)){
            //if it returns true then throw an error
            LOG_ERROR("Semantic Analayzer", "Class name: '" + tempToken.lexeme + "' already exists!", filename, tempToken.lineNum);
        } else {
            //otherwise set the class name to the current identifier
            pclasstable->className = tempToken.lexeme;

            if(tempToken.lexeme == "Memory" || tempToken.lexeme == "Output" || tempToken.lexeme == "Array" || tempToken.lexeme == "Keyboard" || 
            tempToken.lexeme == "Math" || tempToken.lexeme == "String" || tempToken.lexeme == "Sys" || tempToken.lexeme == "Screen"){
                pclasstable->isJackOSAPI = true;

                //if this is true, do not make a vmFile
                vfile.isJackOS = true;
            }

            GlobalSymbolTable->classnames.push_back(tempToken.lexeme);

            vfile.filename = tempToken.lexeme;  

            classScopeSymbolTable->nameSymbolTable = "Class-Scope";

            //create a class-scope symbol table
            //and add to class object
            pclasstable->addSymbolTable(classScopeSymbolTable);
        }

    } else {
        //throw an error if there is no identifier
        LOG_ERROR("Parser", "Expected an identifier after class declaration" , filename, tempToken.lineNum);
        
    }

    //expecting an opening parantheses
    tempToken = localToken->GetNextToken();

    if(tempToken.lexeme == "{"){    
        LOG_SUCCESS("Parser", tempToken.lexeme, filename, tempToken.lineNum);   

    } else {        
        LOG_ERROR("Parser","Expected a class declaration", filename, tempToken.lineNum);

    }

    //check if the next token is a closing parantheses '}'
    tempToken = localToken->PeekNextToken();

    //if this is not a closing parantheses then we call the member declaration
    //while loop used for multiple declarations
    while(tempToken.lexeme != "}"){
        //call the rest of the parsing funcitions
        memberDeclaration(pclasstable->className);

        //peek to see if there is another declaration
        tempToken = localToken->PeekNextToken();
    } 
        
    //expecting a closing parantheses now
    if(tempToken.lexeme == "}"){
        //otherwise consume the token
        tempToken = localToken->GetNextToken();
        
        LOG_SUCCESS("Parser", tempToken.lexeme, filename, tempToken.lineNum);
    } else {
        //throw an error
        LOG_ERROR("Parser","Expected a '}'", filename, tempToken.lineNum);
    }

    //add the class to the global symbol tables
    GlobalSymbolTable->addClassSymbolTable(pclasstable);
}

//classVarDeclar | subroutineDeclar -> 
//(static | field) type identifier {, identifier} or 
//(constructor | function | method) (type|void) identifier (paramList) subroutineBody
void Parser::memberDeclaration(std::string symboltable_name){
    //create a local token
    Token token;

    //peek to see if this is a class var decl or subroutine declration
    token = localToken->PeekNextToken();

    if((token.lexeme == "static") || (token.lexeme == "field")){
        classVariableDeclaration(symboltable_name);

        //LOG_SUCCESS("Parser", token.lexeme, "COOL.jack", token.lineNum);

    } else if ((token.lexeme == "constructor") || (token.lexeme == "function")|| (token.lexeme == "method")) {
        subroutineDeclaration();

       // LOG_SUCCESS("Parser", token.lexeme, "COOL.jack", token.lineNum);

    //throw error if none appear
    } else {
        LOG_ERROR("Parser", "Expected a valid class variable declaration or a subroutine declaration", filename, token.lineNum);
    
    }
}

//add symbols to class scope symbol table
//classVarDec: ('static' | 'field') type varName (',' varName)* ';'
void Parser::classVariableDeclaration(std::string symboltable_name){
    //temporary token for checking
    Token token1;

    //current symbol to be added to the table
    Symbol current_symbol;

    //expecting static or feild
    token1 = localToken->GetNextToken();

    //encountered a potential symbol
    if(token1.lexeme == "static"){
        LOG_SUCCESS("Parser", token1.lexeme, filename, token1.lineNum);

        //set the symbol to static
        current_symbol.kind = SymbolKind::Static;

    } else if(token1.lexeme == "feild") {
        LOG_SUCCESS("Parser", token1.lexeme, filename, token1.lineNum);

        //set the symbol to feild
        current_symbol.kind = SymbolKind::Feild;

    //throw error if there are no class-scope declaration
    } else if(token1.lexeme != "static" && token1.lexeme != "feild"){
        //LOG_ERROR("Parser", "Expected a static or feild", "COOL.jack", token1.lineNum);
    
    }


    //peek to see the token type, to determine symbol type
    token1 = localToken->PeekNextToken();
    
    if(token1.type == TokenType::boolean || token1.type == TokenType::identifier || 
    token1.type == TokenType::integerConstant || token1.type == TokenType::charConstant){
        //get the string value of the type
        current_symbol.type = token1.lexeme;

        //set the initialization
        current_symbol.isInitialized = "false";
        
        //call the type function to consume the token
        type(); 
    } else {
        LOG_ERROR("Parser", "Expected a type", filename, token1.lineNum);

    }

    //expecting the first identifier
    token1 = localToken->GetNextToken();

    if(token1.type == TokenType::identifier){
        //success
        LOG_SUCCESS("Parser", token1.lexeme, filename, token1.lineNum);

        //this is a symbol
        //set the name to the lexeme
        current_symbol.symbolName = token1.lexeme;

        current_symbol.isInitialized = "false";

        //check if it already has been declared
        if(classScopeSymbolTable->findSymbol(token1.lexeme) == 1){
            LOG_ERROR("Semantic Analyzer", "This identifier has already been declared in this scope", filename, token1.lineNum);

        } else {
            //add to the symbol table
            classScopeSymbolTable->addSymbol(current_symbol); 
        }

    } else {
        LOG_ERROR("Parser", "Expected a identifier name", filename, token1.lineNum);

    }

    //see if this is another identifier or a ';'
    token1 = localToken->PeekNextToken();

    //accept either ',' or ';' as the next token
    //we could have more than two identifier definitions, hence the while loop
    while(token1.lexeme == ","){

        //consume the ','
        token1 = localToken->GetNextToken();

        //there should be an identifier now
        token1 = localToken->GetNextToken();

        //create a new symbol
        if(token1.type == TokenType::identifier){
            LOG_SUCCESS("Parser", token1.lexeme, filename, token1.lineNum);

            //create a new local symbol
            Symbol symbol_id2;

            //set the name to the lexeme
            symbol_id2.symbolName = token1.lexeme;
            
            //set the type
            symbol_id2.type = current_symbol.type;

            //set the kind to the current_symbol declared previously
            symbol_id2.kind = current_symbol.kind;

            //check if it exists and add to the class scope symbol table
            //addSymbol throws error if this symbol has already been declared
            if(classScopeSymbolTable->findSymbol(token1.lexeme) == 1){
                LOG_ERROR("Semantic Analyzer", "This identifier has already been declared in this scope", filename, token1.lineNum);

            } else {
                //add to the symbol table
                classScopeSymbolTable->addSymbol(symbol_id2); 
            }

        } else {
            //this is not an identifier, so throw an error
            LOG_ERROR("Parser", "Expected a variable name", filename, token1.lineNum);
        }

        //peek to see if there is another identifier declaration
        token1 = localToken->PeekNextToken();
    }

    //else a ';' is expected now
    token1 = localToken->GetNextToken();

    //call log function
    if(token1.lexeme == ";"){
        LOG_SUCCESS("Parser", token1.lexeme, filename, token1.lineNum);

    } else {
        LOG_ERROR("Parser", "Expected a ';'", filename, token1.lineNum);

    }
}

//subroutineDeclar -> (constructor | function | method) 
//(type|void) identifier (paramList) subroutineBody
void Parser::subroutineDeclaration(){ 
    std::string m_type;

    std::string symboltable_name;

    std::string symboltable_type;

    //create a locally declared symbol
    Symbol symbol;
    
    //create a local token
    Token token;

    bool isMethod = false;

    //expecting a constructor, function, method
    token = localToken->GetNextToken();

    //set the symbol kind in correspondance to the token
    if(token.lexeme == "constructor"){
        LOG_SUCCESS("Parser", token.lexeme, filename, token.lineNum);

        m_type = token.lexeme;

    } else if(token.lexeme == "function"){
        LOG_SUCCESS("Parser", token.lexeme, filename, token.lineNum);

        m_type = token.lexeme;
        
    } else if(token.lexeme == "method"){
        LOG_SUCCESS("Parser", token.lexeme, filename, token.lineNum);

        m_type = token.lexeme;

        isMethod = true;

    } else {
        //throw error if none of the above appear
        LOG_ERROR("Parser", "Expected a constructor, function, or method", filename, token.lineNum);
    }

    //peek to see if the next token is a type or void
    token = localToken->PeekNextToken();

    //if this is a void
    if(token.lexeme == "void"){
        symboltable_type = token.lexeme;

        //consume the 'void' token
        token = localToken->GetNextToken();

        LOG_SUCCESS("Parser", token.lexeme, filename, token.lineNum);
        
    //else this is a type
    } else if(token.type == TokenType::boolean || token.type == TokenType::identifier || 
    token.type == TokenType::integerConstant || token.type == TokenType::charConstant){                
        symboltable_type = token.lexeme;

        //call the type function to consume the token
        type();

    } else {
        LOG_ERROR("Parser", "Expected a void or type", filename, token.lineNum);
    }

    //expecting an identifier after void/type now
    token = localToken->GetNextToken();

    //std::cout << token.lexeme  <<" " << token.stringToken(token.type) << "\n";

    if(token.type == TokenType::identifier){
        LOG_SUCCESS("Parser", token.lexeme, filename, token.lineNum);
        
        symboltable_name = token.lexeme;

        //every method had an implicit first argument 
        //pointing to the object of this method
        symbol.symbolName = "this"; //NAME

        //set the kind of the object to Argument
        symbol.kind = SymbolKind::Argument;  //KIND

        //type is the current class name
        symbol.type = pclasstable->className; //TYPE
        if(m_type == "constructor"){
            generate->printComToFile(VMCommands::call, "Memory.alloc", "1");
            generate->printComToFile(VMCommands::pop, "pointer", "0");

        } else {
            generate->printComToFile(VMCommands::function, pclasstable->className + "." + symboltable_name, std::to_string((pclasstable->numTables++)));
            generate->printComToFile(VMCommands::push, "argument", "0");
            generate->printComToFile(VMCommands::pop, "pointer", "0");

        } 

        if(pclasstable->findSymbolTable(symboltable_name) == 1 && symboltable_name != "new"){
            LOG_ERROR("Semantic Analayzer", "'" + symboltable_name + "' has already been declared", filename, token.lineNum);

        } else{
            ///add the method scope to the class table, and the first object to the symbol table
            pclasstable->createMethodSymbolTable(symboltable_name, symboltable_type, m_type, pclasstable->className, pclasstable->numTables++);
            
            //method is compiled and operates on k+1 arguments which refers to this
            if(isMethod){
                //add the first object to the method scope symbol table
                pclasstable->accessSymbolTable(symboltable_name, symbol);
            }
        }

    } else {
        //throw error if there is no identifier
        LOG_ERROR("Parser", " Expected a subroutine name", filename, token.lineNum);
    }

    //expecting a '(' now
    token = localToken->GetNextToken();

    //log handling
    if(token.lexeme == "("){
        LOG_SUCCESS("Parser", token.lexeme, filename, token.lineNum);

    } else {
        LOG_ERROR("Parser", " Expected a '(' after subroutine declaration", filename, token.lineNum);
    }

    //expecting a paramlist or ')'; use peek to find out
    token = localToken->PeekNextToken();

    //keep calling the parameter list function
    if (token.lexeme != ")") {
        //this function will consume the necessary tokens
        parameterList(symboltable_name);
    }

    //now we expect a ')' after the paramter list
    token = localToken->GetNextToken();

    //log handling functions
    if(token.lexeme == ")"){
        LOG_SUCCESS("Parser", token.lexeme, filename, token.lineNum);

    } else {
        LOG_ERROR("Parser", "Expected a ')'", filename, token.lineNum);

    }

    //now we expect a subroutine body, starting with a '{'
    subroutineBody(symboltable_name);

}

//type -> int | char | boolean | identifier 
void Parser::type(){
    //create a local token object
    Token token;

    //consume the token knowing its a type
    token = localToken->GetNextToken();

    if(token.lexeme == "int"){
        LOG_SUCCESS("Parser", token.lexeme, filename, token.lineNum);

    } else if (token.lexeme == "char"){
        LOG_SUCCESS("Parser", token.lexeme, filename, token.lineNum);

    } else if (token.lexeme == "boolean"){
        LOG_SUCCESS("Parser", token.lexeme, filename, token.lineNum);

        //else this is an identifier name
    } else  if (token.type == TokenType::identifier){
        LOG_SUCCESS("Parser", token.lexeme, filename, token.lineNum);

    } else {
        //call error if conditions not met
        LOG_ERROR("Parser", "Expected a type", filename, token.lineNum);
    }

}

//subroutineBody -> { {statement} }
void Parser::subroutineBody(std::string scope_name){
    //declare a local object token
    Token token;

    //get the next token expecting a '{'
    token = localToken->GetNextToken();

    if(token.lexeme == "{"){
        LOG_SUCCESS("Parser", token.lexeme, filename, token.lineNum);

    } else {
        LOG_ERROR("Parser", " Expected a '{'", filename, token.lineNum);
    }

    //expecting statement(s) or '}'
    token = localToken->PeekNextToken();

    //keep checking for multiple statements
    while(token.lexeme != "}"){
        //statements() will consume the tokens
        statements(scope_name);

        //peek token to see if this is a '}'
        token = localToken->PeekNextToken();
    }

    //confirming that the method has a return statement
    //int symbol_table_offset = pclasstable->findSymbolTable[scope_name];
    if(pclasstable->hasReturnStatement(scope_name) || (pclasstable->isJackOSAPI)){
        LOG_SUCCESS("Parser", "function has return statement", filename, token.lineNum);

    } else {
        LOG_ERROR("Parser", "'" + scope_name + "' needs a return statement", filename, token.lineNum);

    }

    //expecting a '}' now
    token = localToken->GetNextToken();

    if(token.lexeme == "}"){
        LOG_SUCCESS("Parser", token.lexeme, filename, token.lineNum);

    } else {
        LOG_ERROR("Parser", " Expected a '}'", filename, token.lineNum);
    }
}

//statements -> varDeclarStatement | letStatemnt | ifStatement | whileStatement | 
//doStatement | returnStatemnt
void Parser::statements(std::string name){
    //create a local token object
    Token token;

    //peek the next token to see what statement this is
    token = localToken->PeekNextToken();
    
    //variable declaration statement
    if (token.lexeme == "var") {
       // LOG_SUCCESS("Parser", token.lexeme, "COOL.jack", token.lineNum);

		variableDeclarationStatement(name);

    //else this is a let statement
	} else if (token.lexeme ==  "let") {
       // LOG_SUCCESS("Parser", token.lexeme, "COOL.jack", token.lineNum);

		letStatements(name);

    //else this is an if statement
	} else if (token.lexeme == "if") {
       // LOG_SUCCESS("Parser", token.lexeme, "COOL.jack", token.lineNum);

		ifStatements(name);

    //else this is a while statement
	} else if (token.lexeme == "while") {
       // LOG_SUCCESS("Parser", token.lexeme, "COOL.jack", token.lineNum);

		whileStatement(name);

    //else this is a while statement
	} else if (token.lexeme == "do") { 
        //LOG_SUCCESS("Parser", token.lexeme, "COOL.jack", token.lineNum);

		doStatement(name);

    //else this is a return statement
	} else if (token.lexeme == "return") {    
      //  LOG_SUCCESS("Parser", token.lexeme, "COOL.jack", token.lineNum);

		returnStatement(name);

    //else this is not a statement
    } else {
        LOG_ERROR("Parser", "Expected statement", filename, token.lineNum);
    
    }
}

//Symbol: SYMBOLKIND(var)   TYPE   NAME(id)
//var type identifier { , identifier } ;
void Parser::variableDeclarationStatement(std::string scope_name){
    //create a local token object
    Token token;

    //create a local symbol
    Symbol symbol;

    std::string symbol_type;

    //expecting a 'var'
    token = localToken->GetNextToken();

    if(token.lexeme == "var"){
        LOG_SUCCESS("Parser", token.lexeme, filename, token.lineNum);
        
        //set the kind to var
        symbol.kind = SymbolKind::Local;

    } else {
        LOG_ERROR("Parser", "Expected a var declaration statement", filename, token.lineNum);
    
    }

    //expecting a type
    token = localToken->PeekNextToken();

    if(token.type == TokenType::identifier || token.lexeme == "int" || 
    token.lexeme == "boolean" || token.lexeme == "char"){
        LOG_SUCCESS("Parser", token.lexeme, filename, token.lineNum);
        
        //set the type of the symbol
        symbol.type = token.lexeme;
        symbol_type = token.lexeme;
        
        //call the type function to consume the token
        type();

    } else {
        LOG_ERROR("Parser", "Expected a valid type after variable declaration", filename, token.lineNum);
    }

    //expecting an identifier now
    token = localToken->GetNextToken();

    if(token.type == TokenType::identifier){
        LOG_SUCCESS("Parser", token.lexeme, filename, token.lineNum);

        //set the type of the symbol
        symbol.symbolName = token.lexeme;

        if(pclasstable->searchForSymbol(token.lexeme, scope_name) == 1){
            LOG_ERROR("Semantic Analayzer", "'" + token.lexeme + "' has already been declared", filename, token.lineNum);

        } else {
            //add the new symbol to the current scope symbol table
            pclasstable->accessSymbolTable(scope_name, symbol);
        }

    } else {
        LOG_ERROR("Parser", "Expected a type after variable declaration", filename, token.lineNum);
    
    } 

    //check to see if there is a ',' or a ';'
    token = localToken->PeekNextToken();

    Symbol current_symbol;

    while(token.lexeme == ","){
        //consume the ','
        token = localToken->GetNextToken();

        //we expect an identifier
        token = localToken->GetNextToken();

        //log handling
        if(token.type == TokenType::identifier){
            LOG_SUCCESS("Parser", token.lexeme, filename, token.lineNum);

            //type has already been determined
            current_symbol.type = symbol_type;

            //kind is determined
            current_symbol.kind = SymbolKind::Local;
            
            current_symbol.symbolName = token.lexeme;
            
            if(pclasstable->searchForSymbol(token.lexeme, scope_name) == 1){
                LOG_SUCCESS("Semantic Analayzer", token.lexeme, filename, token.lineNum);

            } else {

                //std::cout << "hello mate\n" << current_symbol.symbolName << scope_name;
                //add the new symbol to the current scope symbol table
                pclasstable->accessSymbolTable(scope_name, current_symbol);
            }
        
        } else {
            LOG_ERROR("Parser", "Expected a an identifier", filename, token.lineNum);

        }

        //check if the next token is a ','
        token = localToken->PeekNextToken();
    }

    //expecting a ';' now
    token = localToken->GetNextToken();

    if(token.lexeme == ";"){   
        LOG_SUCCESS("Parser", token.lexeme, filename, token.lineNum);

    } else {
        LOG_ERROR("Parser", "Expected a ';'", filename, token.lineNum);

    }
}

//letStatement: 'let' varName ('[' expression ']')? '=' expression ';' 
void Parser:: letStatements(std::string name){
    std::string id_name;

    std::string varType;

    std::string arrayCheck;

    //create a local token object
    Token token;

    //expecting 'let'
    token = localToken->GetNextToken();

    if(token.lexeme == "let"){
        LOG_SUCCESS("Parser", token.lexeme, filename, token.lineNum);

    } else {
        LOG_ERROR("Parser", "Expected a 'let' keyword", filename, token.lineNum);
    }

    //expecting an identifier now
    token = localToken->GetNextToken();

    if(token.type == TokenType::identifier){
        LOG_SUCCESS("Parser", token.lexeme, filename, token.lineNum);

        id_name = token.lexeme;

        //check if the identifier has been declared
        //enter the symbol name and scope
        if(pclasstable->searchForSymbol(token.lexeme, name) == 1){
            LOG_SUCCESS("Semantic Analayzer", token.lexeme + " has been declared in method", filename, token.lineNum);

            varType = pclasstable->returnSymbolType(token.lexeme, name);
        
        //if not found then search the entire class scope 
        } else if(pclasstable->searchForSymbol(token.lexeme, classScopeSymbolTable->nameSymbolTable)== 1){
            LOG_SUCCESS("Semantic Analayzer", token.lexeme + " has been declared in class", filename, token.lineNum);

            varType = pclasstable->returnSymbolType(token.lexeme, classScopeSymbolTable->nameSymbolTable);

        } else {
            LOG_ERROR("Semantic Analayzer", "'" + token.lexeme +"' has not be declared!", filename, token.lineNum);
        }

    } else {
        LOG_ERROR("Parser", "Expected an identifier", filename, token.lineNum);
    }

    //check if there is an expression ahead ie '[function]'
    token = localToken->PeekNextToken();

    bool isArray = false;

    if(token.lexeme == "["){
        isArray = true;

        //search for offset
        int offset= pclasstable->returnSymOffset(name, id_name);

        //in class
        int offs2 = pclasstable->returnSymOffset(classScopeSymbolTable->nameSymbolTable, id_name);
        
        std::string symKind = pclasstable->returnSymKind(name, id_name);

        std::string kindsym = pclasstable->returnSymKind(classScopeSymbolTable->nameSymbolTable, id_name);

        //std::cout << kindsym ;
        
        //found in current scope
        if(symKind == "null"){
            //push argument 0 e.g
            generate->printComToFile(VMCommands::push, kindsym, std::to_string(offset));

        //found in class scope
        } else if(kindsym == "null"){
            //push argument 0 e.g
            generate->printComToFile(VMCommands::push, symKind, std::to_string(offs2));

        }        
        
        LOG_SUCCESS("Parser", token.lexeme, filename, token.lineNum);

        //check type expressions on both sides (SEMANTIC)
        //consume the token
        token = localToken->GetNextToken();
        
        //this will generate a code -> push argument 0
        //call expression function
        arrayCheck = expression(name, id_name);

        if(arrayCheck == "int" || arrayCheck == "char"){
            LOG_SUCCESS("Parser", "array indices type compatible",filename, token.lineNum);

        } else {
            LOG_ERROR("Parser", "Expressions used as array indices must evaluate to an integer value", filename, token.lineNum);

        }

        //expecting a ']' now
        token = localToken->GetNextToken();

        if(token.lexeme == "]"){
            LOG_SUCCESS("Parser", token.lexeme, filename, token.lineNum);
        } else {
             LOG_ERROR("Parser", "Expected a ']'", filename, token.lineNum);
        
        }
    } 
    
    if(isArray){
        generate->printComToFile(VMCommands::add, "", "");
    }

    //expecting a '='
    token = localToken->GetNextToken();

    if(token.lexeme == "="){
        LOG_SUCCESS("Parser", token.lexeme, filename, token.lineNum);

    //throw an error
    } else {
        LOG_ERROR("Parser", "'" + token.lexeme + "' is invalid expected a '[' or '='",filename, token.lineNum);
    }
    
    std::string expr_type;

    if(isArray){
        //pop pointer 1 -> this is an array
        generate->printComToFile(VMCommands::pop, "pointer", "1");
    }

    //expecting a type now
    //check for type
    expr_type = expression(name, id_name);

    if(isArray){
        generate->printComToFile(VMCommands::pop, "that", "0");

    } else {
        //search offset in both tables
        int offset = pclasstable->returnSymOffset(name, id_name);

        //class scope
        int off = pclasstable->returnSymOffset(classScopeSymbolTable->nameSymbolTable, id_name);

        if(offset == -1){
            generate->printComToFile(VMCommands::pop, "this", std::to_string(off));

        } else if(off == -1){
            generate->printComToFile(VMCommands::pop, "this", std::to_string(offset));

        }
    }

    //later check for compatability
    std::string mtype = pclasstable->methodType(name);

    //initialize the variable
    if(mtype == "constructor"){
        pclasstable->symbolInitialize(id_name, classScopeSymbolTable->nameSymbolTable);
    } else{
        pclasstable->symbolInitialize(id_name, name);
    }
      
    //must be if 'e.g char c = 82' or same type
    //if method name is returned check if it has been declared
    //later check for type compatibility
    bool isFunc = false;

    if(expr_type != "char" && expr_type != "boolean" && expr_type != "int"){
        //means this is a function name
        isFunc = true;

        if(isArray){
            varType = "int";
        }

        GlobalSymbolTable->statcompatibility.push_back({varType, expr_type,filename, token.lineNum});
    }

    if(expr_type == varType || (varType == "char" && expr_type == "int") || checkSubNames(expr_type) || (isFunc) || (isArray && expr_type == "int")){
        LOG_SUCCESS("Semantic Analayzer", token.lexeme, filename, token.lineNum);
 
    } else {
        LOG_ERROR("Semantic Analayzer", "'let' statement contains incompatible types", filename, token.lineNum);

    }

    //expecting a ';' now
    token = localToken->GetNextToken();

    if(token.lexeme == ";"){
        LOG_SUCCESS("Parser", token.lexeme, filename, token.lineNum);

    } else {
        LOG_ERROR("Parser", "Expected a ';'", "sd", token.lineNum);

    }
}

//expression -> relationalExpression { ( & | | ) relationalExpression }
std::string Parser::expression(std::string scope_name, const std::string& letstatem){
    std::string expr_type;

    //local token object
    Token token;

    //call the relational expression function
    expr_type = relationalExpression(scope_name, letstatem);

    //check for '&' or '|'
    token = localToken->PeekNextToken();

    //since there can be multiple relational expression, use while loop
    while(token.lexeme == "&"|| token.lexeme == "|"){
        //consume the symbol token
        token = localToken->GetNextToken();
        
        //log handle
        LOG_SUCCESS("Parser", token.lexeme, filename, token.lineNum);

        //expecting another relational expression now
        relationalExpression(scope_name , letstatem);

        if(token.lexeme == "&"){
            generate->printComToFile(VMCommands::AND, "", "");

        } else if (token.lexeme == "|"){
            generate->printComToFile(VMCommands::OR, "", "");
        }

        //peek to see if there is another relational expression
        token = localToken->PeekNextToken();

        //if there are no more factors, then this is the end
        if(token.lexeme != "&" && token.lexeme != "|"){
            //this should be an 'int'
            return expr_type;
        }
    }

    return expr_type;
}

//relationalExpression -> ArithmeticExpression { ( = | > | < ) ArithmeticExpression }
std::string Parser::relationalExpression(std::string scope_name, std::string let){
    std::string arithm_type;

    std::string op;

    //local token
    Token token;

    //expecting arithmetic expression now
    arithm_type = arithmeticExpression(scope_name, let);

    //check to see if there is an operator
    token = localToken->PeekNextToken();

    //multiple arithmetic expression check
    while(token.lexeme== "=" || token.lexeme == ">" || token.lexeme == "<"){
        std::string arithm_n;

        op = token.lexeme;

        //log handling
        LOG_SUCCESS("Parser", token.lexeme, filename,token.lineNum);

        //consume the symbol token
        token = localToken->GetNextToken();

        //call the function
        //this will generate the code for the expression
        arithm_n = arithmeticExpression(scope_name, let);

        //generate after both expressions have been pushed
        if(op == "="){
            generate->printComToFile(VMCommands::eq, "", "");

        } else if(op == ">"){
            generate->printComToFile(VMCommands::gt, "", "");

        } else if(op == "<"){
            generate->printComToFile(VMCommands::lt, "", "");

        }

        if(arithm_type == "int" && arithm_n == "int" || 
        (arithm_type == "int" && arithm_n == "char") || (arithm_type == "char" && arithm_n == "int")){
            LOG_SUCCESS("Semantic Analayzer", "compatible",filename ,token.lineNum);
        } else {
            LOG_ERROR("Semantic Analayzer", "Incompatible types: '=', '>', '<' needs two integer values", filename, token.lineNum);
        }

        //peek to see if there is another operator
        token = localToken->PeekNextToken();

        //if there are no more factors, then this is the end
        if(token.lexeme != "=" && token.lexeme != "<" && token.lexeme != ">"){
            //this should be an 'int'
            return arithm_type;
        }
    }

    return arithm_type;
}

//arithemeticExpression -> term { ( + | - ) term }
std::string Parser::arithmeticExpression(std::string scope_name, std::string let){
    std::string term_1;
    //local token object to call reveal the lexer
    Token token;

    std::string op;

    std::string term_n;

    bool isMethod = false;

    //expecting a term now
    //errors reported from within the function
    //get the type
    term_1 = term(scope_name, let);
    
    //POTENTIAL ERROR
    //this is a method so check for compatibility later
    if(term_1 != "int" && term_1 != "char" && term_1 != "boolean"){
        isMethod = true;

        //GlobalSymbolTable->statcompatibility.push_back({term_n, term_1, "file", token.lineNum});
    }

    //peek to see if there is an operator
    token = localToken->PeekNextToken();

    //call multiple terms
    while(token.lexeme == "+" || token.lexeme == "-"){
        op = token.lexeme;

        //consume the symbol token
        localToken->GetNextToken();

        //log handling
        LOG_SUCCESS("Parser", token.lexeme, filename ,token.lineNum);
      
        //expecting another term now
        term_n = term(scope_name, let);

        //generate code after expressions generated
        if(op == "+"){
            generate->printComToFile(VMCommands::add, "", "");

        } else if(op =="-"){
            generate->printComToFile(VMCommands::sub, "", "");

        }

        //this is a method so check for compatibility later
        if(term_n != "int" && term_n != "char" && term_n != "boolean"){
            isMethod = true;

            GlobalSymbolTable->statcompatibility.push_back({term_1, term_n, "file", token.lineNum});
        }

        if(term_1 == "int" && term_n == "int" || (isMethod)){
            LOG_SUCCESS("Semantic Analayzer", "compatible", filename ,token.lineNum);
        } else {
            LOG_ERROR("Semantic Analayzer", "Incompatible types: need to add/subtract two integer values", filename, token.lineNum);
        
        }

        //peek to see if there is another term
        token = localToken->PeekNextToken();

        //if there are no more factors, then this is the end
        if(token.lexeme != "+" && token.lexeme != "-"){
            //this should be an 'int'
            return term_1;
        }
    }

    //if there is only one term, then return its type
    return term_1;
}

//term -> factor { ( * | / ) factor }
std::string Parser::term(std::string scope_name, std::string let){
    std::string firstFactor = "";

    std::string op1;
   
    std::string secondFactor = "";

    bool isMethod = false;

    //local token object
    Token token;

    //expecting a factor now
    firstFactor = factor(scope_name, let);

    //POTENTIAL ERROR
    //this is a method so check for compatibility later
    if(firstFactor != "int" && firstFactor != "char" && firstFactor != "boolean"){
        isMethod = true;

       // GlobalSymbolTable->statcompatibility.push_back({secondFactor, firstFactor, "file", token.lineNum});
    }

    //peek to see if there is another factor now
    token = localToken->PeekNextToken();

    //call multiple terms
    while(token.lexeme == "*" || token.lexeme== "/"){
        //consume the (*||/) token
        token = localToken->GetNextToken();

        op1 = token.lexeme;

        LOG_SUCCESS("Parser", token.lexeme, filename,token.lineNum);

        //store the type to a local temp string
        secondFactor = factor(scope_name, let);

        //this is a method so check for compatibility later
        if(secondFactor != "int" && secondFactor != "char" && secondFactor != "boolean"){
            isMethod = true;

            GlobalSymbolTable->statcompatibility.push_back({firstFactor, secondFactor, filename, token.lineNum});
        }

       // std::cout << secondFactor << firstFactor;

        //determine the compatability
        if((firstFactor == secondFactor) || (isMethod)){
            LOG_SUCCESS("Semantic Analayzer", "Multiplication/Divide", filename,token.lineNum);

        } else {
            LOG_ERROR("Semantic Analayzer" , "Only two integer values can be multiplied/divided", filename, token.lineNum);
            
            break;
        }
        
        //log handling
        LOG_SUCCESS("Parser", token.lexeme, filename,token.lineNum);

        //peek to see if there is another '*' or '/'
        token = localToken->PeekNextToken(); 

        if(op1 == "*"){
            generate->printComToFile(VMCommands::call, "Math.multiply", "2");

        } else if(op1 == "/"){
            generate->printComToFile(VMCommands::call, "Math.divide", "2");

        }

        //if there are no more factors, then this is the end
        if(token.lexeme == ")"){
            //this should be an 'int'
        
            return firstFactor;
            
            break;
        }

        secondFactor.clear();
    }

    //if there is only one factor, then return its type
    return firstFactor;
}

//factor -> ( - | ~ | ε ) operand
std::string Parser::factor(std::string scope_name, std::string let){
    //local token object
    Token token;
    
    //check if there is a symbol -> '-' or  '~' (negate), or no string (epsilon)
    token = localToken->PeekNextToken();

    bool neg;

    if(token.lexeme == "-"){
        //consume the symbol
        token = localToken->GetNextToken();

        generate->printComToFile(VMCommands::NOT, "", "");

        LOG_SUCCESS("Parser", token.lexeme, filename,token.lineNum);
        
    } else if(token.lexeme == "~") {
        //consume the symbol
        token = localToken->GetNextToken();

        neg = true;

        LOG_SUCCESS("Parser", token.lexeme, filename,token.lineNum);
    }

    //unconditionally call operand function
    std::string type_value = operand(scope_name, let);

    //generate negate after call for operand
    if(neg){
        generate->printComToFile(VMCommands::neg, "", "");
    }

    return type_value;
}

//operand -> integerConstant | identifier [.identifier ]
// [ [ expression ] | (expressionList ) ] | (expression) | stringLiteral | true | false | null | this
std::string Parser::operand(std::string scope_name, std::string let) {
    //local token object to reveal the lexer
    Token token;

    std::string firstId = "";

    std::string firstId_type;

    //peek to determine the operand
    token = localToken->PeekNextToken();

    if(token.type == TokenType::integerConstant){
        LOG_SUCCESS("Parser", token.lexeme, filename,token.lineNum);

        //consume the integer constant
        token = localToken->GetNextToken();

        generate->printComToFile(VMCommands::push, "constant", token.lexeme);

        return "int";

    } else if (token.type == TokenType::identifier){
        //if operand is only an identifer -> variable -> check if it exists, return type
        firstId = token.lexeme;

        std::string secondId;

        std::string firstId_type;

        std::string exprtype;

        bool isThis;

        LOG_SUCCESS("Parser", token.lexeme, filename,token.lineNum);
        
        //x.mult(5)
        //push x
        //generate->printComToFile(VMCommands::push, firstId, std::to_string(pclasstable->returnSymOffset(scope_name, firstId)));
        
        //consume the identifier
        token = localToken->GetNextToken();

        //peek next token to see if there is a '.' for [.identifier]
        token = localToken->PeekNextToken();

            //this is an .identifier
            if(token.lexeme == "."){
                //this means that the first identifier is a className, Keyboard, Array, Screen etc.
                
                //consume the '.'
                token = localToken->GetNextToken();

                LOG_SUCCESS("Parser", token.lexeme, filename,token.lineNum);
                
                //expecting an identifier -> method within the class name
                token = localToken->GetNextToken();

                if(token.type == TokenType::identifier){
                    LOG_SUCCESS("Parser", token.lexeme,filename,token.lineNum);

                    secondId = token.lexeme;

                    std::string test1;
                    std::string test2;
                    
                    //firstId_type is used to find the symbol type incase it is a user defined variable
                    if(pclasstable->returnSymbolType(firstId, classScopeSymbolTable->nameSymbolTable) == "null"){
                        firstId_type = pclasstable->returnSymbolType(firstId, scope_name);
                        
                    } else if(pclasstable->returnSymbolType(firstId, scope_name) == "null"){
                        firstId_type = pclasstable->returnSymbolType(firstId, classScopeSymbolTable->nameSymbolTable);
                        
                    }

                    //check later if the subroutine and class exists
                    GlobalSymbolTable->classmethodarray.push_back({firstId, secondId, token.lineNum, filename, firstId_type});

                } else {
                    LOG_ERROR("Parser", "Expected an identifier", filename,token.lineNum);
                
                }

                //now determine if there is an expression or expressionlist or epsilon
                token = localToken->PeekNextToken();

                //identifier.identifier[expression]
                //API
                if(token.lexeme == "["){
                    LOG_SUCCESS("Parser", token.lexeme,filename,token.lineNum);

                    //consume the '[' token
                    token = localToken->GetNextToken();

                    //expecting an expression now
                    exprtype = expression(scope_name, let);

                    //expecting a ']' now
                    token = localToken->GetNextToken();

                    if(token.lexeme == "]"){
                        LOG_SUCCESS("Parser", token.lexeme, filename,token.lineNum);
                    } else {
                        LOG_ERROR("Parser", "Expected a ']'", filename,token.lineNum);
                    }

                    return exprtype;

                //push 5
                //else this is identifier.identifier(expressionList)
                } else if(token.lexeme == "("){
        
                    LOG_SUCCESS("Parser", token.lexeme, filename,token.lineNum);

                    //consume the '(' token
                    token = localToken->GetNextToken();

                    //expecting an expressionList now
                    //check later if the expression list matches the args list
                    expressionList(firstId,secondId, scope_name, let);

                    //expecting a ')' now
                    token = localToken->GetNextToken();

                    //std::cout << token.lexeme;

                    if(token.lexeme == ")"){
                        LOG_SUCCESS("Parser", token.lexeme, filename,token.lineNum);

                    } else {
                        LOG_ERROR("Parser", "Expected a ']'", filename,token.lineNum);

                    }

                    //check later if method is inside the specified class
                    //if this is other.getDenominator() etc. check for compatibility later
                    if(pclasstable->searchForSymbol(firstId, scope_name) == 1){
                        LOG_SUCCESS("Semantic Analayzer", firstId + " has been declared", filename, token.lineNum);

                        firstId_type = pclasstable->returnSymbolType(firstId, scope_name);
                    
                        //GlobalSymbolTable->classmethodarray.push_back({firstId_type, secondId, token.lineNum,filename, pclasstable->className});

                    //if not found then search the entire class scope 
                    } else if(classScopeSymbolTable->findSymbol(firstId) == 1){
                        LOG_SUCCESS("Semantic Analayzer", firstId + " has been declared", filename, token.lineNum);
                        
                        firstId_type = pclasstable->returnSymbolType(firstId, classScopeSymbolTable->nameSymbolTable);

                    }

                    if(firstId_type != "null"){
                        GlobalSymbolTable->classmethodarray.push_back({firstId_type, secondId, token.lineNum, filename , pclasstable->className});

                    }

                    //if this is a declaration or declaration
                    if(secondId == "new"){
                        //bar = new int(n)
                        //bar = Memory.alloc(n)
                        generate->printComToFile(VMCommands::call, "Memory.alloc(1)", "");

                    } else {
                        //call mult after pushing
                        generate->printComToFile(VMCommands::call, secondId, "");
                    }
     
                    //return the method name in the expression, and later check the type compatibilty
                    return secondId;
                }

             //else this is a (expressionList) or (expression)
            } else if(token.lexeme == "["){
                //consume the '[' token
                token = localToken->GetNextToken();

                //expecting an expression now
                exprtype = expression(scope_name, let);

                //expecting a ']' now
                token = localToken->GetNextToken();

                if(token.lexeme == "]"){
                    LOG_SUCCESS("Parser", token.lexeme, filename,token.lineNum);

                } else {
                    LOG_ERROR("Parser", "Expected a ']'", filename,token.lineNum);
                }

                return exprtype;

            } else if(token.lexeme == "("){
                //consume the '(' token
                token = localToken->GetNextToken();

                //expecting an expression now
                //must be a method within the current class
                expressionList(pclasstable->className,firstId, scope_name, let);

                //expecting a ')' now
                token = localToken->GetNextToken();

                if(token.lexeme == ")"){
                    LOG_SUCCESS("Parser", token.lexeme, filename,token.lineNum);

                } else {
                    LOG_ERROR("Parser", "Expected a ']'", filename,token.lineNum);

                }

                /*
                * According to Chapter 11 from the Nand2Tetris course
                * a jack subroutine in a class is compiled as class.sub 
                */
                generate->printComToFile(VMCommands::call, pclasstable->className + "." + firstId, "");

            //else this is just an identifier, so check if it has been declared
            } else {
                if(pclasstable->searchForSymbol(firstId, scope_name) == 1){
                    LOG_SUCCESS("Semantic Analayzer", firstId + " has been declared", filename, token.lineNum);

                    firstId_type = pclasstable->returnSymbolType(firstId, scope_name);

                //if not found then search the entire class scope 
                } else if(classScopeSymbolTable->findSymbol(firstId) == 1){
                    LOG_SUCCESS("Semantic Analayzer", firstId + " has been declared", filename, token.lineNum);
                    
                    firstId_type = pclasstable->returnSymbolType(firstId, classScopeSymbolTable->nameSymbolTable);
                } else {
                    LOG_ERROR("Semantic Analayzer", "'" + firstId +"' has not been declared", filename, token.lineNum);
                }

                //check if the variable has been initialized in the class and current scope 
                std::string checkCurrentScope = pclasstable->initSymbol(firstId, scope_name);
                std::string checkClassScope = pclasstable->initSymbol(firstId, classScopeSymbolTable->nameSymbolTable);

                //let == firstId ensures that let sum = sum + ... is valid in which sum is declared
                if(checkClassScope == "true" || checkCurrentScope == "true" || (let == firstId)){
                    LOG_SUCCESS("Semantic Analayzer", firstId + " has been initialized", filename, token.lineNum);

                } else {
                    LOG_ERROR("Semantic Analayzer", "'" + firstId +"' has not been initialized", filename, token.lineNum);

                }
                
                //search for offset
                int offset= pclasstable->returnSymOffset(scope_name, firstId);

                //in class
                int offs2 = pclasstable->returnSymOffset(classScopeSymbolTable->nameSymbolTable, firstId);

                //if this is the same name as the varname from let statement
                if(let == firstId){
                    if(offset == -1){
                        generate->printComToFile(VMCommands::push, "this", std::to_string(offs2));

                    } else if(offs2 == -1) {
                        generate->printComToFile(VMCommands::push, "this", std::to_string(offset));
                    }
                } else if(let != firstId){
                    std::string symKind = pclasstable->returnSymKind(scope_name, firstId);

                    std::string kindsym = pclasstable->returnSymKind(classScopeSymbolTable->nameSymbolTable, firstId);

                    //found in current scope
                    if(symKind != "null"){
                        //push argument 0 e.g
                        generate->printComToFile(VMCommands::push, symKind, std::to_string(offset));

                    //found in class scope
                    } else if(kindsym != "null"){
                        //push argument 0 e.g
                        generate->printComToFile(VMCommands::push, kindsym, std::to_string(offs2));

                    }

                    return firstId_type;
                }
                
                return firstId_type;  
            }
    //else if this is a string literal
    } else if(token.type == TokenType::stringLiteral){
        LOG_SUCCESS("Parser", token.lexeme, filename, token.lineNum);

        for(char& it : token.lexeme){
            generate->printComToFile(VMCommands::push, "constant" , std::to_string(it));
            generate->printComToFile(VMCommands::push, "String.appendChar" , "2");
        }

        //consume the token
        token = localToken->GetNextToken();

        generate->printComToFile(VMCommands::push, "constant", std::to_string(token.lexeme.length()));
        generate->printComToFile(VMCommands::push, "String.new", "1");

        return "String";

    //if this is a true 
    } else if(token.lexeme == "char"){
         LOG_SUCCESS("Parser", token.lexeme, filename, token.lineNum);

        //consume the token
        token = localToken->GetNextToken();

        return "char";
    } else if (token.lexeme == "true"){
        LOG_SUCCESS("Parser", token.lexeme, filename, token.lineNum);

        //consume the 'true' token
        token = localToken->GetNextToken();

        generate->printComToFile(VMCommands::push, "constant", "1");

        return "boolean";

    //if this is false
    } else if (token.lexeme == "false"){
        LOG_SUCCESS("Parser", token.lexeme, filename, token.lineNum);

        //consume 'false' keyword
        token = localToken->GetNextToken();

        generate->printComToFile(VMCommands::push, "constant", "0");

        return "boolean";
    
    //if this is null
    } else if (token.lexeme == "null"){
        LOG_SUCCESS("Parser", token.lexeme, filename, token.lineNum);

        //consume the 'null' keyword
        token = localToken->GetNextToken();
        
        generate->printComToFile(VMCommands::push, "constant", "0");

    //if this is a 'this' keyword
    } else if (token.lexeme == "this"){
        LOG_SUCCESS("Parser", token.lexeme, filename, token.lineNum);

        //consume the token
        token = localToken->GetNextToken();

        generate->printComToFile(VMCommands::push, "pointer", "0");

        //this referes to the current class name
        return pclasstable->className;

    } else if(token.lexeme == "("){
        std::string expression_type;

        LOG_SUCCESS("Parser", token.lexeme, filename, token.lineNum);

        //consume the '('
        token = localToken->GetNextToken();

        expression_type = expression(scope_name, let);

        token = localToken->GetNextToken();

        //expecting a ')'
        if(token.lexeme == ")"){
            LOG_SUCCESS("Parser", token.lexeme, filename, token.lineNum);

        } else {
            LOG_ERROR("Parser", "Expected a ')'", filename, token.lineNum);
        }

        return expression_type;

    //throw an error if none of the above exists
    } else {
        LOG_ERROR("Parser", "Expected an operand",filename, token.lineNum);

    }
}

void Parser::ifStatements(std::string name){
    //create a local token object
    Token token;

    //expecting 'if'
    token = localToken->GetNextToken();

    //log handling
    if(token.lexeme == "if"){
        LOG_SUCCESS("Parser", token.lexeme, filename, token.lineNum);
    
    } else {
        LOG_ERROR("Parser", "Expected an 'if'", filename, token.lineNum);
    }

    //expecting a '('
    token = localToken->GetNextToken();

    //log handling
    if(token.lexeme == "("){
        LOG_SUCCESS("Parser", token.lexeme, filename, token.lineNum);

    } else { 
        LOG_ERROR("Parser", " Expected a '('", filename, token.lineNum);
    } 

    //unconditional call for expression
    expression(name, "");

    //expecting a ')' now
    token = localToken->GetNextToken();

    //log handling
    if(token.lexeme == ")"){
        LOG_SUCCESS("Parser", token.lexeme, filename, token.lineNum);

    } else {
        LOG_ERROR("Parser", "Expected a ')'", filename, token.lineNum);

    }

    //expecting a '{'
    token = localToken->GetNextToken();

    generate->printComToFile(VMCommands::NOT, "", "");

    generate->printComToFile(VMCommands::IFGOTO, "", "");

    //log handling
    if(token.lexeme == "{"){
        LOG_SUCCESS("Parser", token.lexeme, filename, token.lineNum);

    } else { 
        LOG_ERROR("Parser", "Expected a '{'", filename, token.lineNum);
    }

    //check to see if the next token is an expression or '}'
    token = localToken->PeekNextToken();

    //call multiple expressions if there is any
    while (token.lexeme != "}"){
        LOG_SUCCESS("Parser", token.lexeme, filename, token.lineNum);

        //this will consume the tokens
        statements(name);

        //peek to check if this is a '}'
        token = localToken->PeekNextToken();
    } 

    //we have encountered a '}'
    token = localToken->GetNextToken();

    if(token.lexeme == "}"){
        LOG_SUCCESS("Parser", token.lexeme, filename, token.lineNum);

    } else {
        LOG_ERROR("Parser", "Expected a '}'", filename,token.lineNum);
    }

    generate->printComToFile(VMCommands::GOTO, "", "");

    //peek to see if there is an 'else; statement as well
    token = localToken->PeekNextToken();

    if(token.lexeme == "else"){
        //consume the 'else' token
        token = localToken->GetNextToken();

        //we are expecting a '{' now
        token = localToken->GetNextToken();

        //log handling
        if(token.lexeme == "{"){
            LOG_SUCCESS("Parser", token.lexeme, filename,token.lineNum);

        } else {
            LOG_ERROR("Parser", "Expected a '{'", filename,token.lineNum);
        }

        //check to see if the next token is an expression or '}'
        token = localToken->PeekNextToken();

        //call multiple expressions if there is any
        while (token.lexeme != "}"){
            LOG_SUCCESS("Parser", token.lexeme, filename, token.lineNum);

            //this will consume the tokens
            statements(name);

            //peek to check if this is a '}'
            token = localToken->PeekNextToken();
        } 

        //we have encountered a '}'
        token = localToken->GetNextToken();

        if(token.lexeme == "}"){
            LOG_SUCCESS("Parser", token.lexeme, filename, token.lineNum);

        } else {
            LOG_ERROR("Parser", "Expected a '}'", filename,token.lineNum);
        }
    }
}
//whilestatement -> while ( expression ) { {statement} }
void Parser::whileStatement(std::string name){
    //create a local token object
    Token token;

    //expecting a while statement
    token = localToken->GetNextToken();

    //log handling
    if(token.lexeme == "while"){
        LOG_SUCCESS("Parser", token.lexeme, filename,token.lineNum);
    
    } else {
        LOG_ERROR("Parser", "Expected an 'if' keyword", filename,token.lineNum);
    }

    //generate label loop as start of while loop
    generate->printComToFile(VMCommands::LABEL, "loop", "");

    //expecting a '('
    token = localToken->GetNextToken();

    if(token.lexeme == "("){
        LOG_SUCCESS("Parser", token.lexeme, filename,token.lineNum);

    } else {
        LOG_ERROR("Parser", "Expected a '('", filename,token.lineNum);
    } 

    //unconditionally call the expression
    expression(name, "");

    //expecting a ')'
    token = localToken->GetNextToken();

    //log handling
    if(token.lexeme == ")"){
        LOG_SUCCESS("Parser", token.lexeme, filename,token.lineNum);

    } else {
        LOG_ERROR("Parser", "Expected a ')'", filename,token.lineNum);

    }

    //goto loop
    generate->printComToFile(VMCommands::IFGOTO, "end", "");

    //label end
    //generate->printComToFile(VMCommands::IFGOTO, "end", "");

    //expecting a '{'
    token = localToken->GetNextToken();

    //log handling
    if(token.lexeme == "{"){
        LOG_SUCCESS("Parser", token.lexeme, filename,token.lineNum);

    } else { 
        LOG_ERROR("Parser", "Expected a '{'", filename,token.lineNum);
    }

    //peek to check whether we have an expression or '}'
    token = localToken->PeekNextToken();

    //can call multiple statements
    while (token.lexeme != "}"){
        //consumes the necassary tokens
        statements(name);

        LOG_SUCCESS("Parser", token.lexeme, filename,token.lineNum);

        //peek to see if there is another '}'
        token = localToken->PeekNextToken();
    } 
    
    //we are expecting a '}' now
    token = localToken->GetNextToken();

    //log handling
    if(token.lexeme == "}"){
        LOG_SUCCESS("Parser", token.lexeme, filename,token.lineNum);

    } else {
        LOG_ERROR("Parser", "Expected a '}'", filename,token.lineNum);
    }

    generate->printComToFile(VMCommands::GOTO, "LOOP", "");

    generate->printComToFile(VMCommands::LABEL, "end", "");
}

//doStatement -> do subroutineCall;
//subroutine can be declared at later stages
void Parser::doStatement(std::string name){
    //create a local token object
    Token token;

    //expecting a 'do' statement
    token = localToken->GetNextToken();

    //log handling
    if(token.lexeme == "do"){
        LOG_SUCCESS("Parser", token.lexeme, filename,token.lineNum);

    } else {
        LOG_ERROR("Parser", "Expected a 'do' keyword", filename, token.lineNum);
    }

    //make sure that subroutine has be declared
    token = localToken->PeekNextToken();

    subroutineNames.push_back({token.lexeme, token.lineNum});

    //unconditionally call the subroutine
    subroutineCall(name);
    
    token = localToken->GetNextToken();

    if(token.lexeme == ";"){
        LOG_SUCCESS("Parser", token.lexeme, filename,token.lineNum);

    } else {   
        LOG_ERROR("Parser", "Expected a ';'", filename,token.lineNum);

    }

    generate->printComToFile(VMCommands::pop, "pointer", "0");
}

//subroutinecall -> identifier [ . identifier ] ( expressionList )
void Parser::subroutineCall(std::string name){
    
    std::string firstId;

    std::string secondid;

    std::string firstId_type;

    int numTok = 0;

    //create a local token
    Token token;

    //expecting an identifier now
    token = localToken->GetNextToken();

    //log handling
    if(token.type == TokenType::identifier){
        firstId = token.lexeme;
         
         numTok++;

        LOG_SUCCESS("Parser", token.lexeme, filename,token.lineNum);

    } else {
        LOG_ERROR("Parser", "Expected an identifier for the subroutine call", filename, token.lineNum);

    }

    //peek to see if there is a '.'
    token = localToken->PeekNextToken();

    if(token.lexeme == "."){
        //this means that the first identifier is a class name
        //consume the token
        token = localToken->GetNextToken();
        token = localToken->GetNextToken();

        //expecting an identifier now
        if(token.type == TokenType::identifier){
            secondid = token.lexeme;

            numTok++;

            LOG_SUCCESS("Parser", token.lexeme, filename, token.lineNum);

        } else {
            LOG_ERROR("Parser", "Expected an identifier", filename, token.lineNum);

        }

        //this is a constructed object
        if(secondid == "new"){
            generate->printComToFile(VMCommands::call, "Memory.alloc", "1");
            generate->printComToFile(VMCommands::call, "pointer", "0");

        //call function
        } else if(secondid != "new"){
            //this means the first id is the class name and the second is a subroutine
            generate->printComToFile(VMCommands::call, firstId + "." + secondid, "");
        }
        
        //firstId_type is used to find the symbol type incase it is a user defined variable
        if(pclasstable->returnSymbolType(firstId, classScopeSymbolTable->nameSymbolTable) == "null"){
            firstId_type = pclasstable->returnSymbolType(firstId, name);
    
        } else if(pclasstable->returnSymbolType(firstId, name) == "null"){
            firstId_type = pclasstable->returnSymbolType(firstId, classScopeSymbolTable->nameSymbolTable);
                        
        }

        //we need to check if the method and class exists later
        //enter both the firstid and its type and check later if it is user defined or class name   
        GlobalSymbolTable->classmethodarray.push_back({firstId, secondid, token.lineNum, filename, firstId_type});

        //expecting a '(' now
        token = localToken->GetNextToken();

        if(token.lexeme == "("){
            LOG_SUCCESS("Parser", token.lexeme, filename,token.lineNum);

        } else {
            LOG_ERROR("Parser", "Expected a '('", filename,token.lineNum);

        }

        //unconditionally call the expression list
        expressionList(firstId, secondid, name , "");

        //expecting a ')'
        token = localToken->GetNextToken();

        //log handling
        if(token.lexeme == ")"){
            LOG_SUCCESS("Parser", token.lexeme, filename,token.lineNum);

        } else {
            LOG_ERROR("Parser", "Expected a ')'", filename,token.lineNum);

        }
    //expecting an expression list
    } else if(token.lexeme == "(") {
        //consume the '('
        token = localToken->GetNextToken();

        //method should be in the current class
        expressionList(pclasstable->className, firstId, name, "");

        //expecting a ')'
        token = localToken->GetNextToken();

        //log handling
        if(token.lexeme == ")"){
            LOG_SUCCESS("Parser", token.lexeme, filename,token.lineNum);

        } else {
            LOG_ERROR("Parser", "Expected a ')'", filename,token.lineNum);

        }
    //this is just a funciton
    } else if (numTok == 1){
        generate->printComToFile(VMCommands::call, pclasstable->className + "." + firstId, "");

    }
}

//expressionList -> expression { , expression } | ε
void Parser::expressionList(std::string class_name, std::string method_scope, std::string currentclass, std::string let){
    std::vector<std::string> exprtypes;
    //exprtypes.clear();

    std::string exprtype_n;

    Token token;

    //peek to see if this is an expression or an epsilon
    token = localToken->PeekNextToken();

    //if we encounter epsilon then break function
    if(token.lexeme == ")"){
        LOG_SUCCESS("Parser", token.lexeme, filename,token.lineNum);
        
        //array is 0
        GlobalSymbolTable->exprlist_array.push_back({exprtypes, class_name, method_scope, token.lineNum, filename});

        return;

    //we encountered an expression
    } else if(token.lexeme != ")"){  
        //returns a type
        exprtype_n = expression(currentclass, let);
        
        //add to the expression list vector
        exprtypes.push_back(exprtype_n);

        //peek to see if we have another expression
        token = localToken->PeekNextToken();
       
        while(token.lexeme == ","){
            //consume the ',' token
            token = localToken->GetNextToken();

            LOG_SUCCESS("Parser", token.lexeme, filename,token.lineNum);

            //expecting another expression
            //return type of that expression
            exprtype_n = expression(currentclass, let);

            exprtypes.push_back(exprtype_n);

            //peek to see if we have another ','
            token = localToken->PeekNextToken();
        }

        //generate->printComToFile(VMCommands::call, method_scope, "");
    }

    //this is used to check that expression lists is compatible with function type
    GlobalSymbolTable->exprlist_array.push_back({exprtypes, class_name, method_scope, token.lineNum, filename});

    //exit function
    return;
}

//returnStatemnt -> return [ expression ] ;
void Parser::returnStatement(std::string name){

    ClassTable::MethodName tempMethod;

    std::string m_type;

    bool isVoidFunc;

    std::string expr_type;

    bool isFuncExpression = false;

    //store the function type
    std::string func_type = "null"; //initialize
    
    tempMethod.method_type = pclasstable->symbolTable(name).method_type;
    tempMethod.function_type = pclasstable->symbolTable(name).function_type;

    //return;} -> void method
    if(tempMethod.function_type == "void" && tempMethod.method_type != "constructor"){
        isVoidFunc = true;
    } else if(tempMethod.function_type == "void" && tempMethod.method_type == "constructor"){
        isVoidFunc = true;
    } else {
        isVoidFunc = false;
    }

    Token token;

    //expecting a 'return' keyword
    token = localToken->GetNextToken();

    //log handling
    if(token.lexeme == "return"){
        LOG_SUCCESS("Parser", token.lexeme, filename,token.lineNum);

    } else {
        LOG_ERROR("Parser", "Expected a 'return' statement", filename,token.lineNum);
    }

    //function has a return statement
    pclasstable->methodHasReturnStatement(name);

    //peek to see if this is an expression or ';'
    token = localToken->PeekNextToken();

    //if we have ';' and it is a void this is valid
    if(token.lexeme == ";" && (isVoidFunc == true) || (pclasstable->isJackOSAPI)){
        //consume the ';'
        token = localToken->GetNextToken();

        LOG_SUCCESS("Parser", token.lexeme, filename,token.lineNum);

    } else if(token.lexeme != ";"  || (pclasstable->isJackOSAPI)){
        //unconditionally call the expression function
        expr_type = expression(name, "");

        if(expr_type != "int" && expr_type != "char" && expr_type != "boolean"){
            isFuncExpression = true;

            //this means that the expression is a function, check for compatibility later
            GlobalSymbolTable->statcompatibility.push_back({tempMethod.function_type, expr_type,filename, token.lineNum});
        }
        
        //determine if the expression is compatible with function type
        if(tempMethod.function_type == expr_type || (isFuncExpression)  || (pclasstable->isJackOSAPI)){
            LOG_SUCCESS("Parser", token.lexeme, filename,token.lineNum);

        } else {
            LOG_ERROR("Semantic Analayzer", "return expression is incompatible with the function type",filename,token.lineNum);

        }

        //now we expect a ';'
        token = localToken->GetNextToken();

        if(token.lexeme == ";"){
            LOG_SUCCESS("Parser", token.lexeme,filename,token.lineNum);
        } else {
            LOG_ERROR("Parser", "Expected a ';'", filename,token.lineNum);
        }

    } else if(token.lexeme == ";" && isVoidFunc == false) {
        LOG_ERROR("Semantic Analayzer", "Expected an expression after 'return' statement", filename,token.lineNum);

    } else if((isVoidFunc) && token.lexeme != ";"){
        LOG_ERROR("Semantic Analayzer", "Incorrect void function return statement", filename,token.lineNum);

    } else {
        LOG_ERROR("Parser", "Expected an expression after 'return' statement",filename,token.lineNum);

    }

    if(isVoidFunc){
        generate->printComToFile(VMCommands::push, "constant", "0");
    }

    //out of scope
    //a return statement should not have anything else other than '}'
    token = localToken->PeekNextToken();

    if(token.lexeme == "["){
        token = localToken->GetNextToken();

        std::string arraExpr = expression(name, "");

        token = localToken->GetNextToken();

        if(token.lexeme == "]"){
            LOG_SUCCESS("Parser", token.lexeme, filename,token.lineNum);

        } else {
            LOG_ERROR("Parser", "Expected a ']'", filename,token.lineNum);

        }

        token = localToken->GetNextToken();

        if(token.lexeme == ";"){
            LOG_SUCCESS("Parser", token.lexeme, filename,token.lineNum);

        } else {
            LOG_ERROR("Parser", "Expected a ';'", filename,token.lineNum);

        }
    }

    token = localToken->PeekNextToken();

    generate->printComToFile(VMCommands::RETURN, "", "");

    if(token.lexeme == "}"){
        LOG_SUCCESS("Parser", token.lexeme, filename,token.lineNum);

    } else {
        LOG_ERROR("Parser", "function is out of scope, expected a '}'", filename,token.lineNum);

    } 
}

//paramList -> type identifier {, type identifier} | ε
void Parser::parameterList(std::string scope_name){
    Symbol symbol;

    Token token;

    int argsOffset = 0;

    //peek to see if this is an epsilon
    token = localToken->PeekNextToken();

    //if this is a ')' that means this is an epsilon so break function
    if(token.lexeme == ")"){
        LOG_SUCCESS("Parser", token.lexeme, filename,token.lineNum);

        return;

    } else if(token.type == TokenType::boolean || token.type == TokenType::identifier || 
        token.type == TokenType::integerConstant || token.type == TokenType::charConstant){
        
        //only add the type to the args check as this is relevant
        pclasstable->addArgsSymbolTable(token.lexeme, scope_name);

        //get the string value of the type
        symbol.type = token.lexeme;
        
        //call the type function to consume the token
        type(); 

        //expecting an identifier now
        token = localToken->GetNextToken();

        //error handling
        if(token.type == TokenType::identifier){
            LOG_SUCCESS("Parser", token.lexeme, filename,token.lineNum);

            //set the name to the current lexeme
            symbol.symbolName = token.lexeme;   

            //since paramList is an argument
            symbol.kind = SymbolKind::Argument;

            //offset in st
            symbol.offset = 0;

            symbol.isInitialized = "true";

            //later check for duplicate argument names
            //add the argument to the symbol table  
            pclasstable->accessSymbolTable(scope_name, symbol);
            
        } else {
            LOG_ERROR("Parser", "Expected an identifier at the parameter list",filename,token.lineNum);

        }

        //peek to see if there is a ','
        token = localToken->PeekNextToken();
        
        while(token.lexeme == ","){
            //declare another symbol
            Symbol sym_2;

            LOG_SUCCESS("Parser", token.lexeme, filename,token.lineNum);

            //consume the ',' token
            token = localToken->GetNextToken();

            //peek to see if the next token is a type
            token = localToken->PeekNextToken();

            //later used to check correct args used in a statement
            pclasstable->addArgsSymbolTable(token.lexeme, scope_name);

            //this if-else statement is used to set the symbol type
            //the type function will consume the token
            if(token.type == TokenType::boolean || token.type == TokenType::identifier || 
            token.type == TokenType::integerConstant || token.type == TokenType::charConstant){
                sym_2.type = token.lexeme;

                //consume the type
                type();   
            } else {
                LOG_ERROR("Parser", "Expected a type", filename, token.lineNum);

            }

            //expecting an identifier now
            token = localToken->GetNextToken();

            //error handling
            if(token.type == TokenType::identifier){
                LOG_SUCCESS("Parser", token.lexeme, filename,token.lineNum);

                sym_2.symbolName = token.lexeme;

                sym_2.kind = SymbolKind::Argument;

                sym_2.offset = argsOffset++;

                sym_2.isInitialized = "true";

                //add another argument to the symbol table
                pclasstable->accessSymbolTable(scope_name, sym_2);

            } else {
                LOG_ERROR("Parser", "Expected an identifier at the parameter list", filename,token.lineNum);

            }

            //peek to see if the next token is ','
            token = localToken->PeekNextToken();
        }
    //this is just an error
    } else {
        LOG_ERROR("Parser", "Expected a valid argument or ')'", filename,token.lineNum);

    }
}

void Parser::printSymbolTable(){
    pclasstable->printTables();
}

void Parser::checkSubroutineDeclaration(){
    std::vector<SubroutineName>::size_type it;

    for(it =0; it != subroutineNames.size(); ++it){
        if(pclasstable->findSymbolTable(subroutineNames[it].name) == -1){
            LOG_ERROR("Semantic Analayzer", "'" + subroutineNames[it].name + "' does not exitst", subroutineNames[it].line,filename);
        }
    }
}

bool Parser::checkSubNames(std::string subname){
    std::vector<statementTypes>::size_type it;

    //search for symbol table by name
    for(it = 0; it != subroutineNames.size(); it++){
        if(subroutineNames[it].name == subname){
            return true;
        }
    }
}

void Parser::methodTypeCheck(){
    std::vector<statementTypes>::size_type it;
    //search for symbol table by name
    for(it = 0; it != typeCheck.size(); it++){
        if(pclasstable->returnFunctionType(typeCheck[it].methodName) == typeCheck[it].statementType){
            LOG_SUCCESS("Semantic Analayzer", "compatible method type", typeCheck[it].line_num, filename);
        } else {
            LOG_ERROR("Semantic Analayzer", "Uncompatible types in statement", typeCheck[it].line_num, filename);

        }
    }
}

void Parser::createVMFile(){
    //if this is an API file, do not make file
    if(!(vfile.isJackOS)){
        generate->createFile(vfile.filename);

    } 
}

