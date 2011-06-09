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
    int numericID;
    
public:
    symbol(string _id);
    ~symbol(void);
    
    
    int getNumericID();
    void setNumericID(int nid);
    
    string getID();
    
    virtual string javaString(){ return "";};
    
    virtual void f(){}; //Just to make typeid returned the derived type correclty uss=ing rtti.
};