#pragma once

#include <string>
#include "statement.h"
#include "SemanticInfo.h"
#include "ContextSimbols.h"

using namespace std;

class FunctionCode :
	public Statement
{
	Statement* code;
	string nombreFuncion;
public:
	FunctionCode(Statement* next, Statement* _code, string nombre);
	FunctionCode(void);
	~FunctionCode(void);

	string getNombreFuncion();

	void validarSemantico();
	void interpretar();

};

