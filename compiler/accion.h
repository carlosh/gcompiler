//
//  accion.h
//  compiler
//
//  Created by Carlos Hernandez on 5/30/11.
//  Copyright 2011 Software Factory, LLC. All rights reserved.
//

#pragma once
#include "terminal.h"

enum tipo_accion {
    SHIFT = 0,
    REDUCE = 1,
    ACCEPT = 2
    };


class Accion
{
    tipo_accion tipo;
    int estadoID;
    terminal* term;
    
public:
    Accion(tipo_accion _tipo, int _estadoID, terminal* _term);
    
    int getEstadoID();
    tipo_accion getTipo();
    terminal* getTerminal();
    
    void setEstadoID(int id);
};