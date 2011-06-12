//
//  codegenerator.cpp
//  compiler
//
//  Created by Carlos Hernandez on 4/16/11.
//  Copyright 2011 Software Factory, LLC. All rights reserved.
//

#include "codegenerator.h"
#include "TokenInclude.h"
#include <typeinfo>
#include "compilerexception.h"


#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

string to_string(int i)
{
    std::string s;
    std::stringstream out;
    out << i;
    s = out.str();
    
    return s;
}


codegenerator::codegenerator(LexAnalyzer* tlex)
{
    lex = tlex;
    importCode = false;
    parseCode = false;
    classDeclaration = false;
    className = "";
    currentItemSet = 0;
    codeID = 0;
    
    terminalList.push_back(new terminal(EPSILON, "String"));
}


void codegenerator::start()
{
    /*
     int i;
     for( i = 0; i < buffer_size; ++i )
     {
     printf("%c", ((char*)buffer)[i]);
     }
     */
	
	try
	{
        
        printf("Parseando archivo...\n");
        
		Token* token;
		while((token = lex->nextToken()))
		{
			//printf("%s -> %s \r\n", token->lexema().c_str(), token->className());
            
			
            if ( typeid(*token) == typeid(TokenTagOpen) )
                tagOpen();
            else if ( typeid(*token) == typeid(TokenDeclaration) )
                tagDeclaration();
            else if ( typeid(*token) == typeid(TokenTerminal) )
                tagTerminal();
            else if ( typeid(*token) == typeid(TokenNonTerminal) )
                tagNonTerminal();
            else if ( typeid(*token) == typeid(TokenID) )
                tagProductions();
            
            
            if ( typeid(*token) == typeid(TokenEof) )
				break;
		}
        
        //check if every nonterminal has a production.
        vector<nonterminal*>::iterator iterNonTerminal;
        
        for (iterNonTerminal = nonterminalList.begin(); iterNonTerminal != nonterminalList.end(); ++iterNonTerminal) {
            
            string nonTerminalID = (*iterNonTerminal)->getID();
          
            if (!isNonTerminalInProduction(nonTerminalID))
                throw new compilerexception("Non Terminal (" + nonTerminalID + ") has no productions.");
            
        }
        
        //add $ terminal symbol
        terminalList.push_back(new terminal("$"));
        

        aumentar();
        
        printGramatica();
        
        
        
        //next steps.
        
        ItemSet* item0 = new ItemSet();
        
        ProductionItem* tempProd = new ProductionItem(gramatica[0], this->getTerminal("$"), true);
        tempProd->setPassId(0);
        item0->addProduction(tempProd);
        itemSetList[0] = item0;
        item0->setId(0);
        
        
        cerradura(item0);
        
        item0->setStateIDToItems(0);
        
        //crear estado inicial.
        State* estado = new State(false, 0);
        
        State* inicio = estado;
        State* currentState = estado;
        
        listaEstados[0] = inicio;
        
        int estadoActual = 0;
        estadoSiguiente = 1;
        
        printf("Creando estados...\n");
        
        vector<symbol*> symbols;
        
        symbols.insert(symbols.end(), nonterminalList.begin(), nonterminalList.end());
        symbols.insert(symbols.end(), terminalList.begin(), terminalList.end());
        
        
        
        estadoActual = 0;
        
        
        do
        {
            currentState = listaEstados.find(estadoActual)->second;
            
            ItemSet* currentSet = itemSetList.find(estadoActual)->second;
            
            currentState->setItemSet(currentSet);
            
            currentState->setIsProcessed(true);
            vector<ProductionItem*>::iterator iterProdItem;
            
            long totalItems = currentSet->getProductionItems()->size();
            
            //for (iterProdItem = currentSet->getProductionItems().begin(); iterProdItem != currentSet->getProductionItems().end(); ++iterProdItem) {
            for (long i = 0; i < totalItems; i++) {
                
                ProductionItem* prodItem = (*currentSet->getProductionItems())[i];  //((ProductionItem*)*iterProdItem);
                if (!prodItem->getIsGotoProcessed())
                {
                    prodItem->setGotoProcessed(true);
                    symbol* sym = prodItem->nextSymbol();
                    
                    if ( sym != NULL && sym->getID() == EPSILON )
                    {
                        //add reduce
                        int reducePosition = getReducirLocation(prodItem->getProduction()); 
                        
                        
                        if (prodItem->getProduction()->getVariable()->getID() == START)
                        { 
                            Accion* accion = new Accion(ACCEPT, reducePosition, prodItem->getLookAhead());
                            currentState->addAccion(accion);    
                        }
                        else if (reducePosition > 0)
                        {
                            Accion* accion = new Accion(REDUCE, reducePosition, prodItem->getLookAhead());
                            currentState->addAccion(accion);
                        }
                        
                        continue;
                    }
                    
                    
                    if (sym != NULL)
                    {
                        ItemSet* newSet = ir_a(currentSet, sym);
                        
                        
                        //verificamos si ese set no existe ya generado.
                        if (newSet->getProductionItems()->size() > 0)
                        {   
                            int estadoFoundID = 0;
                            bool itemExists = checkIfItemSetExists(newSet, &estadoFoundID); 
                            if (!itemExists)
                            {
                                itemSetList[estadoSiguiente] = newSet;
                                bool isAccept = false;
                                
                                if (estadoSiguiente == 1)
                                    isAccept = true;
                                
                                State* estadoNuevo = new State(isAccept, estadoSiguiente);
                                
                                Transition* transition = new Transition(sym, estadoNuevo->getID());
                                
                                currentState->addTransition(transition);
                                estadoNuevo->setItemSet(newSet);
                                newSet->setStateIDToItems(estadoSiguiente);
                                
                                listaEstados[estadoSiguiente] = estadoNuevo;
                                
                                //printf("Nuevo estado : %d\n", estadoSiguiente);
                                
                                //agregar lista de acciones e ir_a
                                if ( typeid(*sym) == typeid(terminal) )
                                {
                                    Accion* accion = new Accion(SHIFT, estadoNuevo->getID(), (terminal*)sym);
                                    
                                    currentState->addAccion(accion);
                                    
                                }
                                else if ( typeid(*transition->getConsume()) == typeid(nonterminal) )
                                {
                                    Ira* ir = new Ira(estadoNuevo->getID(), (nonterminal*)sym);
                                    
                                    currentState->addIra(ir);
                                }
                                
                                
                                //nos alistamos para el estado siguiente
                                estadoSiguiente++;
                                
                                
                            }
                            else
                            {
                                Transition* transition = new Transition(sym, listaEstados.find(estadoFoundID)->second->getID());
                                currentState->addTransition(transition);
                                
                                //agregar lista de acciones e ir_a
                                if ( typeid(*sym) == typeid(terminal) )
                                {
                                    Accion* accion = new Accion(SHIFT, estadoFoundID, (terminal*)sym);
                                    
                                    currentState->addAccion(accion);
                                }
                                else if ( typeid(*transition->getConsume()) == typeid(nonterminal) )
                                {
                                    Ira* ir = new Ira(estadoFoundID, (nonterminal*)sym);
                                    
                                    currentState->addIra(ir);
                                }
                            }
                        }
                    }
                    else
                    {
                        //add reduce
                        int reducePosition = getReducirLocation(prodItem->getProduction()); 
                        
                        
                        if (prodItem->getProduction()->getVariable()->getID() == START)
                        { 
                            Accion* accion = new Accion(ACCEPT, reducePosition, prodItem->getLookAhead());
                            if (!currentState->accionExists(accion))
                                currentState->addAccion(accion);    
                        }
                        else if (reducePosition > 0)
                        {
                            Accion* accion = new Accion(REDUCE, reducePosition, prodItem->getLookAhead());
                            currentState->addAccion(accion);
                        }
                    }
                    
                }
            }    
            
            
            estadoActual++;
            
            
        } 
        while (estadoActual < estadoSiguiente);
        
        
        //PrintItemSets();
        
        //printStates();
        
        printf("Total estados creados %d\n", estadoActual - 1);
        
        
        
        printf("Minimizando estados...\n");
        
        
        //minimize states
        minimizar();
        
        //printStates();
        
        printf("Total estados despues de minimizar -> %d\n", (int)listaEstados.size());
        
        //create file
        generateJavaFile();
        
        //create sym file
        generateJavaSymFile();
	}
	catch(exception* c)
	{
		printf("Error located at line %d, column %d :%s", lex->LineNumber(), lex->ColNumber(), c->what());
	} 
}


