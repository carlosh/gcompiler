//
//  Transition.cpp
//  compiler
//
//  Created by Carlos Hernandez on 5/8/11.
//  Copyright 2011 Software Factory, LLC. All rights reserved.
//

#include "Transition.h"


Transition::Transition(symbol* _consume, State* _toState)
{
    consume = _consume;
    toState = _toState;
}



State* Transition::getState()
{
    return toState;
}

symbol* Transition::getConsume()
{
    return consume;
}


void Transition::setState(State *newState)
{
    toState = newState;
}