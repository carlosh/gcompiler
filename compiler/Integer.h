#pragma once
#include "tipo.h"
class Integer :
	public Tipo
{
public:
	Integer(void);
	~Integer(void);

	bool EsEquivalente(Tipo* tipo);
};

