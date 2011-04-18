#pragma once

#include "Token.h"
#include <string>
#include <map>


using namespace std;


template<typename T> Token * createNewInstance() { return new T; };
typedef std::map<std::string, Token*(*)()> map_type;


class LexAnalyzer
{
private:

		
	
	map_type reservedWords;
	char *buffer;
	long bufferSize;
	int lineNumber;
	int colNumber;
	int currentBufferPosition;
	bool pascalMode;
	int currentTokenBytes;

	Token* _currentToken;
	char currentSymbol();
	char nextSymbol();
	Token* internalNextToken();
	void rewindToLastSymbol();

public:
	LexAnalyzer(void *buffer, long buffer_size);
	~LexAnalyzer(void);
	Token* nextToken();
	

	int LineNumber();
	int ColNumber();
	void rewindToLastToken();
	Token* currentToken();


	//symbol check functions
	bool isOperator(char c);
	bool isHexDigit(char c);
	Token* checkReservedWords(Token* token);
	bool isOctal(char c);
	bool isDigit(char c);
	bool isLetter(char c);
	bool isSpace(char c);
	bool isEnter(char c);
    
    void reset();

};

