#pragma once




class Statement
{
	Statement* nextStatement;
public:
	Statement(Statement* next);
	Statement(void);
	~Statement(void);

	Statement* getNextStatement();
	void setNextStatement(Statement* next);

	virtual void validarSemantico();
	virtual void interpretar();

	void validacionSemantica(void);
	void interpretacion(void);


};

