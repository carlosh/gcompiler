//
//  ira.cpp
//  compiler
//
//  Created by Carlos Hernandez on 5/30/11.
//  Copyright 2011 Software Factory, LLC. All rights reserved.
//

#include "ira.h"


Ira::Ira(int _estadoID, nonterminal* nt)
{
    estadoID = _estadoID;
    nonterm = nt;
}

int Ira::getEstadoID()
{
    return estadoID;
}

nonterminal* Ira::getNonTerminal()
{
    return nonterm;
}


void Ira::setEstadoID(int id)
{
    estadoID = id;
}