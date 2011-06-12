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
#include <map>
#include "Parametro.h"

using namespace std;

class production
{
    nonterminal* variable;
    vector<symbol*> symbols;
    string codigo;
    int codigoId;
    vector<Parametro*> parametros;
    
public:
    production(nonterminal* nonterminalVariable);
  
    void addSymbol(symbol* _symbol);
    void setCodigo(string code);
    
    nonterminal* getVariable();
    vector<symbol*> getSymbols();
    string getCodigo();
    
    void setCodigoId(int id);
    int getCodigoId();
    
    void addParametro(string id, symbol* sym, int position);
    void insertListaParametros(vector<Parametro*> lista);
    
    vector<Parametro*> getParametros();

    string toString();
};