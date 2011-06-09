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
#include "production.h"
#include "ItemSet.h"
#include "State.h"
#include "Transition.h"
#include "TableLR.h"
#include "stdafx.h"
#include <iostream>
#include <stdio.h>

using namespace std;

#define EPSILON "_epsilon"
#define START  "$START"
#define EOFT  "$"
#define CODEPREFIX "CODIGO$"


typedef std::map<int, ItemSet*> itemset_type;


class codegenerator
{
    
private:
    
    TablaLR* tablaLR;
    std::map<int, State*> listaEstados;
    int currentItemSet;
    itemset_type itemSetList;
    LexAnalyzer* lex;
    vector<production*> gramatica;
    bool importCode;
    bool parseCode;
    bool classDeclaration;
    
    string className;
    
    vector<nonterminal*> nonterminalList;
    vector<terminal*> terminalList;
    
    int estadoSiguiente;
    
    string fileName;
    
    int codeID;
    
    void aumentar();

public:

    
    codegenerator(LexAnalyzer* tlex);
    
    void start();
    void tagOpen();
    void importTag();
    void parseCodeTag();
    void tagDeclaration();
    void tagTerminal();
    void tagNonTerminal();
    void tagProductions();
    void parseSymbols(nonterminal* symbol);
    
    
    bool isNonTerminalInProduction(string id);
    symbol* getSymbol(string id);
    terminal* getTerminal(string id);
    nonterminal* getNonTerminal(string id);
    bool isTerminal(string id);
    bool isNonTerminal(string id);
    bool isSymbolDeclared(string id);
    
    bool productionKernelListEqual(vector<ProductionItem*> listA, vector<ProductionItem*> listB);
    
    void cerradura(ItemSet* item);
    void printProduction(production* prod, int dotPosition);
    void processItem(ItemSet* item);
    //vector<symbol*> getLookAhead(symbol* sym);
    vector<terminal*> primero(symbol* sym);
    
    bool checkIfItemSetExists(ItemSet* itemSet, int* estadoID);
    void moveIraTo(State *estadoFrom, State *estadoTo);
    
    vector<ProductionItem*> uniqueKernelProductionList(ItemSet* set);
    
    void mergeStates(State* baseState, State* toMerge);
    
    ItemSet* ir_a(ItemSet* itemSet, symbol* sym);
    
    bool productionItemEqual(ProductionItem* prodItemA, ProductionItem* prodItemB, bool ignoreDotPosition, bool ignoreLookAhead);
    bool productionItemEqualForMerge(ProductionItem* prodItemA, ProductionItem* prodItemB, bool ignoreDotPosition, bool ignoreLookAhead);
    
    
    //bool productionEqual(production* a, production* b);
    bool itemSetEqual(ItemSet *itemSetA, ItemSet *itemSetB);
    
    void PrintItemSets();
    
    
    
    ItemSet* getItemSetForState(ItemSet *itemSet);
    
    void generateJavaFile();
    void generateJavaSymFile();
    
    int getReducirLocation(production* prod);
    
    bool itemSetEqualForState(ItemSet *itemSetA, ItemSet *itemSetB);
    
    void moveTransitions(State* estadoFrom, State* estadoTo);
    void moveTransitionsTo(State* estadoFrom, State* estadoTo);
    void moveAccionesTo(State* estadoFrom, State* estadoTo);
    void moveAcciones(State* estadoFrom, State* estadoTo);
    void moveIra(State* estadoFrom, State* estadoTo);
    
    bool moveProductionFromItemSet(ItemSet* itemSetFrom, ItemSet* itemSetTo);
    
    void printGramatica();
    int terminalPosition(terminal* t);
    
    void minimizar();
    
    void printStates();
    
    long getNextStateID();
    
    bool epsilonFoundIn(vector<terminal*> lista);
    
    void insertarProducciones(FILE* file);
    void insertarEstados(FILE* file);
    void insertarExecuteCodeMethod(FILE* file);
    
    
    
    

};