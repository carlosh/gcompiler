#include "StdAfx.h"
#include "SemanticInfo.h"


SemanticInfo* SemanticInfo::instance = NULL;

SemanticInfo::SemanticInfo(void)
{
	//tablaTipos.insert(pair_def("STRING", new Cadena()));
	//tablaTipos.insert(pair_def("FLOAT", new Flotante()));
	//tablaTipos.insert(pair_def("CHAR", new Caracter()));
	//tablaTipos.insert(pair_def("INTEGER", new Integer()));
	//tablaTipos.insert(pair_def("BOOL", new Booleano()));

	context = GLOBAL_CONTEXT;

	//add first context table.

	ContextSymbols* contextSymbol = new ContextSymbols();

	
	addContext(context, contextSymbol);
}

SemanticInfo::~SemanticInfo(void)
{
}


void SemanticInfo::setContext(string ct)
{
	context = ct;
}


string SemanticInfo::getContext()
{
	return context;
}

void SemanticInfo::addContext(string name, ContextSymbols* contextSymbol)
{
	//tablaContextos.insert(pair_def_context(name, contextSymbol));
}

void SemanticInfo::insertTipo(string name, Tipo* tipo)
{
	//tablaTipos.insert(pair_def(name, tipo));
}

Tipo* SemanticInfo::getTipo(string name)
{
    /*__gnu_cxx::hash_map<string, Tipo*>::iterator iter;
	iter = tablaTipos.find(name);

	if (iter != tablaTipos.end())
		return iter->second;
	else
     
     */
		return NULL;
}

Tipo* SemanticInfo::getTipoFromContext(string name)
{
	Tipo* tipo = NULL;

	if (context != GLOBAL_CONTEXT)
	{
		tipo = this->getContextTable(context)->getSimbolo(name);
	}

	if ( tipo != NULL )
	{
		tipo = this->getContextTable(GLOBAL_CONTEXT)->getSimbolo(name);
	}

	return tipo;
}

ContextSymbols* SemanticInfo::getContextTable(string name)
{
	//ContextSymbols* contextValor = getContextValorTable(context);

	//if (contextValor == NULL)
	//	addContext(contextName, new ContextValor());


    /*__gnu_cxx::hash_map<string, ContextSymbols*>::iterator iter;
	iter = tablaContextos.find(name);

	if (iter != tablaContextos.end())
		return iter->second;
	else
		return NULL;
*/
    
    return NULL;
}

ContextSymbols* SemanticInfo::getContextTable()
{
	return getContextTable(context);
}