bool codegenerator::moveProductionFromItemSet(ItemSet *itemSetFrom, ItemSet *itemSetTo)
{
    
    bool newProductionsAdded = false;
    
    long totalItems = itemSetFrom->getProductionItems()->size();
    
    for (long j = 0; j < totalItems; j++) {
        
        
        ProductionItem* prodFrom = (*itemSetFrom->getProductionItems())[j];
        
        
        long totalItemsTo = itemSetTo->getProductionItems()->size();
        
        
        bool isEqual = false;
        for (int i = 0; i < totalItemsTo; i++) {
            
            ProductionItem* prodTo = (*itemSetTo->getProductionItems())[i];
            
            if ( productionItemEqual(prodFrom, prodTo, false, false) )
            {
                isEqual = true;
                break;
            }
        }
        
        if (!isEqual)
        {
            //move production item;
            newProductionsAdded = true;
            prodFrom->setPassId(0);
            itemSetTo->addProduction(prodFrom);
        }
        //else
          //  delete prodFrom;
    }
    
    
    return newProductionsAdded;
    
}



void codegenerator::minimizar()
{
    
    vector<State*> estadosToDelete;
   
    map<int, State*>::iterator itFirst;
    
    for (itFirst = listaEstados.begin(); itFirst != listaEstados.end(); ++itFirst) {
        
        State* set1 = itFirst->second;
        
        if (!set1->getMarkedForDelete())
        {
            map<int, State*>::iterator itSecond;
            
            for (itSecond = listaEstados.begin(); itSecond != listaEstados.end(); ++itSecond) {
                
                //for (int j = 0; j < totalStates; j++) {
                
                State* set2 = itSecond->second;
                
                if (!set2->getMarkedForDelete())
                {
                    
                    if ( itFirst->second->getID() != itSecond->second->getID() && !set1->getMarkedForDelete() )
                    {
                        if (productionKernelListEqual(uniqueKernelProductionList(set1->getItemSet()), uniqueKernelProductionList(set2->getItemSet())))
                        {
                            
                            mergeStates(set1, set2);
                            
                            set2->setMarkedForDelete(true);
                            
                            
                            estadosToDelete.push_back(set2);
                            
                            
                        }
                    }
                }
            }
        }
    }
    
    
    
    
    //ahora borramos los estados de la lista de estados.
    long totalStates = estadosToDelete.size();
    
    for (int i = 0; i < totalStates; i++) {
        
        State* set = estadosToDelete[i];
        
        listaEstados.erase(set->getID());
        
    }
}

void codegenerator::setDirectoryPath(string path)
{
    directoryPath = path;
}


void codegenerator::mergeStates(State* baseState, State* toMerge)
{

    moveProductionFromItemSet(toMerge->getItemSet(), baseState->getItemSet());
    moveTransitions(toMerge, baseState);
    moveTransitionsTo(toMerge, baseState);
    moveAccionesTo(toMerge, baseState);
    moveAcciones(toMerge, baseState);
    moveIra(toMerge, baseState);
    moveIraTo(toMerge, baseState);
}

void codegenerator::moveAccionesTo(State *estadoFrom, State *estadoTo)
{
    
    map<int, State*>::iterator itStates;
    
    for (itStates = listaEstados.begin(); itStates != listaEstados.end(); ++itStates) {
        
        //for (int i = 0; i < totalStates; i++) {
        
        State* estado = itStates->second;// listaEstados[i];
      
        
        //for (int i = 0; i < totalStates; i++) {
        
        //State* estado = listaEstados[i];
        
        //if (!estado->getMarkedForDelete())
        {
            long totalAcciones = estado->getListaAcciones().size();
            
            for (long j = 0; j < totalAcciones; j++) {
                
                Accion* accion = estado->getListaAcciones()[j];
                
                if ( accion->getEstadoID() == estadoFrom->getID() && accion->getTipo() == SHIFT )
                {
                    accion->setEstadoID(estadoTo->getID());
                }
            }
        }
        
    }
}

void codegenerator::moveIraTo(State *estadoFrom, State *estadoTo)
{
    
    map<int, State*>::iterator itStates;
    
    for (itStates = listaEstados.begin(); itStates != listaEstados.end(); ++itStates) {
        
        //for (int i = 0; i < totalStates; i++) {
        
        State* estado = itStates->second;// listaEstados[i];
        
        
        //for (int i = 0; i < totalStates; i++) {
        
        //State* estado = listaEstados[i];
        
        //if (!estado->getMarkedForDelete())
        {
            long totalIra = estado->getListaIra().size();
            
            for (long j = 0; j < totalIra; j++) {
                
                Ira* ira = estado->getListaIra()[j];
                
                if ( ira->getEstadoID() == estadoFrom->getID() )
                {
                    ira->setEstadoID(estadoTo->getID());
                }
            }
        }
        
    }
}


void codegenerator::moveAcciones(State *estadoFrom, State *estadoTo)
{
    //recorremos las transiciones de ambos estados y las agregamos al nuevo estado.
    //agregar transiciones
    long totalAcciones = estadoFrom->getListaAcciones().size();
    
    for (long j = 0; j < totalAcciones; j++) {
        
        Accion* accion = estadoFrom->getListaAcciones()[j];
        
        
        if (!estadoTo->accionExists(accion))
            estadoTo->addAccion(accion);
    }
    
    estadoFrom->getListaAcciones().clear();
}

void codegenerator::moveIra(State *estadoFrom, State *estadoTo)
{
    //recorremos las transiciones de ambos estados y las agregamos al nuevo estado.
    //agregar transiciones
    long totalIra = estadoFrom->getListaIra().size();
    
    for (long j = 0; j < totalIra; j++) {
        
        Ira* ira = estadoFrom->getListaIra()[j];
        
        
        if (!estadoTo->iraExists(ira))
            estadoTo->addIra(ira);
    }
    
    estadoFrom->getListaIra().clear();
}


void codegenerator::moveTransitionsTo(State *estadoFrom, State *estadoTo)
{
    map<int, State*>::iterator itStates;
    
    for (itStates = listaEstados.begin(); itStates != listaEstados.end(); ++itStates) {
        
        //for (int i = 0; i < totalStates; i++) {
        
        State* estado = itStates->second;// listaEstados[i];
        
        
        
        long totalTransiciones = estado->getTransitions().size();
        
        for (long j = 0; j < totalTransiciones; j++) {
            
            Transition* transition = estado->getTransitions()[j];
            
            if ( transition->getState() == estadoFrom->getID() )
            {
                transition->setState(estadoTo->getID());
            }
        }
        
        
    }
    
}



void codegenerator::moveTransitions(State *estadoFrom, State *estadoTo)
{
    //recorremos las transiciones de ambos estados y las agregamos al nuevo estado.
    //agregar transiciones
    long totalTransiciones = estadoFrom->getTransitions().size();
        
    for (long j = 0; j < totalTransiciones; j++) {
    
        Transition* transition = estadoFrom->getTransitions()[j];
        
        estadoTo->addTransition(transition);        
    }
    
    estadoFrom->getTransitions().clear();
}

long codegenerator::getNextStateID()
{
    return estadoSiguiente;
}


void codegenerator::printGramatica()
{
    long totalProducciones = gramatica.size();
    for (int i = 0; i < totalProducciones; i++) {
        
        production* fromLista = gramatica[i];
        
        printf("[%d] ", i);
        printProduction(fromLista, -1);
        printf("\n");
    }             
}

void codegenerator::PrintItemSets()
{
    
    map<int, ItemSet*>::iterator itStates;
    
    //long totalStates = listaEstados.size();
    
    int i = 0;
    for (itStates = itemSetList.begin(); itStates != itemSetList.end(); ++itStates) {
        
        
        //for (int i = 0; i < totalStates; i++) {
        
        
        ItemSet* itemSet = itStates->second;
        
        vector<ProductionItem*>::iterator iterProdItem;
        
        
        for (iterProdItem = (*itemSet->getProductionItems()).begin(); iterProdItem != (*itemSet->getProductionItems()).end(); ++iterProdItem) {
            
            ProductionItem* prodItem = ((ProductionItem*)*iterProdItem);// (*itemSet->getProductionItems())[j];
            
            printf("I%d ", i);
            
            printf("%s", prodItem->getIsKernel()?"(k)":"");
            
            printProduction(prodItem->getProduction(), prodItem->getDotPosition());
            
            printf("/ %s", (prodItem->getLookAhead()->getID().c_str()));
            
            printf(" -- %s", prodItem->getIsGotoProcessed()?"YES":"NO");
            
            
            printf("\n");
        }
        
        printf("\n\n\n");

        i++;
        
    }
}


