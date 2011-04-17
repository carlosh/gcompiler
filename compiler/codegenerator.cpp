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

codegenerator::codegenerator(LexAnalyzer* tlex)
{
    lex = tlex;
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
			printf("%s -> %s \r\n", token->lexema().c_str(), token->className());
            
			if ( typeid(*token) == typeid(TokenEof) )
				break;
		}
	}
	catch(exception* c)
	{
		printf("Error located at line %d, column %d :%s", lex->LineNumber(), lex->ColNumber(), c->what());
	} 
}