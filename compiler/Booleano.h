#pragma once
#include "tipo.h"
class Booleano :
	public Tipo
{
public:
	Booleano(void);
	~Booleano(void);

	bool EsEquivalente(Tipo* tipo);
};