void codegenerator::printStates()
{
    
    map<int, State*>::iterator itStates = listaEstados.begin();
    
    //long totalStates = listaEstados.size();
    
    int i = 0;
    for (itStates = listaEstados.begin(); itStates != listaEstados.end(); ++itStates) {
        
        
        //for (int i = 0; i < totalStates; i++) {
        
        State* estado = itStates->second;// listaEstados[i];//  ;
        
        ItemSet* itemSet = estado->getItemSet();
        
        vector<ProductionItem*>::iterator iterProdItem;
        
        long total = itemSet->getProductionItems()->size();
        
        //for (long j = 0; j < total; j++) {
            
        int lines = 0;
        for (iterProdItem = (*itemSet->getProductionItems()).begin(); iterProdItem != (*itemSet->getProductionItems()).end(); ++iterProdItem) {
            
            ProductionItem* prodItem = ((ProductionItem*)*iterProdItem);// (*itemSet->getProductionItems())[j];
            
            printf("I%d (%d)", i, estado->getID());
            
            printf("%s", prodItem->getIsKernel()?"(k)":"");
            
            printf("[%d]", prodItem->getOriginalState());
            
            printProduction(prodItem->getProduction(), prodItem->getDotPosition());
            
            printf("/ %s", (prodItem->getLookAhead()->getID().c_str()));
            
            printf(" -- %s", prodItem->getIsGotoProcessed()?"YES":"NO");
            
            printf("\r\n");
            
            lines++;
        }
        
        
        printf("\n");
        printf("Productions -> %d\n", lines);
        printf("\n");
        
        printf("<acciones>\n");
        
        total = estado->getListaAcciones().size();
        
        //map<terminal*, Accion*>::iterator iterAcciones = estado->getListaAcciones().begin();
        
        for (long j = 0; j < total; j++) {
            
             
            //terminal* ter = (*iterAcciones).first;
            Accion* accion = estado->getListaAcciones()[j];
            
            
            printf("%s %s %d\n", accion->getTerminal()->getID().c_str(), (accion->getTipo() == 0?"D":(accion->getTipo() == 1?"R":"A")), accion->getEstadoID());
           
        }
        
        
        printf("</acciones>\n\n");

        
        printf("<ir_a>\n");
        
        total = estado->getListaIra().size();
        
        //map<terminal*, Accion*>::iterator iterAcciones = estado->getListaAcciones().begin();
        
        for (long j = 0; j < total; j++) {
            
            
            //terminal* ter = (*iterAcciones).first;
            Ira* ira = estado->getListaIra()[j];
            
            
            printf("%s %d\n", ira->getNonTerminal()->getID().c_str(), ira->getEstadoID());
            
        }
        
        
        printf("</ir_a>\n\n");

        
        printf("<transiciones>\n");
        
        total = estado->getTransitions().size();
        
        //map<terminal*, Accion*>::iterator iterAcciones = estado->getListaAcciones().begin();
        
        for (long j = 0; j < total; j++) {
            
            
            //terminal* ter = (*iterAcciones).first;
            Transition* transition = estado->getTransitions()[j];
            
            
            printf("with %s to %d\n", transition->getConsume()->getID().c_str(), transition->getState());
            
        }
        
        
        printf("</transiciones>\n\n");
        
        
        i++;
        
    }
}

int codegenerator::getReducirLocation(production *prod)
{
    
    long totalProducciones = gramatica.size();
    for (long i = 0; i < totalProducciones; i++) {
        
        production* fromLista = gramatica[i];
        
        if (prod->getVariable()->getID() == fromLista->getVariable()->getID())
        {
            
            if ( fromLista->getSymbols().size() == prod->getSymbols().size() )
            {
                vector<symbol*>::iterator listaTerminales;
                
                //int j = 0;
                bool equal = true;
                long totalj = fromLista->getSymbols().size();
                
                for (int j = 0; j < totalj; j++) {
                    //for (listaTerminales = fromLista->getSymbols().begin(); listaTerminales != fromLista->getSymbols().end(); ++listaTerminales) {
                    
                    symbol* simboloA = fromLista->getSymbols()[j];//  *listaTerminales;
                    
                    if ( prod->getSymbols()[j] != simboloA )
                    {
                        equal = false;
                        break;
                    }
                    
                    //j++;
                }
                
                if (equal)
                    return (int)(i);
            }
        }
    }
    
    
    return -1;
}



void codegenerator::insertarProducciones(FILE *file)
{
    fputs("\t\t//====inicio producciones\r\n", file);
    
    string data;
    
    
    
    
    fputs("\t\t//====inicio lista simbolos\n", file);
    
    
    int numericID = 1;
    
    //primero agregamos los simbolos a una lista
       
    //primero agregamos los simbolos a una lista
    vector<terminal*>::iterator iterTerminal;
    
    //terminales
    
    for (iterTerminal = terminalList.begin(); iterTerminal != terminalList.end(); ++iterTerminal) {
        terminal* nt = *iterTerminal;
        
        nt->setNumericID(numericID);
        
        data = "\t\tsymbols.put(String.valueOf(" + to_string(nt->getNumericID()) + "), new Terminal(\"" + nt->getID() + "\", " + to_string(numericID) + "));\n";
        fputs(data.c_str(), file);
        
        numericID++;
    }
    
    //no terminales
    vector<nonterminal*>::iterator iterNonTerminal;
    
    for (iterNonTerminal = nonterminalList.begin(); iterNonTerminal != nonterminalList.end(); ++iterNonTerminal) {
        nonterminal* nt = *iterNonTerminal;
        
        nt->setNumericID(numericID);
        
        data = "\t\tsymbols.put(" + nt->javaString() + ", new Nonterminal(\"" + nt->getID() + "\", " + to_string(nt->getNumericID()) + "));\n";
        fputs(data.c_str(), file);
        
        numericID++;
    }    

    
    
    
    
    //fputs("\t\tNonterminal nonterminal = null;\n", file);
    //fputs("\t\tTerminal terminal = null;\n", file);
    
    fputs("\t\tProduction prod = null;\n", file);
    
    
    long totalProducciones = gramatica.size();
    
    for (int i = 0; i < totalProducciones; i++) {
        
        production* prod = gramatica[i];
        
        
        
        
        
        fputs("\t\t//======Produccion...\n", file);
        
        data = "\t\tprod = new Production((Nonterminal)symbols.get(" + prod->getVariable()->javaString() + "));\n";
        fputs(data.c_str(), file);
        
        data = "\t\tprod.setCodigoId(" + to_string(prod->getCodigoId()) + ");\n";
        fputs(data.c_str(), file);
        
        
        vector<symbol*>::iterator listaTerminales;
        
        long totalTerminales = prod->getSymbols().size();
        
        fputs("\t\t//========simbolos en la produccion\n", file);
        
        
        for (int j = 0; j < totalTerminales; j++) {
            
            //for (listaTerminales = prod->getSymbols().begin(); listaTerminales != prod->getSymbols().end(); ++listaTerminales) {
            symbol* simbolo = prod->getSymbols()[j]; //*listaTerminales;
            
            if (typeid(*simbolo) == typeid(terminal))
            {
                data = "\t\tprod.getSimbolos().add(symbols.get(" + simbolo->javaString() + "));\n";
                fputs(data.c_str(), file);
            }
            else
            {
                data = "\t\tprod.getSimbolos().add(symbols.get(" + simbolo->javaString() + "));\n";
                fputs(data.c_str(), file);
            }
        }

        fputs("\t\t//fin simbolos produccion\n", file);
        
        data = "\t\tgramatica.getProductions().add(prod);\n";
        fputs(data.c_str(), file);
        fputs("\t\t//fin produccion\n", file);
        
    }
    
    
    fputs("\n\n", file);

    
}

