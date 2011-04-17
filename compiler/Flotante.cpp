#include "StdAfx.h"
#include "Flotante.h"


Flotante::Flotante(void)
{
}


Flotante::~Flotante(void)
{
}


bool Flotante::EsEquivalente(Tipo* tipo)
{
	return (typeid(*tipo) == typeid(Flotante));
}