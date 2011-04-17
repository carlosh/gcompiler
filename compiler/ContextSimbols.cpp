#include "StdAfx.h"
#include "ContextSimbols.h"
#include "Cadena.h"



ContextSymbols::ContextSymbols(void)
{
}


ContextSymbols::~ContextSymbols(void)
{
}


Tipo* ContextSymbols::getSimbolo(string name)
{
	//this->insertSimbolo("c", new Cadena());
	//tablaSimbolos.insert(pair_def("cc", new Cadena()));


	//map_table_type prueba;

	//prueba[name] = new Tipo();

	//tablaSimbolos[name] = new Tipo();

    /*
    __gnu_cxx::hash_map<string, Tipo*>::iterator iter;
	iter = tablaSimbolos.find(name);

	if (iter != tablaSimbolos.end())
		return iter->second;
	else
     
     */
		return NULL;
}

void ContextSymbols::insertSimbolo(string name, Tipo* tipo)
{
	//tablaSimbolos[name] = tipo;
	//tablaSimbolos->insert(pair_def_3(name, tipo));
}
