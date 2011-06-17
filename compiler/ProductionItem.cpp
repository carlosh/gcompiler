//
//  ProductionItem.cpp
//  compiler
//
//  Created by Carlos Hernandez on 5/9/11.
//  Copyright 2011 Software Factory, LLC. All rights reserved.
//

#include "ProductionItem.h"

ProductionItem::ProductionItem(production* _prod, terminal* la, bool _isKernel)
{
    prod = _prod;
    isKernel = _isKernel;
    lookAhead = la;
    dotPosition = 0;
    isProcessed = false;
    gotoProcessed = false;
    passId = 1;
    lookAheadString = "";
    
}


vector<symbol*> ProductionItem::getBeta()
{
    vector<symbol*> lista;
    
    for (int i = dotPosition + 1; i < prod->getSymbols().size(); i++) {
        lista.push_back(prod->getSymbols()[i]);
    }
    
    return lista;
}

void ProductionItem::setPassId(int p)
{
    passId = p;
}

int ProductionItem::getPassId()
{
    return passId;
}

void ProductionItem::setOriginalState(int os)
{
    originalState = os;
}

int ProductionItem::getOriginalState()
{
    return originalState;
}

void ProductionItem::setLookAhead(terminal* la)
{
    lookAhead = la;
}

terminal* ProductionItem::getLookAhead()
{
    return lookAhead;
}

production* ProductionItem::getProduction()
{
    return prod;
}

int ProductionItem::getDotPosition()
{
    return dotPosition;
}

void ProductionItem::moveDot()
{
    if ( dotPosition + 1 <= prod->getSymbols().size() )
        dotPosition++;
}

void ProductionItem::appendLookAheadString(string l)
{
    if (lookAheadString != "")
        lookAheadString += ", ";
    
    lookAheadString += l;
}

string ProductionItem::getLookAheadString()
{
    return lookAheadString;
}


void ProductionItem::setDotPosition(int pos)
{
    dotPosition = pos;
}

symbol* ProductionItem::nextSymbol()
{
    if ( dotPosition < prod->getSymbols().size() )
    {
        symbol* temp = prod->getSymbols()[dotPosition]; 
        return temp;
    }
    
    return NULL;
}


void ProductionItem::setIsKernel(bool kernel)
{
    isKernel = kernel;
}

bool ProductionItem::getIsKernel()
{
    return isKernel;
}

bool ProductionItem::getIsProcessed()
{
    return isProcessed;
}

void ProductionItem::setIsProcessed(bool processed)
{
    isProcessed = processed;
}

bool ProductionItem::getIsGotoProcessed()
{
    return gotoProcessed;
}

void ProductionItem::setGotoProcessed(bool processed)
{
    gotoProcessed = processed;
}