//
//  Parametro.h
//  compiler
//
//  Created by Carlos Hernandez on 6/9/11.
//  Copyright 2011 Software Factory, LLC. All rights reserved.
//


#pragma once

#include <string>

using namespace std;

class Parametro
{
    string id;
    string returnType;
    int position;
    
    
public:
    Parametro(string _id, string _returnType, int pos);
    
    int getPosition();
    string getId();
    string getReturnType();
};