void codegenerator::insertarEstados(FILE *file)
{

    fputs("\t\t//====inicio estados\n\n", file);
    
    string data;
    string tonto = "";
    
    fputs("\t\tState state = null;\n", file);
    fputs("\t\tTransition transition = null;\n", file);
    fputs("\t\tAccion accion = null;\n", file);
    fputs("\t\tIra ira = null;\n", file);
    
    
    //long totalStates = listaEstados.size();
    
    map<int, State*>::iterator iter;
    
    for (iter = listaEstados.begin(); iter != listaEstados.end(); ++iter) {
    
    
        //for (int i = 0; i < totalStates; i++) {
    
        
        State* estado = iter->second; //listaEstados[i];
        //        
        data = "\t\tstate = new State(" + tonto + (estado->isAcceptState()?"true":"false") + tonto + ", " + to_string(estado->getID()) + ");\n";
        fputs(data.c_str(), file);
        
        //agregar transiciones
        long totalTransiciones = estado->getTransitions().size();
        
        //map<terminal*, Accion*>::iterator iterAcciones = estado->getListaAcciones().begin();
        
        for (long j = 0; j < totalTransiciones; j++) {
            
            
            //terminal* ter = (*iterAcciones).first;
            Transition* transition = estado->getTransitions()[j];
            
            data = "\t\ttransition = new Transition(symbols.get(" + transition->getConsume()->javaString() + "), " + to_string(transition->getState()) + ");\n";
            fputs(data.c_str(), file);
            
            fputs("\t\tstate.getTransitions().add(transition);\n", file);
        }
        
        
        
        //agregar acciones
        fputs("\n\n", file);
        
        fputs("\t\t//======Accion\n\n", file);
       
        long totalAcciones = estado->getListaAcciones().size();
        
        //map<terminal*, Accion*>::iterator iterAcciones = estado->getListaAcciones().begin();
        
        for (long j = 0; j < totalAcciones; j++) {
            
            
            //terminal* ter = (*iterAcciones).first;
            Accion* accion = estado->getListaAcciones()[j];
            
            data = "\t\taccion = new Accion(" + tonto + (accion->getTipo() == SHIFT?"Accion.tipo_accion.SHIFT":(accion->getTipo() == REDUCE?"Accion.tipo_accion.REDUCE":"Accion.tipo_accion.ACCEPT")) + tonto + ", " + to_string(accion->getEstadoID()) + ");\n";
            fputs(data.c_str(), file);
            
            data = "\t\tstate.getListaAcciones().put((Terminal)symbols.get(" + accion->getTerminal()->javaString() + "), accion);\n";
            fputs(data.c_str(), file);
            
        }

        

        //agregar ir_a
        
        fputs("\n\n", file);
        fputs("\t\t//======Ir a\n\n", file);
        
        long totalIra = estado->getListaIra().size();
        
        //map<terminal*, Accion*>::iterator iterAcciones = estado->getListaAcciones().begin();
        
        for (long j = 0; j < totalIra; j++) {
            
            
            //terminal* ter = (*iterAcciones).first;
            Ira* ira = estado->getListaIra()[j];
            
            data = "\t\tira = new Ira(" + to_string(ira->getEstadoID()) + ");\n";
            fputs(data.c_str(), file);
            
            data = "\t\tstate.getListaIra().put((Nonterminal)symbols.get(" + ira->getNonTerminal()->javaString() + "), ira);\n";
            fputs(data.c_str(), file); 
        }
        
        fputs("\n\n", file);
        fputs("\t\t//fin ira a\n\n", file);
        
        
        data = "\t\tstates.put(\"" + to_string(estado->getID()) + "\", state);\n";
        fputs(data.c_str(), file);
        
        fputs("\n\n", file);
        
        
        
    }
}

void codegenerator::insertarExecuteCodeMethod(FILE *file)
{
    
    string p = "\n\n\t@SuppressWarnings({ \"unchecked\", \"unused\" })//To supress some object to array casting, and unused variables\n";
    fputs(p.c_str(), file);
    
    p = "\n\n\t@Override\n";
    fputs(p.c_str(), file);
    
    fputs("\tpublic Object executeCode(ArrayList<Object> parametros, int codeId) throws Exception\n", file);
    fputs("\t{\n\n", file);
    
    fputs("\t\tswitch(codeId)\n", file);
    fputs("\t\t{\n", file);
    
    
    long totalProducciones = gramatica.size();
    for (int i = 0; i < totalProducciones; i++) {
        
        production* fromLista = gramatica[i];
        
        if (fromLista->getCodigoId() >= 0)
        {
            p = "\t\t\tcase " + to_string(fromLista->getCodigoId()) + ": //" + fromLista->toString() + "\n";
            fputs(p.c_str(), file);
            
            fputs("\t\t\t{\n", file);
            
            
            //insertar codigo de parametros y el return.
            vector<Parametro*>::iterator itParam;
            vector<Parametro*> listaParam = fromLista->getParametros();
            
            
            long totalSimbolos = fromLista->getSymbols().size();
            //long totalParam = fromLista->getParametros().size();
            int parami = 0;
            
            for (itParam = listaParam.begin(); itParam != listaParam.end(); ++itParam) {
            
                Parametro* parametro = *itParam;
                
                string paramID = parametro->getId();
                string symbolType = parametro->getReturnType();
                
                
                p = "\t\t\t\t" + symbolType + " " + paramID + " = (" + symbolType + ")parametros.get(" + to_string((int)totalSimbolos - parametro->getPosition()-1) +");\n";
                fputs(p.c_str(), file);
                
                parami++;
                
            }
            
            //poner result variable;
            p = "\t\t\t\t" + fromLista->getVariable()->getReturnObjectID() + " $ = null" + ";\n\n\n";
            fputs(p.c_str(), file);
            
            
            p = "\t\t\t" + fromLista->getCodigo() + "\n\n\n";
            fputs(p.c_str(), file);
            
            p = "\t\t\t\t return $;\n";
            fputs(p.c_str(), file);
            
            
            fputs("\t\t\t}\n", file);
        }
    } 
    
    
    fputs("\t\t}\n\n", file);
    
    fputs("\t\treturn null;\n", file);
    
    
    fputs("\t}\n\n", file);
    
}

void codegenerator::generateJavaSymFile()
{
    
    string symFileName = directoryPath + "sym.java";
    
    printf("Creando archivo %s\n", symFileName.c_str());
    
    FILE* file = NULL;
	file = fopen(symFileName.c_str(), "w");
    
    fputs("\r\n\r\n", file);

    string data;
    
    fputs("/* Autogenerated class */\n\n\n\n", file);
    
    fputs("public class sym {\n", file);
    fputs("\t/* terminals */\n", file);
       
    //primero agregamos los simbolos a una lista
    vector<terminal*>::iterator iterTerminal;
    
    for (iterTerminal = terminalList.begin(); iterTerminal != terminalList.end(); ++iterTerminal) {
        terminal* nt = *iterTerminal;
        
        data = "\tpublic static final int " + nt->getID() + " = " + to_string(nt->getNumericID()) +";\n";
        fputs(data.c_str(), file);
    }

    
    
    fputs("}\n", file);
    
    fclose(file);
}

void codegenerator::generateJavaFile()
{
    fileName = directoryPath + this->className + ".java";
    
    printf("Creando archivo %s\n", fileName.c_str());
    
    FILE* file = NULL;
	file = fopen(fileName.c_str(), "w");
    
    fputs("\r\n\r\n", file);
    //fputs("import Parser;\r\n\r\n", file);
    
    fputs("import java.util.ArrayList;\n", file);
    
    fputs(importString.c_str(), file);
    
    fputs("\n\n", file);
    
    
    
    
    string p = "public class " + this->className + " extends Parser\r\n";
    fputs(p.c_str(), file);
    fputs("{\r\n\r\n", file);
    
    p = "\tpublic " + this->className + "(Yylex scanner) {\r\n";
    fputs(p.c_str(), file);
    
    
    
    
    fputs("\t\r\n", file);
    
   
    fputs("\t\tsuper(scanner);\n", file);
    
    
    fputs("\t\r\n", file);
    
    
    insertarProducciones(file);
    
    insertarEstados(file);
    
    //final del constructor
    fputs("\t}\r\n", file);
    
    
    insertarExecuteCodeMethod(file);
    
    
    p = "\t\t" + parseString;
    fputs(p.c_str(), file);
    
    fputs("\n\n", file);
    
    //final de la clase
    fputs("}\r\n", file);
    
    
    
    fclose(file);
    
}



