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
    markedForDelete = false;
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

void ItemSet::setIsMarkedForDelete(bool b)
{
    markedForDelete = b;
}

bool ItemSet::getIsMarkedForDelete()
{
    return markedForDelete;
}

void ItemSet::setId(int _id)
{
    id = _id;
}

int ItemSet::getId()
{
    return id;
}

void ItemSet::setStateIDToItems(int id)
{
    vector<ProductionItem*>::iterator it;
    
    for (it = items->begin(); it != items->end(); it++) {
        (*it)->setOriginalState(id);
    }
}





