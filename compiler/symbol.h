//
//  simbolo.h
//  compiler
//
//  Created by Carlos Hernandez on 4/17/11.
//  Copyright 2011 Software Factory, LLC. All rights reserved.
//
#pragma once
#include <string>

using namespace std;

class symbol
{
    string id;
    
public:
    symbol(string _id);
    ~symbol(void);
    
    string getID();
};