bool codegenerator::checkIfItemSetExists(ItemSet *itemSet, int* estadoID)
{
    long totalSets = itemSetList.size();
    
    for (int i = 0; i < totalSets; i++) {
        
        ItemSet* itemFromList = itemSetList[i];
        
        if (itemFromList->getProductionItems()->size() == itemSet->getProductionItems()->size())
        {
            
            long totalItems = itemFromList->getProductionItems()->size();
            
            bool isEqual = true;
            //for (iterProdItem = currentSet->getProductionItems().begin(); iterProdItem != currentSet->getProductionItems().end(); ++iterProdItem) {
            for (long j = 0; j < totalItems; j++) {
                
                ProductionItem* prodItemList = (*itemFromList->getProductionItems())[j];
                ProductionItem* prodItemSet =  (*itemSet->getProductionItems())[j];
                
                if ( !productionItemEqual(prodItemList, prodItemSet, false, false) )
                {
                    isEqual = false;
                    break;
                }
            }
            
            if (isEqual)
            {
                *estadoID = i;
                return true;
            }
        }
    }
    
    return false;
}


vector<ProductionItem*> codegenerator::uniqueKernelProductionList(ItemSet* set)
{
    vector<ProductionItem*> list;
    
    vector<ProductionItem*>::iterator it;
    
    for (it = set->getProductionItems()->begin(); it != set->getProductionItems()->end(); it++) {
        ProductionItem* prod = (*it);
        
        
        if (prod->getIsKernel())
        {
            
            vector<ProductionItem*>::iterator it2;
            
            
            bool dontAdd = false;
            for (it2 = list.begin(); it2 != list.end(); ++it2) {
                ProductionItem* prod2 = *it2;
                
                if (prod2->getIsKernel())
                {
                    if (productionItemEqual(prod, prod2, false, true))
                    {
                        dontAdd = true;
                    }
                }
            }
            
            if (!dontAdd)
                list.push_back(prod);
        }
        
    }
    
    return list;
    
}

bool codegenerator::itemSetEqual(ItemSet *itemSetA, ItemSet *itemSetB)
{
    ItemSet *menor;
    ItemSet *mayor;
    
    
    
    if (itemSetA->getProductionItems()->size() >= itemSetB->getProductionItems()->size())
    {
        mayor = itemSetA;
        menor = itemSetB;
    }
    else
    {
        mayor = itemSetB;
        menor = itemSetA;
    }
    
    
    
    
    
    long totalItems = menor->getProductionItems()->size();
    
    for (long j = 0; j < totalItems; j++) {
        
        ProductionItem* prodMenor = (*menor->getProductionItems())[j];
        
        if (prodMenor->getIsKernel())
        {
            
            long totalItemsMayor = mayor->getProductionItems()->size();
            
            bool isEqual = false;
            for (int i = 0; i < totalItemsMayor; i++) {
                
                ProductionItem* prodMayor = (*mayor->getProductionItems())[i];
                
                //if (prodMayor->getIsKernel())
                {
                    if ( productionItemEqualForMerge(prodMayor, prodMenor, false, true) )
                    {
                        isEqual = true;
                        break;
                    }
                }
            }
            
            if (!isEqual)
            {
                return false;
            }
        }
    }
    
    if (itemSetA->getProductionItems()->size() == 0 || itemSetB->getProductionItems()->size() == 0 )
        return false;
    
    return true;
}


bool codegenerator::productionKernelListEqual(vector<ProductionItem*> listA, vector<ProductionItem*> listB)
{
    
    if (listA.size() != listB.size())
        return false;
 
        
    
        
        
    long totalItems = listA.size();
        
    for (long j = 0; j < totalItems; j++) {
        
        ProductionItem* prodMenor = listA[j];
        
        long totalItemsMayor = listB.size();
        
        bool isEqual = false;
        for (int i = 0; i < totalItemsMayor; i++) {

            ProductionItem* prodMayor = listB[i];
                
            if ( productionItemEqualForMerge(prodMayor, prodMenor, false, true) )
            {
                isEqual = true;
                break;
            }
        }
            
        if (!isEqual)
        {
            return false;
        }
        
    }

    return true;
}

void codegenerator::printProduction(production* prod, int dotPosition)
{
    printf("%s -> ", (prod)->getVariable()->getID().c_str());
    
    vector<symbol*>::iterator iterSymbol;
    
    string codigo = (prod)->getCodigo();
    
    //int symbolPos = 0;
    long totalSym = (prod)->getSymbols().size();
    int symbolPos;
    for (symbolPos = 0; symbolPos < totalSym; symbolPos++) {
        //for (iterSymbol = (prod)->getSymbols().begin(); iterSymbol != (prod)->getSymbols().end(); ++iterSymbol) 
        //{
        
        symbol* sym = (prod)->getSymbols()[symbolPos];
        
        //printf("%s ", codigo[symbolPos].c_str());
        
        if (symbolPos == dotPosition)
            printf("%s ", "(*)");

        printf("%s ", sym->getID().c_str());
        
        
        //symbolPos++;
    }
    
    if (symbolPos == dotPosition)
        printf("%s ", "(*)");
    
    //if (symbolPos < codigo.size())
    //    printf("%s ", codigo[symbolPos].c_str());
}



vector<terminal*> codegenerator::primero(symbol* sym)
{
    vector<terminal*> lista;
    vector<production*>::iterator iterProd;
    vector<terminal*> lista2;
    
    if (typeid(*sym) == typeid(terminal))
        lista.push_back((terminal*)sym);
    else
    {
        long sizeGram = gramatica.size();
        for (int it = 0; it < sizeGram; it++) {
            //for (iterProd = gramatica.begin(); iterProd != gramatica.end(); ++iterProd) {
            production* prd = gramatica[it];
            
            if ( prd->getVariable()->getID() == sym->getID()  )
            {
                if (prd->getSymbols().size() == 1 && prd->getSymbols()[0]->getID() == EPSILON )
                {
                    lista2.push_back(getTerminal(EPSILON));
                }
                else
                {
                    
                    
                    //production* prod = *iterProd;
                    //es una produccion
                    if (prd->getSymbols().size() > 0) {  
                        
                        
                        if ( prd->getSymbols()[0]->getID() == sym->getID() )
                        {
                            //do nothing
                        }
                        else
                        {
                            //printf("%s \n", prd->getSymbols()[0]->getID().c_str());
                            
                            long totalSimbolos = prd->getSymbols().size();
                            
                            bool dontAddEpsilon = false;
                            for (int simi = 0; simi < totalSimbolos; simi++) {
                                
                                vector<terminal*> temp = primero(prd->getSymbols()[simi]);
                                
                                vector<terminal*>::iterator iteratorTerminal;
                                
                                for (iteratorTerminal = temp.begin(); iteratorTerminal != temp.end(); ++iteratorTerminal) {
                                    terminal* term2 = *iteratorTerminal;
                                    
                                    if (term2->getID() != EPSILON)
                                        lista2.push_back(term2);
                                }
                                
                                //lista2.insert(lista2.end(), temp.begin(), temp.end());
                                
                                
                                if (!epsilonFoundIn(temp))
                                {   
                                    dontAddEpsilon = true;
                                    break;
                                }
                            }
                            
                            if (!dontAddEpsilon)
                            {
                                lista2.push_back(getTerminal(EPSILON));
                            }
                        }
                    }
                }
            }
        }
        
        vector<terminal*>::iterator iteratorTerminal;
        vector<terminal*>::iterator iteratorTerminal2;
        
        
        for (iteratorTerminal = lista2.begin(); iteratorTerminal != lista2.end(); ++iteratorTerminal) {
            terminal* term = *iteratorTerminal;
            
            bool found = false;
            for (iteratorTerminal2 = lista.begin(); iteratorTerminal2 != lista.end(); ++iteratorTerminal2) {
                terminal* term2 = *iteratorTerminal2;
                
                if (term2->getID() == term->getID())
                    found = true;
            }
            
            if (!found)
                lista.push_back(term);
        }        
    }
    
    
    return lista;
        
}

bool codegenerator::epsilonFoundIn(vector<terminal *> lista)
{
    long total = lista.size();
    
    for (int i = 0; i < total; i++) {
        terminal* t = lista[i];
        
        if (t->getID() == EPSILON)
            return true;
    }
    
    return false;
}


