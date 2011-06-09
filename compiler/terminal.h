//
//  terminal.h
//  compiler
//
//  Created by Carlos Hernandez on 4/17/11.
//  Copyright 2011 Software Factory, LLC. All rights reserved.
//

#pragma once
#include "symbol.h"
#include <string>

using namespace std;

class terminal : public symbol
{
    
public:
    terminal(string _id);
    terminal(string _id, string rt);
    
    string javaString();
};