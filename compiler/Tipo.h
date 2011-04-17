#pragma once

#include <typeinfo>

class Tipo
{
public:
	Tipo(void);
	~Tipo(void);

	bool operator<(const Tipo& right) const
        {
            return true;
        }

	virtual bool EsEquivalente(Tipo* tipo);
};