ItemSet* codegenerator::ir_a(ItemSet* itemSet, symbol* sym)
{
    ItemSet* itemJ = new ItemSet();
    
    vector<ProductionItem*>::iterator iterProdItem;
    
    long total = itemSet->getProductionItems()->size();

    for (long i = 0; i < total; i++) {
    //for (iterProdItem = itemSet->getProductionItems().begin(); iterProdItem != itemSet->getProductionItems().end(); ++iterProdItem) {
        
        ProductionItem* prodItem = (*itemSet->getProductionItems())[i];// ((ProductionItem*)*iterProdItem);
        
        symbol* nextSymbol = prodItem->nextSymbol();
        
        if (nextSymbol != NULL )
        {
            if ( nextSymbol->getID() == sym->getID() )
            {
                ProductionItem* newItem = new ProductionItem(prodItem->getProduction(), prodItem->getLookAhead(), false);
                
                
                newItem->setIsKernel(true);
                newItem->setPassId(0);
                newItem->setGotoProcessed(false);
                
                newItem->setDotPosition(prodItem->getDotPosition());
                
                //movemos una posicion.
                newItem->moveDot();
                
                itemJ->addProduction(newItem);
                
                prodItem->setGotoProcessed(true);
                
            }
        }
    }
    
    cerradura(itemJ);
    
    return itemJ;
    
}

void codegenerator::cerradura(ItemSet* itemSet)
{
    
    bool checkAgain;
    
    do {

        
        checkAgain = false;
        vector<production*>::iterator iterProd;
        vector<ProductionItem*> nuevasProducciones;
        
        
        vector<ProductionItem*>::iterator iterProdItem;
        
        long totalP = itemSet->getProductionItems()->size();
        
        for (int itemi = 0; itemi < totalP; itemi++) {
            //for (iterProdItem = itemSet->getProductionItems().begin(); iterProdItem != itemSet->getProductionItems().end(); ++iterProdItem) {
            ProductionItem* prodItem = (*itemSet->getProductionItems())[itemi]; //((ProductionItem*)*iterProdItem);
            
            
            
            if (!prodItem->getIsProcessed())
            {
                prodItem->setIsProcessed(true);
                symbol* sym = prodItem->nextSymbol();
                //vector<production*> nuevasProducciones;
                
                if (sym != NULL)
                {
                    
                    vector<symbol*> symbolsAfterBeta = prodItem->getBeta();
                    symbolsAfterBeta.push_back(prodItem->getLookAhead());
                    
                    vector<production*>::iterator iterProd;
                    
                    for (iterProd = gramatica.begin(); iterProd != gramatica.end(); ++iterProd) {
                        if ( (*iterProd)->getVariable()->getID() == sym->getID() )
                        {
                            
                            vector<symbol*>::iterator iterSymbolProduction;
                            
                            
                            if (symbolsAfterBeta.size() > 0)
                            {
                                
                                
                                vector<terminal*>::iterator terminalIterator;
                                
                                vector<terminal*> listaPrimeroFinal;
                                
                                bool allHadEpsilon = true;
                                for (iterSymbolProduction = symbolsAfterBeta.begin(); iterSymbolProduction != symbolsAfterBeta.end(); ++iterSymbolProduction) {
                                    symbol* sym = *iterSymbolProduction;
                                    vector<terminal*> listaPrimero = primero(sym);
                                    
                                    for (terminalIterator = listaPrimero.begin(); terminalIterator != listaPrimero.end(); ++terminalIterator) {
                                        
                                        terminal* lookAheadSymbol = *terminalIterator;
                                        
                                        if (lookAheadSymbol->getID() != EPSILON)
                                        {
                                            listaPrimeroFinal.push_back(lookAheadSymbol);
                                        }
                                    }
                                    
                                    if (!epsilonFoundIn(listaPrimero)) {
                                        allHadEpsilon = false;
                                        break;
                                    }
                                }    
                                
                                
                                if ( allHadEpsilon )
                                    listaPrimeroFinal.push_back(getTerminal(EPSILON));
                                
                                
                                
                                for (terminalIterator = listaPrimeroFinal.begin(); terminalIterator != listaPrimeroFinal.end(); ++terminalIterator) {
                                    
                                    terminal* lookAheadSymbol = *terminalIterator;
                                    
                                    //buscamos si ya existe en la lista de itemSet, si no la agregamos.
                                    ProductionItem* nuevoItem = new ProductionItem(*iterProd, lookAheadSymbol, false);
                                    nuevoItem->setPassId(0);
                                    
                                    nuevasProducciones.push_back(nuevoItem);
                                    
                                }
                            }
                            else
                            {
                                vector<terminal*> listaPrimero = primero(prodItem->getLookAhead());
                                
                                vector<terminal*>::iterator terminalIterator;
                                
                                for (terminalIterator = listaPrimero.begin(); terminalIterator != listaPrimero.end(); ++terminalIterator) {
                                    
                                    terminal* lookAheadSymbol = *terminalIterator;
                                    //buscamos si ya existe en la lista de itemSet, si no la agregamos.
                                    ProductionItem* nuevoItem = new ProductionItem(*iterProd, lookAheadSymbol, false);
                                    nuevoItem->setPassId(0);
                                    
                                    nuevasProducciones.push_back(nuevoItem);
                                }
                                
                            }
                        }
                    }
                }
            }
        }
        
        if ( nuevasProducciones.size() > 0 )
        {
            vector<ProductionItem*>::iterator iterProdItem;
            vector<ProductionItem*>::iterator iterProdList;
            
            for (iterProdItem = nuevasProducciones.begin(); iterProdItem != nuevasProducciones.end(); ++iterProdItem) {
             
                ProductionItem* itemFromList = *iterProdItem;
                
                bool found = false;
                
                long sizeProd = itemSet->getProductionItems()->size();
                
                for (long x = 0; x < sizeProd; x++) {
                    
                    //for(iterProdList = itemSet->getProductionItems().begin(); iterProdList != itemSet->getProductionItems().end(); ++iterProdList) {
                    
                    ProductionItem* itemFromSet = (*itemSet->getProductionItems())[x];//*iterProdList;
                    
                    //check if the 2 productions are alike.
                    
                    if ( productionItemEqual(itemFromSet, itemFromList, false, false) )
                    {
                        found = true;
                        break;
                    }
                }
                
                if (!found)
                {
                    itemSet->addProduction(itemFromList);
                    checkAgain = true;
                }
                else
                    delete itemFromList;
            }
            
            nuevasProducciones.clear();
        }
        
    } while (checkAgain);
    
}

bool codegenerator::productionItemEqualForMerge(ProductionItem *prodItemA, ProductionItem *prodItemB, bool ignoreDotPosition, bool ignoreLookAhead)
{
    if ( prodItemA->getProduction()->getSymbols().size() == prodItemB->getProduction()->getSymbols().size() )
    {
        
        if (prodItemA->getProduction()->getVariable()->getID() != prodItemB->getProduction()->getVariable()->getID())
            return false;
        
        if (!ignoreDotPosition)
            if ( prodItemA->getDotPosition() != prodItemB->getDotPosition() )
                return false;
        
        
        if (!ignoreLookAhead)
        {
            if ( prodItemA->getLookAhead()->getID() != prodItemB->getLookAhead()->getID() )
                return false;
        }
        
        vector<symbol*>::iterator listaTerminales;
        
        //int i = 0;
        long totalLista = prodItemA->getProduction()->getSymbols().size();
        for (int i = 0; i < totalLista; i++) {
            //for (listaTerminales = prodItemA->getProduction()->getSymbols().begin(); listaTerminales != prodItemA->getProduction()->getSymbols().end(); ++listaTerminales) {
            symbol* simboloA = prodItemA->getProduction()->getSymbols()[i];// *listaTerminales;
            
            if ( prodItemB->getProduction()->getSymbols()[i]->getID() != simboloA->getID() )
            {
                return false;
            }
            //i++;
        }
        
        
        
        return true;
        
    }
    else
        return false;
}

