//
//  nonterminal.cpp
//  compiler
//
//  Created by Carlos Hernandez on 4/17/11.
//  Copyright 2011 Software Factory, LLC. All rights reserved.
//

#include "nonterminal.h"

nonterminal::nonterminal(string _id, string objectName)
: symbol(_id)
{

    returnObjectID = objectName;
}

string nonterminal::getReturnObjectID()
{
    return returnObjectID;
}


string nonterminal::javaString()
{
    return "\"" + getID() + "\""; 
}