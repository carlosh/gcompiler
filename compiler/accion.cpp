//
//  accion.cpp
//  compiler
//
//  Created by Carlos Hernandez on 5/30/11.
//  Copyright 2011 Software Factory, LLC. All rights reserved.
//

#include "accion.h"


Accion::Accion(tipo_accion _tipo, int _estadoID, terminal* t)
{
    tipo = _tipo;
    estadoID = _estadoID;
    term = t;
}


int Accion::getEstadoID()
{
    return estadoID;
}

tipo_accion Accion::getTipo()
{
    return tipo;
}

terminal* Accion::getTerminal()
{
    return term;
}

void Accion::setEstadoID(int id)
{
    estadoID = id;
}