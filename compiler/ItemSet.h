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
    int id;
    vector<ProductionItem*>* items;
    bool markedForDelete;
    
public:
    
    ItemSet();
    ~ItemSet();
    
    void addProduction(ProductionItem* prod);
    
    vector<ProductionItem*>* getProductionItems();
    
    void setId(int _id);
    int getId();
    bool getIsMarkedForDelete();
    void setIsMarkedForDelete(bool b);
    void setStateIDToItems(int id);
    
    //vector<ProductionItem*> uniqueKernelProductionList();
    
};