bool codegenerator::productionItemEqual(ProductionItem *prodItemA, ProductionItem *prodItemB, bool ignoreDotPosition, bool ignoreLookAhead)
{
    if ( prodItemA->getProduction()->getSymbols().size() == prodItemB->getProduction()->getSymbols().size() )
    {
        
        if (prodItemA->getProduction()->getVariable()->getID() != prodItemB->getProduction()->getVariable()->getID())
            return false;
        
        if (!ignoreDotPosition)
            if ( prodItemA->getDotPosition() != prodItemB->getDotPosition() )
                return false;
        
        
        if (!ignoreLookAhead)
        {
            if ( prodItemA->getLookAhead()->getID() != prodItemB->getLookAhead()->getID() )
                return false;
        }
        
        vector<symbol*>::iterator listaTerminales;
        
        //int i = 0;
        long totalLista = prodItemA->getProduction()->getSymbols().size();
        for (int i = 0; i < totalLista; i++) {
            //for (listaTerminales = prodItemA->getProduction()->getSymbols().begin(); listaTerminales != prodItemA->getProduction()->getSymbols().end(); ++listaTerminales) {
            symbol* simboloA = prodItemA->getProduction()->getSymbols()[i];// *listaTerminales;
            
            if ( prodItemB->getProduction()->getSymbols()[i]->getID() != simboloA->getID() )
            {
                return false;
            }
            //i++;
        }
        
       
        
        return true;
        
    }
    else
        return false;
}



void codegenerator::aumentar()
{
    if (gramatica.size() > 0) {
        nonterminal* t = new nonterminal(START, gramatica[0]->getVariable()->getReturnObjectID());
        t->setNumericID(0);
        production* inicio = new production(t);
        //inicio->addCode("");
        //inicio->addCode("");
        
        inicio->addSymbol(gramatica[0]->getVariable());
        inicio->addSymbol(getTerminal(EOFT));
        
        gramatica.insert(gramatica.begin(), inicio);
    }
}

void codegenerator::tagOpen()
{
   if ( typeid(*lex->currentToken()) == typeid(TokenTagOpen) )
   {
       lex->nextToken();
       
       if ( typeid(*lex->currentToken()) == typeid(TokenImport) )
           importTag();
       else if ( typeid(*lex->currentToken()) == typeid(TokenParserCode) )
           parseCodeTag();
   }
    else
        throw new compilerexception("Tag '<%' expected!");
}

void codegenerator::importTag()
{
    if ( typeid(*lex->currentToken()) == typeid(TokenImport) )
    {
        if (!importCode)
        {
            lex->setPascalMode(true);
            importCode = true;
            Token* token;
            importString = "";
            while((token = lex->nextToken()))
            {
                //printf("%s -> %s \r\n", token->lexema().c_str(), token->className());
                
                
                
                if ( typeid(*token) == typeid(TokenTagClose) )
                {
                    break;
                }
                else if ( typeid(*token) == typeid(TokenEof) )
                    throw new compilerexception("Unexpected Eof token. Expected '%>' instead.");
                else
                    importString += token->lexema();
                
            }
        }
        else
            throw new compilerexception("Tag 'import' can only be declared once!");
    }
    else
        throw new compilerexception("Tag 'import' expected!");
}

void codegenerator::parseCodeTag()
{
    if ( typeid(*lex->currentToken()) == typeid(TokenParserCode) )
    {
        if (!parseCode)
        {
            lex->setPascalMode(true);
            parseCode = true;
            Token* token;
            parseString = "";
            while((token = lex->nextToken()))
            {
                printf("%s \r\n", token->lexema().c_str());
                
                
                
                if ( typeid(*token) == typeid(TokenTagClose) )
                {
                    break;
                }
                else if ( typeid(*token) == typeid(TokenEof) )
                    throw new compilerexception("Unexpected Eof token. Expected '%>' instead.");
                else
                    parseString += token->lexema(); 
                
            }
        }
        else
            throw new compilerexception("Tag 'parsecode' can only be declared once!");
    }
    else
        throw new compilerexception("Tag 'parsecode' expected!");
}


void codegenerator::tagDeclaration()
{
    if ( typeid(*lex->currentToken()) == typeid(TokenDeclaration) )
    {
        lex->nextToken();
        
        if ( typeid(*lex->currentToken()) == typeid(TokenClass) )
        {
            if (!classDeclaration)
            {   
                classDeclaration = true;
                
                lex->nextToken();
                
                className = lex->currentToken()->lexema();
            }
            else
                throw new compilerexception("Tag 'class' can only be declared once!");    
        }
        else
            throw new compilerexception("Only 'class' expected after '%'. Unexpected token!");
    }
    else
        throw new compilerexception("Tag '%' expected!");
}

void codegenerator::tagTerminal()
{
    if ( typeid(*lex->currentToken()) == typeid(TokenTerminal) )
    {
        bool onlySymbols = false;
        string returnId = "String";
        Token* token;
        string id;
        bool dontConsume = false;
        while((token = lex->nextToken()))
        {
            //printf("%s -> %s \r\n", token->lexema().c_str(), token->className());
            if ( typeid(*token) == typeid(TokenID) )
            {
                
                if (!onlySymbols)
                {
                    returnId = token->lexema();
                    
                    token = lex->nextToken();
                    
                    if ( typeid(*token) != typeid(TokenID) && typeid(*token) != typeid(TokenComma) && typeid(*token) != typeid(TokenSemicolon)  )
                    {
                        throw new compilerexception("Symbol expected!"); 
                    }
                    else if ((typeid(*token) == typeid(TokenComma)) || (typeid(*token) == typeid(TokenSemicolon)))
                    {
                        //en este caso no se definio tipo
                        //el que pensamos que era un tipo, es realmente un simbolo
                        id = returnId;
                        dontConsume = true;
            
                        returnId = "String";
                        lex->rewindToLastToken();
                    }
                    
                    onlySymbols = true;
                }
                
                if (!dontConsume)
                    id = lex->currentToken()->lexema();
                else
                    dontConsume = false;
                
                
                
                if (isSymbolDeclared(id))
                {
                    throw new compilerexception("Symbol ID (" + id + ") already declared!");
                }
                
                //simbolo
                terminal* terminalSymbol = new terminal(id, returnId);
                terminalList.push_back(terminalSymbol);

                
                token = lex->nextToken();
                    
                if ( typeid(*token) != typeid(TokenComma) && ( typeid(*token) != typeid(TokenSemicolon) ) )
                    throw new compilerexception("',' expected!");
            }
            
            if ( typeid(*token) == typeid(TokenSemicolon) )
                break;
            else if ( typeid(*token) == typeid(TokenEof) )
                throw new compilerexception("Unexpected Eof token. Expected '%>' instead.");
            
        }
    }
    else
        throw new compilerexception("Tag 'terminal' expected!");

}

void codegenerator::tagNonTerminal()
{
    if ( typeid(*lex->currentToken()) == typeid(TokenNonTerminal) )
    {
        bool onlySymbols = false;
        string returnId = "";
        Token* token;
        while((token = lex->nextToken()))
        {
            //printf("%s -> %s \r\n", token->lexema().c_str(), token->className());
            
            if ( typeid(*token) == typeid(TokenID) )
            {
                if (!onlySymbols)
                {
                    returnId = token->lexema();
                    
                    token = lex->nextToken();
                    
                    if ( typeid(*token) != typeid(TokenID) )
                    {
                        throw new compilerexception("Symbol expected!"); 
                    }
                    
                    onlySymbols = true;
                }
                
                string id = lex->currentToken()->lexema();
                
                if (isSymbolDeclared(id))
                {
                    throw new compilerexception("Symbol ID (" + id + ") already declared!");
                }
                
                nonterminal* nonterminalSymbol = new nonterminal(id, returnId);
                nonterminalList.push_back(nonterminalSymbol);
                
                token = lex->nextToken();
                
                if ( typeid(*token) != typeid(TokenComma) && ( typeid(*token) != typeid(TokenSemicolon) ) )
                    throw new compilerexception("',' expected!");
            }
            
            if ( typeid(*token) == typeid(TokenSemicolon) )
                break;
            else if ( typeid(*token) == typeid(TokenEof) )
                throw new compilerexception("Unexpected Eof token. Expected '%>' instead.");
            
        }
    }
    else
        throw new compilerexception("Tag 'nonterminal' expected!");
}



