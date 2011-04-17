//
//  codegenerator.h
//  compiler
//
//  Created by Carlos Hernandez on 4/16/11.
//  Copyright 2011 Software Factory, LLC. All rights reserved.
//

#include "LexAnalyzer.h"
#include "production.h"
#include <vector>
#include "terminal.h"
#include "nonterminal.h"

class codegenerator
{
    
private:
    
    LexAnalyzer* lex;
    vector<production*> gramatica;
    bool importCode;
    bool parseCode;
    bool classDeclaration;
    
    string className;
    
    vector<nonterminal*> nonterminalList;
    vector<terminal*> terminalList;

public:

    
    codegenerator(LexAnalyzer* tlex);
    
    void start();
    void tagOpen();
    void importTag();
    void parseCodeTag();
    void tagDeclaration();
    void tagTerminal();
    void tagNonTerminal();
    
    bool isIdInList(string id);
};