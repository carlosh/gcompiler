//
//  rowLR.h
//  compiler
//
//  Created by Carlos Hernandez on 5/30/11.
//  Copyright 2011 Software Factory, LLC. All rights reserved.
//

#include <vector>
#include <map>
#include "terminal.h"
#include "accion.h"
#include "nonterminal.h"
#include "ira.h"

using namespace std;

class RowLR
{
    map<terminal*, Accion*> ListaAcciones;
    map<nonterminal*, Ira*> ListaIra;
    
    
public:
    
    map<terminal*, Accion*> getListaAcciones();
    map<nonterminal*, Ira*> getListaIra();
    
    
    
};