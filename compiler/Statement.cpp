#include "StdAfx.h"
#include "Statement.h"
#include "FunctionCode.h"

Statement::Statement(Statement* next)
{
	nextStatement = next;
}


Statement::Statement(void)
{
	 nextStatement = NULL;
}

Statement::~Statement(void)
{
}

Statement* Statement::getNextStatement()
{
	return nextStatement;
}

void Statement::setNextStatement(Statement* next)
{
	nextStatement = next;
}


void Statement::validarSemantico()
{
}


void Statement::validacionSemantica()
{
	bool contextChanged = false;
	if (typeid(*this) == typeid(FunctionCode))
	{
		contextChanged = true;
		FunctionCode* fc = (FunctionCode*)this;

		//change context.
		SemanticInfo::getInstance()->setContext(fc->getNombreFuncion());

	}

	validarSemantico();
	if (nextStatement != NULL)
		nextStatement->validacionSemantica();

	if (contextChanged)
		SemanticInfo::getInstance()->setContext(GLOBAL_CONTEXT);
}


void Statement::interpretacion()
{
	//bool contextChanged = false;

	if (typeid(*this) == typeid(FunctionCode))
	{
		//contextChanged = true;
		//FunctionCode* fc = (FunctionCode*)this;

		//change context.
		//InterpretacionInfo::getInstance()->setContext(fc->getNombreFuncion());

	}

	interpretar();

	//if (contextChanged)
	//	InterpretacionInfo::getInstance()->setContext(GLOBAL_CONTEXT_VALOR);


	if (nextStatement != NULL)
		nextStatement->interpretacion();

}


void Statement::interpretar()
{
}