//
//  compilerexception.h
//  compiler
//
//  Created by Carlos Hernandez on 4/16/11.
//  Copyright 2011 Software Factory, LLC. All rights reserved.
//

#include <stdio.h>
#include <string>

using namespace std;

class compilerexception : exception
{
    
    string errorDescription;
    
public:
    compilerexception(string error);
    
    ~compilerexception() throw();
    
    const char* what() const throw();
};