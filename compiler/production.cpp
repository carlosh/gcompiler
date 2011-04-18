//
//  produccion.cpp
//  compiler
//
//  Created by Carlos Hernandez on 4/17/11.
//  Copyright 2011 Software Factory, LLC. All rights reserved.
//

#include "production.h"

production::production(nonterminal* nonterminalvariable)
{
    variable = nonterminalvariable;
};

void production::addSymbol(symbol* _symbol)
{
    symbols.push_back(_symbol);
}

void production::addCode(string code)
{
    codigo.push_back(code);
}


nonterminal* production::getVariable()
{
    return variable;
}


vector<symbol*> production::getSymbols()
{
    return symbols;
}

vector<string> production::getCodigo()
{
    return codigo;
}