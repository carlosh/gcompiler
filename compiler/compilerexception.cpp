//
//  compilerexception.cpp
//  compiler
//
//  Created by Carlos Hernandez on 4/16/11.
//  Copyright 2011 Software Factory, LLC. All rights reserved.
//

#include "compilerexception.h"


compilerexception::compilerexception(string error)
{
    errorDescription = error; 
}

compilerexception::~compilerexception() throw()
{
    
}

const char* compilerexception::what() const throw()
{
    return errorDescription.c_str();
}