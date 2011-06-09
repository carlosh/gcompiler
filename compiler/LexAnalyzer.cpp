
#include "StdAfx.h"
#include "LexAnalyzer.h"
#include <algorithm>
#include <iostream>
#include <string>
#include "TokenInclude.h"
#include "compilerexception.h"

using namespace std;


LexAnalyzer::LexAnalyzer(void *_buffer, long buffer_size)
{
	colNumber = 1;
	lineNumber = 1;
	currentTokenBytes = 0;
	_currentToken = NULL;
	pascalMode = false;
	currentBufferPosition = 0;
	bufferSize = buffer_size;
	buffer = (char*)malloc(bufferSize + 1);
	memcpy(buffer, _buffer, buffer_size);

	//Fill Reserved Words.
	reservedWords["import"] = &createNewInstance<TokenImport>;
	reservedWords["class"] = &createNewInstance<TokenClass>;
	reservedWords["parse_code"] = &createNewInstance<TokenParserCode>;
	reservedWords["terminal"] = &createNewInstance<TokenTerminal>;
	reservedWords["nonterminal"] = &createNewInstance<TokenNonTerminal>;

}


LexAnalyzer::~LexAnalyzer(void)
{
	delete buffer;
}

int LexAnalyzer::ColNumber()
{
	return colNumber;
}

int LexAnalyzer::LineNumber()
{
	return lineNumber;
}

Token* LexAnalyzer::currentToken()
{
	return _currentToken;
}

Token* LexAnalyzer::nextToken()
{
	_currentToken = internalNextToken();

	//if (typeid(*_currentToken) == typeid(TokenComments)) 
	//	return nextToken();

	return _currentToken;
}

Token* LexAnalyzer::internalNextToken()
{
	int estado = 0;
	currentTokenBytes = 0;
	string lexema = "";
	Token* token = NULL;
	char c;

	while(currentSymbol())
	{
        switch(estado)
        {
			case 0:
				if (isLetter(currentSymbol())) //ID
					estado = 1;
				else if (isSpace(currentSymbol()) || isEnter(currentSymbol()) )
					nextSymbol();
				else if (currentSymbol() == '<')
					estado = 3;
				else if (currentSymbol() == '%') 
					estado = 5;
				else if (currentSymbol() == '{')
					estado = 8;
				else if (currentSymbol() == ',')
					estado = 11;
				else if (currentSymbol() == ';')
					estado = 7;
                else if (currentSymbol() == '-')
					estado = 12;
                else if (currentSymbol() == '|')
					estado = 14;
                else if (currentSymbol() == ':')
                    estado = 15;
                else
					return NULL;
                
				continue;
				break;
			case 1: //ID 1
				c = currentSymbol();
				//lexema.append(1,tolower(c));
				lexema.append(1,c);
                token = new TokenID();
                
				nextSymbol();
                
				if (isLetter(currentSymbol()) || isDigit(currentSymbol()) || currentSymbol() == '_' || currentSymbol() == '<' || currentSymbol() == '>' ||currentSymbol() == '.')
					estado = 2;			
				else
				{
					token->appendToLexema(lexema);
					return checkReservedWords(token);
				}
                
				break;
			case 2: //ID 2
				if (isLetter(currentSymbol()) || isDigit(currentSymbol()) || currentSymbol() == '_' || currentSymbol() == '<' || currentSymbol() == '>' || currentSymbol() == '.')
				{	
					c = currentSymbol();
					//lexema.append(1,tolower(c));
					lexema.append(1,c);
                    nextSymbol();
				}
				else
				{
					token->appendToLexema(lexema);
					return checkReservedWords(token);
				}
				break;
			case 3: //<%
				c = currentSymbol();
				lexema.append(1,c);
				token = new TokenTagOpen();
                
				nextSymbol();
                
				if (currentSymbol() == '%' )
				{
					estado = 4;
				}
                else
                {
                    //error.
                    throw new compilerexception("Error léxico. Esperando '%'");
                    
                }
                
				break;
			case 4: //<% 
				c = currentSymbol();
				lexema.append(1,c);
                
				nextSymbol();
                
				token->appendToLexema(lexema);
				return token;
                
				break;
			case 5: //%>, %
				c = currentSymbol();
                lexema.append(1,c);

				nextSymbol();
                
                if (currentSymbol() == '>' )
				{
                    c = currentSymbol();
                    lexema.append(1,c);
                    token = new TokenTagClose();
                    
                    nextSymbol();
				}
                else
                {
                    token = new TokenDeclaration();
                }
                
                token->appendToLexema(lexema);
				return token;
                
				break;
			case 7: //;
				c = currentSymbol();
				lexema.append(1,c);
                
                token = new TokenSemicolon();
                
				nextSymbol();
                
                token->appendToLexema(lexema);
                return token;
				
				break;
			case 11: //,
				c = currentSymbol();
				lexema.append(1,c);
                
                token = new TokenComma();
                
				nextSymbol();
                
                token->appendToLexema(lexema);
                return token;
				
				break;
			case 8: //{
				c = currentSymbol();
				//lexema.append(1,c);
				
                token = new TokenJavaCode();
                
				nextSymbol();
                
				if (currentSymbol() == '}' )
				{
					estado = 10;
				}
				else
					estado = 9;
                
				break;
			case 9: 
				c = currentSymbol();
				lexema.append(1,c);
                
				nextSymbol();
                
				if (currentSymbol() == '}' )
				{
					estado = 10;
				}
                
				break;
			case 10:
				c = currentSymbol();
				//lexema.append(1,c);
				nextSymbol();
                
				token->appendToLexema(lexema);
				return token;
                
				break;
            case 12: //->
				c = currentSymbol();
				lexema.append(1,c);
				token = new TokenProduction();
                
				nextSymbol();
                
				if (currentSymbol() == '>' )
				{
					estado = 13;
				}
                else
                {
                    //error.
                    throw new compilerexception("Error léxico. Esperando '>'");
                }
                
				break;
			case 13: //->
				c = currentSymbol();
				lexema.append(1,c);
                
				nextSymbol();
                
				token->appendToLexema(lexema);
				return token;
                
				break;
            case 14: //|
				c = currentSymbol();
				lexema.append(1,c);
                
                token = new TokenPipe();
                
				nextSymbol();
                
                token->appendToLexema(lexema);
                return token;
				
				break;
            case 15: //:
				c = currentSymbol();
				lexema.append(1,c);
                
                token = new TokenColon();
                
				nextSymbol();
                
                token->appendToLexema(lexema);
                return token;
				
				break;
                
        }

	}

	//revisar si tonemos un token pendiente
	if (token != NULL)
	{
		token->appendToLexema(lexema);
		return token;
	}

	if (currentSymbol() == 0)
	{
		token = new TokenEof();
		token->appendToLexema("EOF");
		return token;
	}

	return NULL;
}

