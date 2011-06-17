//
//  ProductionItem.h
//  compiler
//
//  Created by Carlos Hernandez on 5/9/11.
//  Copyright 2011 Software Factory, LLC. All rights reserved.
//

#pragma once
#include "production.h"
#include "terminal.h"


class ProductionItem
{
    production* prod;
    int dotPosition;
    terminal* lookAhead;
    bool isKernel;
    bool isProcessed;
    bool gotoProcessed;
    int passId;
    int originalState;
    string lookAheadString;
    
public:
    ProductionItem(production* _prod, terminal* _la, bool isKernel);
    void moveDot();
    int getDotPosition();
    void setDotPosition(int pos);
    symbol* nextSymbol();
    void setIsKernel(bool kernel);
    bool getIsKernel();
    void setIsProcessed(bool processed);
    bool getIsProcessed();
    bool getIsGotoProcessed();
    void setGotoProcessed(bool processed);
    production* getProduction();
    terminal* getLookAhead();
    void setLookAhead(terminal *la);
    vector<symbol*> getBeta();
    int getPassId();
    void setPassId(int p);
    void setOriginalState(int os);
    int getOriginalState();
    void appendLookAheadString(string l);
    string getLookAheadString();
};