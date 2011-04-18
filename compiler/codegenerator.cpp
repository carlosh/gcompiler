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
#include <iostream>

codegenerator::codegenerator(LexAnalyzer* tlex)
{
    lex = tlex;
    importCode = false;
    parseCode = false;
    classDeclaration = false;
    className = "";
    
    terminalList.push_back(new terminal(EPSILON));
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
            else if ( typeid(*token) == typeid(TokenID) )
                tagProductions();
            
            
            if ( typeid(*token) == typeid(TokenEof) )
				break;
		}
        
        //check if every nonterminal has a production.
        vector<nonterminal*>::iterator iterNonTerminal;
        
        for (iterNonTerminal = nonterminalList.begin(); iterNonTerminal != nonterminalList.end(); ++iterNonTerminal) {
            
            string nonTerminalID = (*iterNonTerminal)->getID();
          
            if (!isNonTerminalInProduction(nonTerminalID))
                throw new compilerexception("Non Terminal (" + nonTerminalID + ") has no productions.");
            
        }
        

        //display production list
        vector<production*>::iterator iterProd;
        
        for (iterProd = gramatica.begin(); iterProd != gramatica.end(); ++iterProd) {
            
            printf("%s -> ", (*iterProd)->getVariable()->getID().c_str());
            
            vector<symbol*>::iterator iterSymbol;
            
            vector<string> codigo = (*iterProd)->getCodigo();
            
            int symbolPos = 0;
            for (iterSymbol = (*iterProd)->getSymbols().begin(); iterSymbol != (*iterProd)->getSymbols().end(); ++iterSymbol) 
            {
                printf("%s ", codigo[symbolPos].c_str());
                printf("%s ", (*iterSymbol)->getID().c_str());
                
                symbolPos++;
            }
            
            if (symbolPos < codigo.size())
                printf("%s ", codigo[symbolPos].c_str());
            
            
            
            printf("\r\n");
            
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
                               
                if (isSymbolDeclared(id))
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
                
                if (isSymbolDeclared(id))
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



bool codegenerator::isTerminal(string id)
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
    
    return found;
}

bool codegenerator::isNonTerminalInProduction(string id)
{
    bool found = false;
    
    vector<production*>::iterator prodIterator;
    
    for (prodIterator = gramatica.begin(); prodIterator != gramatica.end(); ++prodIterator) {
        if ( (*prodIterator)->getVariable()->getID() == id )
        {
            found = true;
            break;
        }
    }    
    
    
    return found;
}

bool codegenerator::isNonTerminal(string id)
{
    bool found = false;
    
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

nonterminal* codegenerator::getNonTerminal(string id)
{
    vector<nonterminal*>::iterator iterNonTerminal;
    
    for (iterNonTerminal = nonterminalList.begin(); iterNonTerminal != nonterminalList.end(); ++iterNonTerminal) {
        if ( (*iterNonTerminal)->getID() == id )
        {
            return *iterNonTerminal;
        }
    }    
    
    return NULL;
}

terminal* codegenerator::getTerminal(string id)
{
    vector<nonterminal*>::iterator iterNonTerminal;
    
    vector<terminal*>::iterator iterTerminal;
    
    for (iterTerminal = terminalList.begin(); iterTerminal != terminalList.end(); ++iterTerminal) {
        if ( (*iterTerminal)->getID() == id )
        {
            return *iterTerminal;
        }
    }
    
    return NULL;
}

symbol* codegenerator::getSymbol(string id)
{
    symbol* symbolObj = NULL;
    
    symbolObj = getNonTerminal(id);
    
    if (symbolObj != NULL)
        return symbolObj;
    
    symbolObj = getTerminal(id);
    
    return symbolObj;
    
}


bool codegenerator::isSymbolDeclared(string id)
{
    bool foundTerminal, foundNonTerminal = false;
    
    foundTerminal = isTerminal(id);
    foundNonTerminal = isNonTerminal(id);

    
    return foundTerminal || foundNonTerminal;
}

void codegenerator::tagProductions()
{
    while (typeid(*lex->currentToken()) != typeid(TokenEof)) {
        
        //id
        string variableString = lex->currentToken()->lexema();
        
        if (isTerminal(variableString))
        {
            throw new compilerexception("Terminal symbol (" + variableString + ") cannot be on the left side of the production!");  
        }
        
        if (!isNonTerminal(variableString))
        {
           throw new compilerexception("Non Terminal Symbol (" + variableString + ") not declared!");    
        }
        
        nonterminal* symbol = getNonTerminal(variableString);
        
        lex->nextToken();
        
        if (typeid(*lex->currentToken()) != typeid(TokenProduction))
            throw new compilerexception("Token '->' expected!"); 
        
        lex->nextToken();
        
        parseSymbols(symbol);
        
        
        
        
    }
}

void codegenerator::parseSymbols(nonterminal* nonTerminalSymbol)
{
    
    
    while (true) {
        
        production* prod = new production(nonTerminalSymbol);
        
        gramatica.push_back(prod);
        
        string code = "";
        
        if (typeid(*lex->currentToken()) == typeid(TokenJavaCode))
        {
            code = lex->currentToken()->lexema();
            lex->nextToken();
            
            prod->addCode(code);
        }
        else
            prod->addCode(" ");

        
        while (true)
        {
            
            if (typeid(*lex->currentToken()) == typeid(TokenPipe))
                break;
                
            
            if (typeid(*lex->currentToken()) != typeid(TokenID) && typeid(*lex->currentToken()) != typeid(TokenSemicolon)  )
                throw new compilerexception("Symbol expected!"); 
            
            if (typeid(*lex->currentToken()) == typeid(TokenSemicolon))
            {
                prod->addSymbol(getSymbol(EPSILON));
            }
            else
            {
                
                symbol* symbolObj = getSymbol(lex->currentToken()->lexema());
                
                if (symbolObj == NULL)
                {
                   throw new compilerexception("Undefined symbol (" + lex->currentToken()->lexema() + ")!");  
                }
                
                lex->nextToken();
                
                if (typeid(*lex->currentToken()) == typeid(TokenJavaCode))
                {
                    code = lex->currentToken()->lexema();
                    lex->nextToken();
                    
                    prod->addCode(code);
                }
                else
                    prod->addCode(" ");
                
                
                prod->addSymbol(symbolObj);
                
            }
            
            if (typeid(*lex->currentToken()) == typeid(TokenPipe) || typeid(*lex->currentToken()) == typeid(TokenSemicolon) )
                break; //next production from same variable.

        }
        
        if (typeid(*lex->currentToken()) == typeid(TokenPipe))
        {
            lex->nextToken();
            continue; //next production from same variable.
        }
        
        if (typeid(*lex->currentToken()) == typeid(TokenSemicolon))
        {
            lex->nextToken();
            break;
        }
        
        if (typeid(*lex->currentToken()) == typeid(TokenEof))
             throw new compilerexception("Unexpected Eof!"); 
        
    }
}
