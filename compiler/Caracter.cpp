#include "StdAfx.h"
#include "Caracter.h"


Caracter::Caracter(void)
{
}


Caracter::~Caracter(void)
{
}

bool Caracter::EsEquivalente(Tipo* tipo)
{
	return (typeid(*tipo) == typeid(Caracter));
}