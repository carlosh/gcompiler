//
//  ItemSet.cpp
//  compiler
//
//  Created by Carlos Hernandez on 5/9/11.
//  Copyright 2011 Software Factory, LLC. All rights reserved.
//

#include "ItemSet.h"

ItemSet::ItemSet()
{
    items = new vector<ProductionItem*>();
}

void ItemSet::addProduction(ProductionItem* prod)
{
    items->push_back(prod);
}


vector<ProductionItem*>* ItemSet::getProductionItems()
{
    return items;
}


ItemSet::~ItemSet()
{
    items->clear();
    
    delete items;
}