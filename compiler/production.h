//
//  produccion.h
//  compiler
//
//  Created by Carlos Hernandez on 4/17/11.
//  Copyright 2011 Software Factory, LLC. All rights reserved.
//

#include "nonterminal.h"
#include "symbol.h"
#include <vector>


class production
{
    nonterminal* variable;
    vector<symbol*> symbols;
    vector<string> codigo;
    
    
};