//
//  produccion.h
//  compiler
//
//  Created by Carlos Hernandez on 4/17/11.
//  Copyright 2011 Software Factory, LLC. All rights reserved.
//
#pragma once

#include "nonterminal.h"
#include "symbol.h"
#include <vector>
#include <string>

using namespace std;

class production
{
    nonterminal* variable;
    vector<symbol*> symbols;
    vector<string> codigo;
    
public:
    production(nonterminal* nonterminalVariable);
  
    void addSymbol(symbol* _symbol);
    void addCode(string code);
    
    nonterminal* getVariable();
    vector<symbol*> getSymbols();
    vector<string> getCodigo();
};