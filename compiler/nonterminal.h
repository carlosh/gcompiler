//
//  nonterminal.h
//  compiler
//
//  Created by Carlos Hernandez on 4/17/11.
//  Copyright 2011 Software Factory, LLC. All rights reserved.
//

#pragma once

#include "symbol.h"
#include <string>

using namespace std;

class nonterminal : public symbol
{
    
public:
    nonterminal(string _id, string objectName);
    
    
    string javaString();
    
};