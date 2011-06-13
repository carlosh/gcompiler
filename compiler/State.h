//
//  State.h
//  compiler
//
//  Created by Carlos Hernandez on 5/8/11.
//  Copyright 2011 Software Factory, LLC. All rights reserved.
//

#pragma once
#include <vector>
#include "terminal.h"
#include "accion.h"
#include "ira.h"
#include "nonterminal.h"
#include <stdio.h>
#include <map>
#include "ItemSet.h"
#include "Transition.h"

using namespace std;

typedef pair<terminal*, Accion*> pair_accion;
typedef pair<nonterminal*, Ira*> pair_ira;


class State
{
    
    vector<Accion*> listaAcciones;
    vector<Ira*> listaIra;
    vector<Transition*> transitions;
    bool isAccept;
    int id;
    ItemSet* itemSet;
    string mergeStateList;
    bool markedForDelete;
    bool isProcessed;
    
public:
    
    State(bool isAccept, int id);
    
    void setIsAccept(bool isAcceptState);
    
    bool isAcceptState();
    
    
    void addTransition(Transition* transition);
    vector<Transition*> getTransitions();
    int getID();
    
    void setMergeStateList(string list);
    string getMergeStateList();
    
    void addAccion(Accion* acc);
    void addIra(Ira* ir);
    
    void setItemSet(ItemSet* s);
    ItemSet* getItemSet();
    
    bool getIsProcessed();
    void setIsProcessed(bool b);
    
    vector<Accion*> getListaAcciones();
    vector<Ira*> getListaIra();
    
    bool accionExists(Accion* accion);
    bool iraExists(Ira* ira);
    
    void setMarkedForDelete(bool m);
    bool getMarkedForDelete();
    
};