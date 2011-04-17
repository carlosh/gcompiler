//
//  simbolo.cpp
//  compiler
//
//  Created by Carlos Hernandez on 4/17/11.
//  Copyright 2011 Software Factory, LLC. All rights reserved.
//

#include "symbol.h"

symbol::symbol(string _id)
{
    id = _id;
}

symbol::~symbol(void)
{
    
}

string symbol::getID()
{
    return id;
}
