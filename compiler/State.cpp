//
//  State.cpp
//  compiler
//
//  Created by Carlos Hernandez on 5/8/11.
//  Copyright 2011 Software Factory, LLC. All rights reserved.
//

#include "State.h"
#include "compilerexception.h"


#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

string to_string2(int i)
{
    std::string s;
    std::stringstream out;
    out << i;
    s = out.str();
    
    return s;
}

State::State(bool accept, int _id)
{
    isAccept = accept;
    id = _id;
    markedForDelete = false;
    isProcessed = false;
}


void State::setIsAccept(bool isAcceptState)
{
    isAccept = isAcceptState;
}

bool State::isAcceptState()
{
    return isAccept;
}

int State::getID()
{
    return id;
}

bool State::getMarkedForDelete()
{
    return markedForDelete;
}

void State::setMarkedForDelete(bool m)
{
    markedForDelete = m;
}


bool State::accionExists(Accion *accion)
{
    long total = listaAcciones.size();
    
    //map<terminal*, Accion*>::iterator iterAcciones = estado->getListaAcciones().begin();
    
    for (long j = 0; j < total; j++) {
        
        
        //terminal* ter = (*iterAcciones).first;
        Accion* accionLista = listaAcciones[j];
        
        if ( accionLista->getTerminal()->getID() == accion->getTerminal()->getID() )
            return true;
    }
    
    return false;
}

bool State::iraExists(Ira *ira)
{
    long total = listaIra.size();
    
    //map<terminal*, Accion*>::iterator iterAcciones = estado->getListaAcciones().begin();
    
    for (long j = 0; j < total; j++) {
        
        
        //terminal* ter = (*iterAcciones).first;
        Ira* iraLista = listaIra[j];
        
        if ( iraLista->getNonTerminal()->getID() == ira->getNonTerminal()->getID() )
            return true;
    }
    
    return false;
}




void State::addTransition(Transition* transition)
{
    vector<Transition*>::iterator itt;
    
    
    bool found = false;
    for (itt = transitions.begin(); itt != transitions.end(); ++itt) {
        
        Transition* trans = *itt;
        
        if ( trans->getConsume()->getID() == transition->getConsume()->getID() && trans->getState() == transition->getState() )
        {
            found = true;
            break;
        }
    }
    
    if (!found)
        transitions.push_back(transition);
}


vector<Transition*> State::getTransitions()
{
    return transitions;
}

void State::setMergeStateList(string list)
{
    mergeStateList = list;
}

string State::getMergeStateList()
{
    return mergeStateList;
}


void State::addIra(Ira *ir)
{
    if (!iraExists(ir))
        listaIra.push_back(ir);
    else
    {
        throw new compilerexception("Ira estado " + to_string2(ir->getEstadoID()) + " esta duplicado con el non terminal : " + ir->getNonTerminal()->getID());
    }
    
    
}

void State::addAccion(Accion *acc)
{
    if (!accionExists(acc))
        listaAcciones.push_back(acc);
    else
    {
        //error
        string tipoAccion = (acc->getTipo() == 0?"Desplazar":(acc->getTipo() == 1?"Reducir":"Aceptar"));
        throw new compilerexception("La acción de " + tipoAccion + " esta duplicada con el terminal : " + acc->getTerminal()->getID());
    }    
}

void State::setIsProcessed(bool b)
{
    isProcessed = b;
}

bool State::getIsProcessed()
{
    return isProcessed;
}

void State::setItemSet(ItemSet *s)
{
    itemSet = s;
}

ItemSet* State::getItemSet()
{
    return itemSet;
}

vector<Ira*> State::getListaIra()
{
    return listaIra;
}

vector<Accion*> State::getListaAcciones()
{
    return listaAcciones;
}