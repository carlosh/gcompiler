#pragma once
#include "tipo.h"
class Caracter :
	public Tipo
{
public:
	Caracter(void);
	~Caracter(void);


	bool EsEquivalente(Tipo* tipo);

};

