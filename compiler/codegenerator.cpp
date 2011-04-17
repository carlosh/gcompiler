//
//  codegenerator.cpp
//  compiler
//
//  Created by Carlos Hernandez on 4/16/11.
//  Copyright 2011 Software Factory, LLC. All rights reserved.
//

#include "codegenerator.h"
#include "TokenInclude.h"
#include <typeinfo>
#include "compilerexception.h"

codegenerator::codegenerator(LexAnalyzer* tlex)
{
    lex = tlex;
    importCode = false;
    parseCode = false;
    classDeclaration = false;
    className = "";
}


void codegenerator::start()
{
    /*
     int i;
     for( i = 0; i < buffer_size; ++i )
     {
     printf("%c", ((char*)buffer)[i]);
     }
     */
	
	try
	{
		Token* token;
		while((token = lex->nextToken()))
		{
			//printf("%s -> %s \r\n", token->lexema().c_str(), token->className());
            
			
            if ( typeid(*token) == typeid(TokenTagOpen) )
                tagOpen();
            else if ( typeid(*token) == typeid(TokenDeclaration) )
                tagDeclaration();
            else if ( typeid(*token) == typeid(TokenTerminal) )
                tagTerminal();
            else if ( typeid(*token) == typeid(TokenNonTerminal) )
                tagNonTerminal();
            
            
            
            if ( typeid(*token) == typeid(TokenEof) )
				break;
		}
	}
	catch(exception* c)
	{
		printf("Error located at line %d, column %d :%s", lex->LineNumber(), lex->ColNumber(), c->what());
	} 
}

void codegenerator::tagOpen()
{
   if ( typeid(*lex->currentToken()) == typeid(TokenTagOpen) )
   {
       lex->nextToken();
       
       if ( typeid(*lex->currentToken()) == typeid(TokenImport) )
           importTag();
       else if ( typeid(*lex->currentToken()) == typeid(TokenParserCode) )
           parseCodeTag();
   }
    else
        throw new compilerexception("Tag '<%' expected!");
}

void codegenerator::importTag()
{
    if ( typeid(*lex->currentToken()) == typeid(TokenImport) )
    {
        if (!importCode)
        {
            importCode = true;
            Token* token;
            while((token = lex->nextToken()))
            {
                //printf("%s -> %s \r\n", token->lexema().c_str(), token->className());
                
                //TODO: do something later
                
                if ( typeid(*token) == typeid(TokenTagClose) )
                    break;
                else if ( typeid(*token) == typeid(TokenEof) )
                    throw new compilerexception("Unexpected Eof token. Expected '%>' instead.");
                
            }
        }
        else
            throw new compilerexception("Tag 'import' can only be declared once!");
    }
    else
        throw new compilerexception("Tag 'import' expected!");
}

void codegenerator::parseCodeTag()
{
    if ( typeid(*lex->currentToken()) == typeid(TokenParserCode) )
    {
        if (!parseCode)
        {
            parseCode = true;
            Token* token;
            while((token = lex->nextToken()))
            {
                //printf("%s -> %s \r\n", token->lexema().c_str(), token->className());
                
                //TODO: do something later
                
                if ( typeid(*token) == typeid(TokenTagClose) )
                    break;
                else if ( typeid(*token) == typeid(TokenEof) )
                    throw new compilerexception("Unexpected Eof token. Expected '%>' instead.");
                
            }
        }
        else
            throw new compilerexception("Tag 'parsecode' can only be declared once!");
    }
    else
        throw new compilerexception("Tag 'parsecode' expected!");
}


void codegenerator::tagDeclaration()
{
    if ( typeid(*lex->currentToken()) == typeid(TokenDeclaration) )
    {
        lex->nextToken();
        
        if ( typeid(*lex->currentToken()) == typeid(TokenClass) )
        {
            if (!classDeclaration)
            {   
                classDeclaration = true;
                
                lex->nextToken();
                
                className = lex->currentToken()->lexema();
            }
            else
                throw new compilerexception("Tag 'class' can only be declared once!");    
        }
        else
            throw new compilerexception("Only 'class' expected after '%'. Unexpected token!");
    }
    else
        throw new compilerexception("Tag '%' expected!");
}

void codegenerator::tagTerminal()
{
    if ( typeid(*lex->currentToken()) == typeid(TokenTerminal) )
    {
        Token* token;
        while((token = lex->nextToken()))
        {
            //printf("%s -> %s \r\n", token->lexema().c_str(), token->className());
            if ( typeid(*token) == typeid(TokenID) )
            {
                string id = lex->currentToken()->lexema();
                               
                if (isIdInList(id))
                {
                    throw new compilerexception("Symbol ID (" + id + ") already declared!");
                }
                
                //simbolo
                terminal* terminalSymbol = new terminal(id);
                terminalList.push_back(terminalSymbol);

                
                token = lex->nextToken();
                    
                if ( typeid(*token) != typeid(TokenComma) && ( typeid(*token) != typeid(TokenSemicolon) ) )
                    throw new compilerexception("',' expected!");
            }
            
            if ( typeid(*token) == typeid(TokenSemicolon) )
                break;
            else if ( typeid(*token) == typeid(TokenEof) )
                throw new compilerexception("Unexpected Eof token. Expected '%>' instead.");
            
        }
    }
    else
        throw new compilerexception("Tag 'terminal' expected!");

}

void codegenerator::tagNonTerminal()
{
    if ( typeid(*lex->currentToken()) == typeid(TokenNonTerminal) )
    {
        bool onlySymbols = false;
        string returnId = "";
        Token* token;
        while((token = lex->nextToken()))
        {
            //printf("%s -> %s \r\n", token->lexema().c_str(), token->className());
            
            if ( typeid(*token) == typeid(TokenID) )
            {
                if (!onlySymbols)
                {
                    returnId = token->lexema();
                    
                    token = lex->nextToken();
                    
                    if ( typeid(*token) != typeid(TokenID) )
                    {
                        throw new compilerexception("Symbol expected!"); 
                    }
                    
                    onlySymbols = true;
                }
                
                string id = lex->currentToken()->lexema();
                
                if (isIdInList(id))
                {
                    throw new compilerexception("Symbol ID (" + id + ") already declared!");
                }
                
                nonterminal* nonterminalSymbol = new nonterminal(id, returnId);
                nonterminalList.push_back(nonterminalSymbol);
                
                token = lex->nextToken();
                
                if ( typeid(*token) != typeid(TokenComma) && ( typeid(*token) != typeid(TokenSemicolon) ) )
                    throw new compilerexception("',' expected!");
            }
            
            if ( typeid(*token) == typeid(TokenSemicolon) )
                break;
            else if ( typeid(*token) == typeid(TokenEof) )
                throw new compilerexception("Unexpected Eof token. Expected '%>' instead.");
            
        }
    }
    else
        throw new compilerexception("Tag 'nonterminal' expected!");
}


bool codegenerator::isIdInList(string id)
{
    bool found = false;
    
    vector<terminal*>::iterator iterTerminal;
    
    for (iterTerminal = terminalList.begin(); iterTerminal != terminalList.end(); ++iterTerminal) {
        if ( (*iterTerminal)->getID() == id )
        {
            found = true;
            break;
        }
    }

    vector<nonterminal*>::iterator iterNonTerminal;
    
    for (iterNonTerminal = nonterminalList.begin(); iterNonTerminal != nonterminalList.end(); ++iterNonTerminal) {
        if ( (*iterNonTerminal)->getID() == id )
        {
            found = true;
            break;
        }
    }
    
    return found;
}
