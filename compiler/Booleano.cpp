#include "StdAfx.h"
#include "Booleano.h"


Booleano::Booleano(void)
{
}


Booleano::~Booleano(void)
{
}

bool Booleano::EsEquivalente(Tipo* tipo)
{
	return (typeid(*tipo) == typeid(Booleano));
}