bool codegenerator::isTerminal(string id)
{
    bool found = false;
    
    vector<terminal*>::iterator iterTerminal;
    
    for (iterTerminal = terminalList.begin(); iterTerminal != terminalList.end(); ++iterTerminal) {
        if ( (*iterTerminal)->getID() == id )
        {
            found = true;
            break;
        }
    }
    
    return found;
}

bool codegenerator::isNonTerminalInProduction(string id)
{
    bool found = false;
    
    vector<production*>::iterator prodIterator;
    
    for (prodIterator = gramatica.begin(); prodIterator != gramatica.end(); ++prodIterator) {
        if ( (*prodIterator)->getVariable()->getID() == id )
        {
            found = true;
            break;
        }
    }    
    
    
    return found;
}

bool codegenerator::isNonTerminal(string id)
{
    bool found = false;
    
    vector<nonterminal*>::iterator iterNonTerminal;
    
    for (iterNonTerminal = nonterminalList.begin(); iterNonTerminal != nonterminalList.end(); ++iterNonTerminal) {
        if ( (*iterNonTerminal)->getID() == id )
        {
            found = true;
            break;
        }
    }    
    
    
    return found;
}

nonterminal* codegenerator::getNonTerminal(string id)
{
    vector<nonterminal*>::iterator iterNonTerminal;
    
    for (iterNonTerminal = nonterminalList.begin(); iterNonTerminal != nonterminalList.end(); ++iterNonTerminal) {
        if ( (*iterNonTerminal)->getID() == id )
        {
            return *iterNonTerminal;
        }
    }    
    
    return NULL;
}

terminal* codegenerator::getTerminal(string id)
{
    vector<nonterminal*>::iterator iterNonTerminal;
    
    vector<terminal*>::iterator iterTerminal;
    
    for (iterTerminal = terminalList.begin(); iterTerminal != terminalList.end(); ++iterTerminal) {
        if ( (*iterTerminal)->getID() == id )
        {
            return *iterTerminal;
        }
    }
    
    return NULL;
}

symbol* codegenerator::getSymbol(string id)
{
    symbol* symbolObj = NULL;
    
    symbolObj = getNonTerminal(id);
    
    if (symbolObj != NULL)
        return symbolObj;
    
    symbolObj = getTerminal(id);
    
    return symbolObj;
    
}


bool codegenerator::isSymbolDeclared(string id)
{
    bool foundTerminal, foundNonTerminal = false;
    
    foundTerminal = isTerminal(id);
    foundNonTerminal = isNonTerminal(id);

    
    return foundTerminal || foundNonTerminal;
}

void codegenerator::tagProductions()
{
    while (typeid(*lex->currentToken()) != typeid(TokenEof)) {
        
        //id
        string variableString = lex->currentToken()->lexema();
        
        if (isTerminal(variableString))
        {
            throw new compilerexception("Terminal symbol (" + variableString + ") cannot be on the left side of the production!");  
        }
        
        if (!isNonTerminal(variableString))
        {
           throw new compilerexception("Non Terminal Symbol (" + variableString + ") not declared!");    
        }
        
        nonterminal* symbol = getNonTerminal(variableString);
        
        lex->nextToken();
        
        if (typeid(*lex->currentToken()) != typeid(TokenProduction))
            throw new compilerexception("Token '->' expected!"); 
        
        lex->nextToken();
        
        parseSymbols(symbol);
        
        
        
        
    }
}

void codegenerator::parseSymbols(nonterminal* nonTerminalSymbol)
{
    
    
    while (true) {
        
        production* prod = new production(nonTerminalSymbol);
        
        gramatica.push_back(prod);
        
        string code = "";
        
        int symbolPosition = 0;

        
        if (typeid(*lex->currentToken()) == typeid(TokenJavaCode))
        {
            code = lex->currentToken()->lexema();
            lex->nextToken();
            
            //solo verificamos si sigue un epsilon, porque si es asi, entonces agregamos el codigo en esta produccion.
            if (typeid(*lex->currentToken()) == typeid(TokenSemicolon))
            {
                prod->setCodigo(code);
                prod->setCodigoId(codeID++);
            }
            else
            {
                
                string nextCodePrefix = CODEPREFIX + to_string(codeID);
                nonterminal* nt = new nonterminal(nextCodePrefix, "String");
                nonterminalList.push_back(nt);
                production* prodForCode = new production(nt);
                prodForCode->addSymbol(getSymbol(EPSILON));
                prodForCode->setCodigo(code);
                prodForCode->setCodigoId(codeID);
                codeID++;
                
                
                gramatica.push_back(prodForCode);
                
                prod->addSymbol(nt);

                symbolPosition++;
                 
            }
        }
        //else
        //    prod->addCode(" ");

        while (true)
        {
            
            if (typeid(*lex->currentToken()) == typeid(TokenPipe))
                break;
                
            
            if (typeid(*lex->currentToken()) != typeid(TokenID) && typeid(*lex->currentToken()) != typeid(TokenSemicolon)  )
                throw new compilerexception("Symbol expected!"); 
            
            if (typeid(*lex->currentToken()) == typeid(TokenSemicolon))
            {
                prod->addSymbol(getSymbol(EPSILON));
            }
            else
            {
                
                symbol* symbolObj = getSymbol(lex->currentToken()->lexema());
                string param = "";
                
                
                
                if (symbolObj == NULL)
                {
                   throw new compilerexception("Undefined symbol (" + lex->currentToken()->lexema() + ")!");  
                }
                
                lex->nextToken();
                
                if (typeid(*lex->currentToken()) == typeid(TokenColon))
                {
                   
                    
                    //viene un parametro
                    lex->nextToken();
                    
                    //debe venir un id.
                    if (typeid(*lex->currentToken()) == typeid(TokenID))
                    {
                        param = lex->currentToken()->lexema();
                        
                        throw new compilerexception("El profe dijo que mejor solo por posicion no por nombre :P (" + param + ")!");
                    
                        prod->addParametro(param, symbolObj, symbolPosition);
                        
                        lex->nextToken();
                    }
                    else
                        throw new compilerexception("Parameter id expected on symbol (" + symbolObj->getID() + "), received (" + lex->currentToken()->lexema() + ") instead!");  
                    
                }
                
                //ahora todos tienen un parametro.
                prod->addParametro("$" + to_string(symbolPosition), symbolObj, symbolPosition);
                
                code = "";
                
                if (typeid(*lex->currentToken()) == typeid(TokenJavaCode))
                {
                    code = lex->currentToken()->lexema();
                    lex->nextToken();
                    
                    if (typeid(*lex->currentToken()) == typeid(TokenPipe) || typeid(*lex->currentToken()) == typeid(TokenSemicolon) )
                    {
                        prod->setCodigo(code);
                        prod->setCodigoId(codeID);
                        codeID++;
                        prod->addSymbol(symbolObj);
                        
                    }
                    else
                    {
                        
                        string nextCodePrefix = CODEPREFIX + to_string(codeID);
                        nonterminal* nt = new nonterminal(nextCodePrefix, prod->getVariable()->getReturnObjectID());
                        nonterminalList.push_back(nt);
                        production* prodForCode = new production(nt);
                        prodForCode->addSymbol(getSymbol(EPSILON));
                        prodForCode->setCodigo(code);
                        prodForCode->setCodigoId(codeID);
                        
                        prodForCode->insertListaParametros(prod->getParametros());
                        
                        symbolPosition++;
                        prod->addParametro("$" + to_string(symbolPosition), prod->getVariable(), symbolPosition);
                        
                        codeID++;
                        
                        gramatica.push_back(prodForCode);
                        
                        prod->addSymbol(symbolObj);
                        
                        prod->addSymbol(nt);
                         
                    }
                    
                                       
                    //prod->addCode(code);
                }
                else
                    prod->addSymbol(symbolObj);
                
                
                
                symbolPosition++;
                
            }
            
            if (typeid(*lex->currentToken()) == typeid(TokenPipe) || typeid(*lex->currentToken()) == typeid(TokenSemicolon) )
                break; //next production from same variable.

        }
        
        if (typeid(*lex->currentToken()) == typeid(TokenPipe))
        {
            lex->nextToken();
            continue; //next production from same variable.
        }
        
        if (typeid(*lex->currentToken()) == typeid(TokenSemicolon))
        {
            lex->nextToken();
            break;
        }
        
        if (typeid(*lex->currentToken()) == typeid(TokenEof))
             throw new compilerexception("Unexpected Eof!"); 
        
    }
}
