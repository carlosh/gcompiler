#pragma once
#include "tipo.h"


class Flotante :
	public Tipo
{
public:
	Flotante(void);
	~Flotante(void);

	bool EsEquivalente(Tipo* tipo);
};

