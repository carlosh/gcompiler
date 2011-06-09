//
//  Parametro.cpp
//  compiler
//
//  Created by Carlos Hernandez on 6/9/11.
//  Copyright 2011 Software Factory, LLC. All rights reserved.
//

#include "Parametro.h"


Parametro::Parametro(string _id, string _returnType)
{
    id = _id;
    returnType = _returnType;
}

string Parametro::getId()
{
    return id;
}

string Parametro::getReturnType()
{
    return returnType;
}