#pragma once
#include <vector>
#include <ext/hash_map>
#include <string>
#include "Tipo.h"
#include "Cadena.h"
#include "Caracter.h"
#include "Integer.h"
#include "Booleano.h"
#include "Flotante.h"
#include "ContextSimbols.h"

using namespace std;


#define GLOBAL_CONTEXT "global"


typedef __gnu_cxx::hash_map<string, ContextSymbols*> map_context_type;

class SemanticInfo
{
	static SemanticInfo* instance;

	
public:
	map_context_type tablaContextos;
	map_table_type tablaTipos;
	string context;

	SemanticInfo(void);
	~SemanticInfo(void);
	

	static SemanticInfo* getInstance()
	{
		if ( instance == NULL )
		{
			instance = new SemanticInfo();
		}

		return instance;
	}

	void setContext(string ct);
	string getContext();

	Tipo* getTipo(string name);
	void insertTipo(string name, Tipo* tipo);

	ContextSymbols* getContextTable(void);
	ContextSymbols* getContextTable(string name);

	void addContext(string name, ContextSymbols* contextSymbol);

	Tipo* getTipoFromContext(string name);
};




