#include "StdAfx.h"
#include "Cadena.h"


Cadena::Cadena(void)
{
}


Cadena::~Cadena(void)
{
}


bool Cadena::EsEquivalente(Tipo* tipo)
{
	return (typeid(*tipo) == typeid(Cadena));
}