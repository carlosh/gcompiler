//
//  TableLR.h
//  compiler
//
//  Created by Carlos Hernandez on 5/30/11.
//  Copyright 2011 Software Factory, LLC. All rights reserved.
//


#pragma once

#include <vector>
#include <string>
#include "ira.h"
#include "accion.h"
#include "rowLR.h"

using namespace std;


class TablaLR
{
    vector<RowLR*> lista;
    
public:
    
    vector<RowLR*> getLista();
    
    //void addRow(RowLR* r);
    
};