Token* LexAnalyzer::checkReservedWords(Token* token)
{
	map_type::iterator it = reservedWords.find(token->lexema());
    if(it == reservedWords.end())
		return token;

	Token *newToken = it->second();
	//newToken->appendToLexema(token->lexema());
	newToken->appendToLexema("Reserved(");
	newToken->appendToLexema(token->lexema());
	newToken->appendToLexema(")");
    return newToken;
}

char LexAnalyzer::currentSymbol()
{
	if (currentBufferPosition <= (bufferSize - 1))
	{
		char c = ((char*)buffer)[currentBufferPosition];
		if (c > 0)
			return c;
		else
			return 0;
	}
	else 
		return 0;
}

void LexAnalyzer::reset()
{
    currentBufferPosition = 0;
    currentTokenBytes = 0;
    _currentToken = NULL;
    colNumber = 0;
    lineNumber = 0;
}

void LexAnalyzer::rewindToLastToken()
{
	if (currentTokenBytes <= currentBufferPosition)
	{
		currentBufferPosition -= currentTokenBytes;
		currentTokenBytes = 0;
		_currentToken = NULL;
	}
}

void LexAnalyzer::rewindToLastSymbol()
{
	if (currentBufferPosition > 0)
	{
		currentBufferPosition -= 1;
		currentTokenBytes -= 1;
	}
}


char LexAnalyzer::nextSymbol()
{
	if ( currentBufferPosition <= (bufferSize - 1) )
	{	
		char c = ((char*)buffer)[++currentBufferPosition];
		
		currentTokenBytes++;

		if (isEnter(c))
		{
			lineNumber++;
			colNumber = 1;
		}
		else
			colNumber++;

		if (c > 0)
			return c;
		else
			return 0;
	}
	else
	{
		//--currentBufferPosition;
		return 0;
	}
}

bool LexAnalyzer::isOperator(char c)
{
	if ( c == '-' || c == '+' || c == '*' || c == '/' )
		return 1;
	else
		return 0;
}

bool LexAnalyzer::isDigit(char c)
{
	return (bool)isdigit(c);
}

bool LexAnalyzer::isHexDigit(char c)
{
	if (isDigit(c) || (c >= 'A' && c <= 'F') || (c >= 'a' || c <= 'f'))
	{
		return 1;
	}
	return 0;
}

bool LexAnalyzer::isOctal(char c)
{
	if (isDigit(c))
	{
		if (c >= '0' && c <= '7' )
			return 1;
	}
	return 0;
}

bool LexAnalyzer::isLetter(char c)
{
	return (bool)isalpha(c);
}

bool LexAnalyzer::isSpace(char c)
{
	if ( c == ' ' || c == 9 )
		return 1;
	else
		return 0;
}

bool LexAnalyzer::isEnter(char c)
{
	if ( c == '\n' || c == '\r' )
		return 1;
	else
		return 0;
}