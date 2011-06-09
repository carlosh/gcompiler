//
//  terminal.cpp
//  compiler
//
//  Created by Carlos Hernandez on 4/17/11.
//  Copyright 2011 Software Factory, LLC. All rights reserved.
//

#include "terminal.h"


terminal::terminal(string _id) : symbol(_id)
{
    
}

terminal::terminal(string _id, string objectName)
: symbol(_id, objectName)
{
}

string terminal::javaString()
{
    return "String.valueOf(sym." + getID() + ")";
}