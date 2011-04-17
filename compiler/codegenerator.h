//
//  codegenerator.h
//  compiler
//
//  Created by Carlos Hernandez on 4/16/11.
//  Copyright 2011 Software Factory, LLC. All rights reserved.
//

#include "LexAnalyzer.h"

class codegenerator
{
    
private:
    
    LexAnalyzer* lex;
    
public:

    
    codegenerator(LexAnalyzer* tlex);
    
    void start();
};