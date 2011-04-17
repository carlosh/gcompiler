//
//  main.cpp
//  compiler
//
//  Created by Carlos Hernandez on 4/16/11.
//  Copyright 2011 Software Factory, LLC. All rights reserved.
//

#include <iostream>

#include "stdafx.h"
#include <iostream>
#include "LexAnalyzer.h"
#include "codegenerator.h"

int main (int argc, const char * argv[])
{

    long fileLen;
	char* buffer;
	FILE* file = NULL;
	file = fopen(argv[1], "r+");
    
	if ( file != NULL )
	{
		fseek(file, 0, SEEK_END);
		fileLen = ftell(file);
		fseek(file, 0, SEEK_SET);
        
		buffer = (char*)malloc(fileLen + 1);
        
		if ( !buffer )
		{
			cout << "Error reading file to buffer!";
			return 0;
		}
		
		fread(buffer, fileLen, 1, file);
        
		LexAnalyzer* lex;
        
		lex = new LexAnalyzer(buffer, fileLen);
		//lex->start();
		bool error = false;
        
		try
		{
			codegenerator* cc = new codegenerator(lex);
			cc->start();
		}
		catch(exception* c)
		{
			error = true;
			printf("Error located at line %d, column %d :%s", lex->LineNumber(), lex->ColNumber(), c->what());
		}
        
        
		if (!error)
			printf("\r\n\r\n\r\nCompilation ended successfully \r\n");
		
        
		fclose(file);
	}
	else
	{
		printf("Error opening file!\r\n");
	}
    
	//cin.get();
    
    // insert code here...
    //std::cout << "Hello, World!\n";
    return 0;
}

