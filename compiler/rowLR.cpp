//
//  rowLR.cpp
//  compiler
//
//  Created by Carlos Hernandez on 5/30/11.
//  Copyright 2011 Software Factory, LLC. All rights reserved.
//

#include "rowLR.h"


map<terminal*, Accion*> RowLR::getListaAcciones()
{
    return ListaAcciones;
}

map<nonterminal*, Ira*> RowLR::getListaIra()
{
    return ListaIra;
}