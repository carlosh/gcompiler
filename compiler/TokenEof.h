//
//  TokenEof.h
//  compiler
//
//  Created by Carlos Hernandez on 4/17/11.
//  Copyright 2011 Software Factory, LLC. All rights reserved.
//



#pragma once
#include "token.h"

class TokenEof :
public Token
{
public:
	TokenEof(void);
	~TokenEof(void);
};
