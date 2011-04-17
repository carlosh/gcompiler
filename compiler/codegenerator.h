//
//  codegenerator.h
//  compiler
//
//  Created by Carlos Hernandez on 4/16/11.
//  Copyright 2011 Software Factory, LLC. All rights reserved.
//

#include "LexAnalyzer.h"
#include "production.h"
#include <vector>

class codegenerator
{
    
private:
    
    LexAnalyzer* lex;
    vector<production*> gramatica;
    
public:

    
    codegenerator(LexAnalyzer* tlex);
    
    void start();
};