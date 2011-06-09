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
    codigo = "";
    codigoId = -1;
};

void production::addSymbol(symbol* _symbol)
{
    symbols.push_back(_symbol);
}

void production::setCodigo(string code)
{
    codigo = code;
}


nonterminal* production::getVariable()
{
    return variable;
}


vector<symbol*> production::getSymbols()
{
    return symbols;
}

string production::getCodigo()
{
    return codigo;
}

void production::setCodigoId(int id)
{
    codigoId = id;
}

int production::getCodigoId()
{
    return codigoId;
}


void production::addParametro(string id, symbol *sym)
{
    
    
    
    Parametro* newParametro = new Parametro(id, sym->getReturnObjectID());
    
    parametros.push_back(newParametro);
}

vector<Parametro*> production::getParametros()
{
    return parametros;
}

string production::toString()
{
    string rt = getVariable()->getID() + " -> ";
    
    
    vector<symbol*>::iterator iterSymbol;
    
    
    //int symbolPos = 0;
    long totalSym = symbols.size();
    
    int symbolPos;
    for (symbolPos = 0; symbolPos < totalSym; symbolPos++) {
        
        symbol* sym = symbols[symbolPos];
        
        rt += sym->getID() + " ";
        
        
        
    }
    

    return rt;
}


