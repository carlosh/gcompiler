#include "StdAfx.h"
#include "Integer.h"


Integer::Integer(void)
{
}


Integer::~Integer(void)
{
}


bool Integer::EsEquivalente(Tipo* tipo)
{
	return (typeid(*tipo) == typeid(Integer));
}