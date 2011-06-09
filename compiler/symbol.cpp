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
    returnObjectID = "";
}

symbol::symbol(string _id, string rt)
{
    id = _id;
    returnObjectID = rt;
}

symbol::~symbol(void)
{
    
}


string symbol::getReturnObjectID()
{
    return returnObjectID;
}


string symbol::getID()
{
    return id;
}


int symbol::getNumericID()
{
    return numericID;
}

void symbol::setNumericID(int nid)
{
    numericID = nid;
}