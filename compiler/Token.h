#pragma once
#include <string>
//#include <typeinfo.h>


using namespace std;


class Token
{
private:
	string _lexema;

public:
	Token(void);
	~Token(void);

	string lexema();
	void appendToLexema(const char* c);
	void appendToLexema(string _str);
	const char* className();
	virtual void f(){}; //Just to make typeid returned the derived type correclty uss=ing rtti.
};

