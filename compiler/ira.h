//
//  ira.h
//  compiler
//
//  Created by Carlos Hernandez on 5/30/11.
//  Copyright 2011 Software Factory, LLC. All rights reserved.
//

#pragma once
#include "nonterminal.h"

class Ira
{
    int estadoID;
    nonterminal* nonterm;
    
public:
    Ira(int _estadoID, nonterminal* nt);
    
    
    int getEstadoID();
    nonterminal* getNonTerminal();
};