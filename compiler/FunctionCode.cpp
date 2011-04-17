#include "StdAfx.h"
#include "FunctionCode.h"


FunctionCode::FunctionCode(Statement* next, Statement* _code, string nombre)
{
	//Statement::Statement(next);

	code = _code;

	nombreFuncion = nombre;

	ContextSymbols* ct = new ContextSymbols();

	SemanticInfo::getInstance()->addContext(nombreFuncion, ct);
}

FunctionCode::FunctionCode(void)
{
}


FunctionCode::~FunctionCode(void)
{
}

string FunctionCode::getNombreFuncion()
{
	return nombreFuncion;
}

void FunctionCode::validarSemantico()
{
	SemanticInfo::getInstance()->addContext(nombreFuncion, new ContextSymbols());
}

void FunctionCode::interpretar()
{
	//InterpretacionInfo::getInstance()->addContext(nombreFuncion, new ContextValor());


	code->interpretar();
}