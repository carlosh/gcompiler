//
//  ItemSet.h
//  compiler
//
//  Created by Carlos Hernandez on 5/9/11.
//  Copyright 2011 Software Factory, LLC. All rights reserved.
//

#pragma once
#include <vector>
#include "ProductionItem.h"

using namespace std;

class ItemSet
{
    vector<ProductionItem*>* items;
    
public:
    
    ItemSet();
    ~ItemSet();
    
    void addProduction(ProductionItem* prod);
    
    vector<ProductionItem*>* getProductionItems();
};