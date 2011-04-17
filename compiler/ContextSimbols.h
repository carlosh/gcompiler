#pragma once

#include "Tipo.h"
#include <string>
#include <ext/hash_map>


using namespace std;


#define pair_def_3(a, b) std::pair<std::string, Tipo*>(a, b)

#define pair_def(a, b) std::pair<std::string, Tipo*>(a, b)
#define pair_def_context(a, b) std::pair<std::string, ContextSymbols*>(a, b)


typedef __gnu_cxx::hash_map<string, Tipo*> map_table_type;
typedef __gnu_cxx::hash_map<std::string, Tipo*> map_table_type_3;


//typedef std::unordered_map<std::string, Tipo*> map_table_type_2;


class ContextSymbols
{
	map_table_type tablaSimbolos;

public:
	ContextSymbols(void);
	~ContextSymbols(void);


	void insertSimbolo(string name, Tipo* tipo);
	Tipo* getSimbolo(string name);

};

