//
//  Transition.h
//  compiler
//
//  Created by Carlos Hernandez on 5/8/11.
//  Copyright 2011 Software Factory, LLC. All rights reserved.
//

#pragma once
#include "terminal.h"

class State;

class Transition
{
    symbol* consume;
    State* toState;
    
public:
    
    Transition(symbol* _consume, State* _toState);
    
    State* getState();
    symbol* getConsume();
    void setState(State* newState);
};