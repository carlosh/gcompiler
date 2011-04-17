

#include "StdAfx.h"
#include "Token.h"
#include <typeinfo>

using namespace std;

Token::Token(void)
{
	_lexema = "";
}


Token::~Token(void)
{
}

string Token::lexema()
{
	return _lexema;
}

void Token::appendToLexema(const char* c)
{
	_lexema.append(c);
}

void Token::appendToLexema(string _str)
{
	_lexema.append(_str);
}


const char* Token::className()
{
	return typeid(*this).name();
}





