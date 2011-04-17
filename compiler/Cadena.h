#pragma once
#include "tipo.h"
class Cadena :
	public Tipo
{
public:
	Cadena(void);
	~Cadena(void);

	bool EsEquivalente(Tipo* tipo);
};

