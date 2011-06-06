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
    tablaLR = new TablaLR();
    passId = 0;
    
    terminalList.push_back(new terminal(EPSILON));
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
        
        
        /*
        //display production list
        vector<production*>::iterator iterProd;
        
        for (iterProd = gramatica.begin(); iterProd != gramatica.end(); ++iterProd) {
            printProduction((*iterProd), 0);
            printf("\n");
        }
         */
        
        
        passId = 1;
        
        
        //next steps.
        
        ItemSet* item0 = new ItemSet();
        
        ProductionItem* tempProd = new ProductionItem(gramatica[0], this->getTerminal("$"), true);
        tempProd->setPassId(passId);
        item0->addProduction(tempProd);
        itemSetList[0] = item0;
        
        
        
        cerradura(item0);
        
        
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
        
        bool again;
        
        
        do
        {
            again = false;
            estadoActual = 0;
            
            
        do
        {
            
            
            
            
            currentState = listaEstados.find(estadoActual)->second;
       
            ItemSet* currentSet = itemSetList.find(estadoActual)->second;
            
            currentState->setItemSet(currentSet);
            
            /*if ( currentState->getIsProcessed() )
            {
                estadoActual++;
                continue;
            }*/
            
            currentState->setIsProcessed(true);
            vector<ProductionItem*>::iterator iterProdItem;
            
            long totalItems = currentSet->getProductionItems()->size();
            
            //for (iterProdItem = currentSet->getProductionItems().begin(); iterProdItem != currentSet->getProductionItems().end(); ++iterProdItem) {
            for (long i = 0; i < totalItems; i++) {
                
                ProductionItem* prodItem = (*currentSet->getProductionItems())[i];  //((ProductionItem*)*iterProdItem);
                if (!prodItem->getIsGotoProcessed() && prodItem->getPassId() == passId)
                {
                    prodItem->setGotoProcessed(true);
                    symbol* sym = prodItem->nextSymbol();
                    
                    //vector<symbol*>::iterator iterSymbolos;
                    
                    //for (iterSymbolos = symbols.begin(); iterSymbolos != symbols.end(); iterSymbolos++) {
                        
                        //symbol* sym = *iterSymbolos;
                        
                        
                        
                        //just for now.
                        if ( sym != NULL && sym->getID() == EPSILON )
                        {
                            
                            //sym = NULL;
                            
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
                                    
                                    Transition* transition = new Transition(sym, estadoNuevo);
                                    
                                    currentState->addTransition(transition);
                                    estadoNuevo->setItemSet(newSet);
                                    
                                    listaEstados[estadoSiguiente] = estadoNuevo;
                                    
                                    printf("Nuevo estado : %d\n", estadoSiguiente);
                                    
                                    
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
                                    Transition* transition = new Transition(sym, listaEstados.find(estadoFoundID)->second);
                                    currentState->addTransition(transition);
                                    
                                    //hay que agregar los elementos que no son identicos, es decir que el lookahead es diferente.
                                    //State* estadoTo = listaEstados.find(estadoFoundID)->second;
                                    
                                    //estadoTo->setIsProcessed(false);
                                    
                                    //if ( estadoTo->getItemSet() != NULL )
                                    {   
                                        //if ( estadoActual == estadoFoundID)
                                        //moveProductionFromItemSet(newSet, (itemSetList.find(estadoFoundID)->second));
                                        //again = true;
                                    }
                                    
                                    
                                    
                                    //delete newSet;
                                    
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
                    //}
                    
                    
                    
                }
            }    

        
            estadoActual++;
            
        
        } 
        while (estadoActual < estadoSiguiente);
            
        passId++;
            
        }
        while (again);
        
        
        printf("\n");
        
        printStates();
    
        
        printf("%d Estados creados.\n", estadoActual - 1);
        
        printf("Minimizando estados...\n");
        
        
        //minimize states
        minimizar();
        
        printf("\n\n\n\n");
        
        printStates();
        
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
            prodFrom->setPassId(passId + 1);
            itemSetTo->addProduction(prodFrom);
        }
        //else
          //  delete prodFrom;
    }
    
    
    return newProductionsAdded;
    
}


void codegenerator::minimizar()
{

    vector<State*> estadosNuevos;
    vector<State*> estadosToDelete;
    
    long totalStates = listaEstados.size();
    
    bool again;
    
    do {
    
        again = false;
        
    map<int, State*>::iterator itFirst;
    
    for (itFirst = listaEstados.begin(); itFirst != listaEstados.end(); ++itFirst) {
        //for (int i = 0; i < totalStates; i++) {
        
        State* estado1 = itFirst->second;// listaEstados[i];
        
        if (!estado1->getMarkedForDelete())
        {
            map<int, State*>::iterator itSecond;
            
            for (itSecond = listaEstados.begin(); itSecond != listaEstados.end(); ++itSecond) {
            
                //for (int j = 0; j < totalStates; j++) {
                
                State* estado2 = itSecond->second;// listaEstados[j];
                
                if (!estado2->getMarkedForDelete())
                {
                    
                    if ( itFirst->second->getID() != itSecond->second->getID() && !estado1->getMarkedForDelete() )
                    {
                        if (itemSetEqual(estado1->getItemSet(), estado2->getItemSet()))
                        {
                            again = true;
                            //printf(".");
                            
                            //Crear nuevo estado de 1 y 2
                            
                            State* estadoNuevo = new State(false, estadoSiguiente);
                            
                            
                            //movemos todas las transiciones de estos estados al nuevo.
                            moveTransitions(estado1, estadoNuevo);
                            moveTransitions(estado2, estadoNuevo);
                            
                            //creamos una lista de merges, para saber de donde vino posteriormente. Solo para debug
                            string oldList =  (estado1->getMergeStateList() + estado2->getMergeStateList() == ""?"":estado1->getMergeStateList() + estado2->getMergeStateList());
                            string mergeList = (oldList == ""?"[":"[" + oldList + " - ") + to_string(estado1->getID()) + " : " + to_string(estado2->getID()) + "](" + to_string(estadoSiguiente) + ")";
                            
                            estadoNuevo->setMergeStateList(mergeList);
                            
                            printf("%s\n", mergeList.c_str());
                            
                            
                            //buscamos todas las transiciones que apuntan a los estados viejos y les asignamos en nuevo estado
                            moveTransitionsTo(estado1, estadoNuevo);
                            moveTransitionsTo(estado2, estadoNuevo);
                            
                            moveAccionesTo(estado1, estadoNuevo);
                            moveAccionesTo(estado2, estadoNuevo);
                            
                            moveAcciones(estado1, estadoNuevo);
                            moveAcciones(estado2, estadoNuevo);
                            
                            moveIra(estado1, estadoNuevo);
                            moveIra(estado2, estadoNuevo);
                            
                            estadoNuevo->setItemSet(estado1->getItemSet());
                            
                            //moveProductionFromItemSet(estado1->getItemSet(), estadoNuevo->getItemSet());
                            moveProductionFromItemSet(estado2->getItemSet(), estadoNuevo->getItemSet());
                            
                            estadosNuevos.push_back(estadoNuevo);
                            
                            
                            
                            estado1->setMarkedForDelete(true);
                            estado2->setMarkedForDelete(true);
                            estadosToDelete.push_back(estado1);
                            estadosToDelete.push_back(estado2);
                            
                            //set next possible next state id
                            estadoSiguiente++;
                        }
                    }
                }
            }
        }
    }
    
    
    //ahora borramos los estados de la lista de estados.
    
    int t = listaEstados.size();
    int y = estadosNuevos.size();
    totalStates = estadosToDelete.size();
    
    int z = t + y - totalStates;
    
    for (int i = 0; i < totalStates; i++) {
        
        State* estado = estadosToDelete[i];
        
        listaEstados.erase(estado->getID());
        
        
        //como ya no debe de haber ninguna referencia, liberamos la memoria
        //delete estado;
        //if (estado != NULL)
        //delete estado;
    }
    
    int t2 = listaEstados.size();
    int y2 = estadosNuevos.size();
    
    
    //ahora agregamos los nuevos estados a la lista de estados.
    totalStates = estadosNuevos.size();
    
    for (int i = 0; i < totalStates; i++) {
        
        State* estado = estadosNuevos[i];
        
        listaEstados.insert(pair<int, State*>(estado->getID(), estado));
    }
        
        
        
    } while (again);
    
    
}


void codegenerator::moveAccionesTo(State *estadoFrom, State *estadoTo)
{
    long totalStates = listaEstados.size();
    
    
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



void codegenerator::moveAcciones(State *estadoFrom, State *estadoTo)
{
    //recorremos las transiciones de ambos estados y las agregamos al nuevo estado.
    //agregar transiciones
    long totalAcciones = estadoFrom->getListaAcciones().size();
    
    for (long j = 0; j < totalAcciones; j++) {
        
        Accion* accion = estadoFrom->getListaAcciones()[j];
        
        
        if (!estadoTo->accionExists(accion))
            estadoTo->addAccion(accion);
        //else
        //{
        //    //error
        //    string tipoAccion = (accion->getTipo() == 0?"Desplazar":(accion->getTipo() == 1?"Reducir":"Aceptar"));
        //    throw new compilerexception("La acción de " + tipoAccion + " esta duplicada con el terminal : " + accion);
        //}
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
        //else
        //{
        //    //error
        //    string tipoAccion = (accion->getTipo() == 0?"Desplazar":(accion->getTipo() == 1?"Reducir":"Aceptar"));
        //    throw new compilerexception("La acción de " + tipoAccion + " esta duplicada con el terminal : " + accion);
        //}
    }
    
    estadoFrom->getListaIra().clear();
}


void codegenerator::moveTransitionsTo(State *estadoFrom, State *estadoTo)
{
    long totalStates = listaEstados.size();
    
    map<int, State*>::iterator itStates;
    
    for (itStates = listaEstados.begin(); itStates != listaEstados.end(); ++itStates) {
        
        //for (int i = 0; i < totalStates; i++) {
        
        State* estado = itStates->second;// listaEstados[i];
        
        
        
        long totalTransiciones = estado->getTransitions().size();
        
        for (long j = 0; j < totalTransiciones; j++) {
            
            Transition* transition = estado->getTransitions()[j];
            
            if ( transition->getState()->getID() == estadoFrom->getID() )
            {
                transition->setState(estadoTo);
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
            
        for (iterProdItem = (*itemSet->getProductionItems()).begin(); iterProdItem != (*itemSet->getProductionItems()).end(); ++iterProdItem) {
            
            ProductionItem* prodItem = ((ProductionItem*)*iterProdItem);// (*itemSet->getProductionItems())[j];
            
            printf("I%d (%d)", i, estado->getID());
            
            printProduction(prodItem->getProduction(), prodItem->getDotPosition());
            
            printf("/ %s", (prodItem->getLookAhead()->getID().c_str()));
            
            printf(" -- %s", prodItem->getIsGotoProcessed()?"YES":"NO");
            
            printf("\r\n");
        }
        
        printf("\r\n");
        
        
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
            
            
            printf("with %s to %d\n", transition->getConsume()->getID().c_str(), transition->getState()->getID());
            
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
    
    
    return -1;
}



void codegenerator::insertarProducciones(FILE *file)
{
    fputs("\t\t//inicio producciones\r\n", file);
    
    string data;
    
    
    
    
    fputs("\t\t//inicio lista simbolos\n", file);
    
    
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
        
        data = "\t\tsymbols.put(String.valueOf(" + to_string(nt->getNumericID()) + "), new Nonterminal(\"" + nt->getID() + "\", " + to_string(nt->getNumericID()) + "));\n";
        fputs(data.c_str(), file);
        
        numericID++;
    }    

    
    fputs("\t\t//fin lista simbolos\n", file);
    
    
    
    
    //fputs("\t\tNonterminal nonterminal = null;\n", file);
    //fputs("\t\tTerminal terminal = null;\n", file);
    
    fputs("\t\tProduction prod = null;\n", file);
    
    
    long totalProducciones = gramatica.size();
    
    for (int i = 0; i < totalProducciones; i++) {
        
        production* prod = gramatica[i];
        
        
        
        
        
        fputs("\t\t//Produccion...\n", file);
        
        data = "\t\tprod = new Production((Nonterminal)symbols.get(String.valueOf(" + to_string(prod->getVariable()->getNumericID()) + ")));\n";
        
        fputs(data.c_str(), file);
        
        vector<symbol*>::iterator listaTerminales;
        
        long totalTerminales = prod->getSymbols().size();
        
        fputs("\t\t//simbolos en la produccion\n", file);
        
        
        for (int j = 0; j < totalTerminales; j++) {
            
            //for (listaTerminales = prod->getSymbols().begin(); listaTerminales != prod->getSymbols().end(); ++listaTerminales) {
            symbol* simbolo = prod->getSymbols()[j]; //*listaTerminales;
            
            if (typeid(*simbolo) == typeid(terminal))
            {
                data = "\t\tprod.getSimbolos().add(symbols.get(String.valueOf(" + to_string(simbolo->getNumericID()) + ")));\n";
                fputs(data.c_str(), file);
            }
            else
            {
                data = "\t\tprod.getSimbolos().add(symbols.get(String.valueOf(" + to_string(simbolo->getNumericID()) + ")));\n";
                fputs(data.c_str(), file);
            }
        }

        fputs("\t\t//fin simbolos produccion\n", file);
        
        data = "\t\tgramatica.getProductions().add(prod);\n";
        fputs(data.c_str(), file);
        fputs("\t\t//fin produccion\n", file);
        
    }
    
    
    fputs("\t\t//fin producciones\n\n", file);

    
}

void codegenerator::insertarEstados(FILE *file)
{

    fputs("\t\t//inicio estados\n\n", file);
    
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
        data = "\t\tstate = new State(" + tonto +(estado->isAcceptState()?"true":"false") + tonto + ", " + to_string(estado->getID()) + ");\n";
        fputs(data.c_str(), file);
        
        //agregar transiciones
        long totalTransiciones = estado->getTransitions().size();
        
        //map<terminal*, Accion*>::iterator iterAcciones = estado->getListaAcciones().begin();
        
        for (long j = 0; j < totalTransiciones; j++) {
            
            
            //terminal* ter = (*iterAcciones).first;
            Transition* transition = estado->getTransitions()[j];
            
            data = "\t\ttransition = new Transition(symbols.get(String.valueOf(" + to_string(transition->getConsume()->getNumericID()) + ")), " + to_string(transition->getState()->getID()) + ");\n";
            fputs(data.c_str(), file);
            
            fputs("\t\tstate.getTransitions().add(transition);\n", file);
        }
        
        
        
        //agregar acciones
        fputs("\n\n", file);
        
        fputs("\t\t//Accion\n\n", file);
       
        long totalAcciones = estado->getListaAcciones().size();
        
        //map<terminal*, Accion*>::iterator iterAcciones = estado->getListaAcciones().begin();
        
        for (long j = 0; j < totalAcciones; j++) {
            
            
            //terminal* ter = (*iterAcciones).first;
            Accion* accion = estado->getListaAcciones()[j];
            
            data = "\t\taccion = new Accion(" + tonto + (accion->getTipo() == SHIFT?"Accion.tipo_accion.SHIFT":(accion->getTipo() == REDUCE?"Accion.tipo_accion.REDUCE":"Accion.tipo_accion.ACCEPT")) + tonto + ", " + to_string(accion->getEstadoID()) + ");\n";
            fputs(data.c_str(), file);
            
            data = "\t\tstate.getListaAcciones().put((Terminal)symbols.get(String.valueOf(" + to_string(accion->getTerminal()->getNumericID()) + ")), accion);\n";
            fputs(data.c_str(), file);
            
        }

        

        //agregar ir_a
        
        fputs("\n\n", file);
        fputs("\t\t//Ir a\n\n", file);
        
        long totalIra = estado->getListaIra().size();
        
        //map<terminal*, Accion*>::iterator iterAcciones = estado->getListaAcciones().begin();
        
        for (long j = 0; j < totalIra; j++) {
            
            
            //terminal* ter = (*iterAcciones).first;
            Ira* ira = estado->getListaIra()[j];
            
            data = "\t\tira = new Ira(" + to_string(ira->getEstadoID()) + ");\n";
            fputs(data.c_str(), file);
            
            data = "\t\tstate.getListaIra().put((Nonterminal)symbols.get(String.valueOf(" + to_string(ira->getNonTerminal()->getNumericID()) + ")), ira);\n";
            fputs(data.c_str(), file); 
        }
        
        fputs("\n\n", file);
        fputs("\t\t//fin ira a\n\n", file);
        
        
        data = "\t\tstates.put(\"" + to_string(estado->getID()) + "\", state);\n";
        fputs(data.c_str(), file);
        
        fputs("\n\n", file);
        fputs("\t\t//fin estado\n\n", file);
        
        
        
    }
}


void codegenerator::generateJavaSymFile()
{
    
    string symFileName = "/Users/carlosh/Documents/workspace/Parser/src/sym.java";
    
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
    fileName = "/Users/carlosh/Documents/workspace/Parser/src/" + this->className + ".java";
    
    printf("Creando archivo %s\n", fileName.c_str());
    
    FILE* file = NULL;
	file = fopen(fileName.c_str(), "w");
    
    fputs("\r\n\r\n", file);
    //fputs("import Parser;\r\n\r\n", file);
    
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

bool codegenerator::itemSetEqual(ItemSet *itemSetA, ItemSet *itemSetB)
{
    ItemSet *menor;
    ItemSet *mayor;
    
    
    
    //if (itemSetA->getProductionItems()->size() != itemSetB->getProductionItems()->size())
    //    return false;
 
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
        
        long totalItemsMayor = mayor->getProductionItems()->size();
        
        bool isEqual = false;
        for (int i = 0; i < totalItemsMayor; i++) {

            ProductionItem* prodMayor = (*mayor->getProductionItems())[i];
            
            if ( productionItemEqual(prodMayor, prodMenor, false, true) )
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
    
    if (itemSetA->getProductionItems()->size() == 0 || itemSetB->getProductionItems()->size() == 0 )
        return false;
    
    return true;
}

void codegenerator::printProduction(production* prod, int dotPosition)
{
    printf("%s -> ", (prod)->getVariable()->getID().c_str());
    
    vector<symbol*>::iterator iterSymbol;
    
    vector<string> codigo = (prod)->getCodigo();
    
    //int symbolPos = 0;
    long totalSym = (prod)->getSymbols().size();
    int symbolPos;
    for (symbolPos = 0; symbolPos < totalSym; symbolPos++) {
        //for (iterSymbol = (prod)->getSymbols().begin(); iterSymbol != (prod)->getSymbols().end(); ++iterSymbol) 
        //{
        
        symbol* sym = (prod)->getSymbols()[symbolPos];
        
        printf("%s ", codigo[symbolPos].c_str());
        
        if (symbolPos == dotPosition)
            printf("%s ", "(*)");

        printf("%s ", sym->getID().c_str());
        
               
        
        //symbolPos++;
    }
    
    if (symbolPos == dotPosition)
        printf("%s ", "(*)");
    
    if (symbolPos < codigo.size())
        printf("%s ", codigo[symbolPos].c_str());
    
    

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


/*
vector<symbol*> codegenerator::getLookAhead(symbol* symB, symbol* symA)
{
    vector<symbol*> terms;

    
    if (symB == NULL)
        terms.push_back(getTerminal("$"));
    else
    {
        vector<production*>::iterator iterProd;
        
        for (iterProd = gramatica.begin(); iterProd != gramatica.end(); ++iterProd) {
            if ((*iterProd)->getVariable()->getID() == symB->getID())
            {
                if ( isTerminal((*iterProd)->getSymbols()[0]->getID()) )
                {
                    terms.push_back((*iterProd)->getSymbols()[0]);
                }
            }
            
        }
    }
    
    return terms;
}
*/


void codegenerator::processItem(ItemSet* item)
{
    do {
    
        vector<ProductionItem*>::iterator iterProdItem;
        vector<production*>::iterator iterProd;
        
        vector<production*> nuevasProducciones;

        
        for (iterProdItem = item->getProductionItems()->begin(); iterProdItem != item->getProductionItems()->end(); ++iterProdItem) {
            ProductionItem* prodItem = ((ProductionItem*)*iterProdItem);
            
            if ( !prodItem->getIsProcessed() )
            {
                symbol* sym = prodItem->nextSymbol();
                
                if ( sym != NULL )
                {        
                    if ( isNonTerminal(sym->getID()) )
                    {
                        //vector<production*> temp = cerradura((nonterminal*)sym);
                        //nuevasProducciones.insert(nuevasProducciones.end(), temp.begin(), temp.end());
                    }
                }
            
                prodItem->setIsProcessed(true);
            }
        }
    
        
        if ( nuevasProducciones.size() > 0 )
        {
            for (iterProd = nuevasProducciones.begin(); iterProd != nuevasProducciones.end(); ++iterProd) {
                
                ProductionItem* newItem = new ProductionItem(*iterProd, this->getTerminal("$"), false); 
                
                
                //symbol* sym = newItem->nextSymbol();
                
                //if ( isNonTerminal(sym->getID()) )
                //{
                //    prodItem->setLookAhead(getTerminal("$"));
                //}
                //else
                //{
                //vector<production*> nuevasProducciones;
                //nuevasProducciones = cerradura(<#nonterminal *sym#>)
                
                //prodItem->setLookAhead(getTerminal(sym->getID()));
                //}

                
                item->addProduction(newItem);
            }
            nuevasProducciones.clear(); 
        }
        else
        {
            /*
            //ya no hay mas producciones en este nivel, consumamos los caracteres para el siguiente nodo.
            
            vector<ProductionItem*>::iterator iterProdItem;
            vector<production*>::iterator iterProd;
            
            for (iterProdItem = item->getProductionItems().begin(); iterProdItem != item->getProductionItems().end(); ++iterProdItem) {
                
                ProductionItem* prodItem = ((ProductionItem*)*iterProdItem);
                
                ProductionItem* newProdItem = new ProductionItem(prodItem->getProduction(), NULL, true);
                newProdItem->setDotPosition(prodItem->getDotPosition());
                
                
                
                if ( newProdItem->nextSymbol() != NULL )
                {
                    //move to next symbol
                    newProdItem->moveDot();
                    
                    currentItemSet++;
                    ItemSet* itemN = new ItemSet();
                    itemN->addProduction(newProdItem);
                    itemSetList[currentItemSet] = itemN;
                    
                    //processItem(itemN);
                }
                
                
            }
             */
            
            break;
        }

    } while (true);
    

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
                
                
                newItem->setPassId(passId);
                
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
                                    nuevoItem->setPassId(passId);
                                    
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
                                    nuevoItem->setPassId(passId);
                                    
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

/*
bool codegenerator::productionEqual(production *a, production *b)
{
    if ( a->getSymbols().size() == b->getSymbols().size() )
    {
        vector<symbol*>::iterator listaTerminales;
        
        int i = 0;
        for (listaTerminales = a->getSymbols().begin(); listaTerminales != a->getSymbols().end(); ++listaTerminales) {
            symbol* simboloA = *listaTerminales;
            
            if ( b->getSymbols()[i] != simboloA )
            {
                return false;
            }
            
            i++;
        }
        
        return true;
        
    }
    else
        return false;
}
 */

/*
vector<production*> codegenerator::cerradura(nonterminal* sym)
{
    //display production list
    vector<production*>::iterator iterProd;
    vector<production*> nuevasProducciones;
    
    for (iterProd = gramatica.begin(); iterProd != gramatica.end(); ++iterProd) {
        if ( (*iterProd)->getVariable()->getID() == sym->getID() )
        {
            nuevasProducciones.push_back(*iterProd);
        }
    }
    
    return nuevasProducciones;
}
 */

/*
void codegenerator::cerradura(nonterminal* variable)
{
    
}
*/

void codegenerator::aumentar()
{
    if (gramatica.size() > 0) {
        nonterminal* t = new nonterminal(START, gramatica[0]->getVariable()->getReturnObjectID());
        t->setNumericID(0);
        production* inicio = new production(t);
        inicio->addCode("");
        inicio->addCode("");
        
        inicio->addSymbol(gramatica[0]->getVariable());
        //inicio->addSymbol(getTerminal(EOFT));
        
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
            importCode = true;
            Token* token;
            while((token = lex->nextToken()))
            {
                //printf("%s -> %s \r\n", token->lexema().c_str(), token->className());
                
                //TODO: do something later
                
                if ( typeid(*token) == typeid(TokenTagClose) )
                    break;
                else if ( typeid(*token) == typeid(TokenEof) )
                    throw new compilerexception("Unexpected Eof token. Expected '%>' instead.");
                
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
            parseCode = true;
            Token* token;
            while((token = lex->nextToken()))
            {
                //printf("%s -> %s \r\n", token->lexema().c_str(), token->className());
                
                //TODO: do something later
                
                if ( typeid(*token) == typeid(TokenTagClose) )
                    break;
                else if ( typeid(*token) == typeid(TokenEof) )
                    throw new compilerexception("Unexpected Eof token. Expected '%>' instead.");
                
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
        Token* token;
        while((token = lex->nextToken()))
        {
            //printf("%s -> %s \r\n", token->lexema().c_str(), token->className());
            if ( typeid(*token) == typeid(TokenID) )
            {
                string id = lex->currentToken()->lexema();
                               
                if (isSymbolDeclared(id))
                {
                    throw new compilerexception("Symbol ID (" + id + ") already declared!");
                }
                
                //simbolo
                terminal* terminalSymbol = new terminal(id);
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
        
        if (typeid(*lex->currentToken()) == typeid(TokenJavaCode))
        {
            code = lex->currentToken()->lexema();
            lex->nextToken();
            
            prod->addCode(code);
        }
        else
            prod->addCode(" ");

        
        while (true)
        {
            
            if (typeid(*lex->currentToken()) == typeid(TokenPipe))
                break;
                
            
            if (typeid(*lex->currentToken()) != typeid(TokenID) && typeid(*lex->currentToken()) != typeid(TokenSemicolon)  )
                throw new compilerexception("Symbol expected!"); 
            
            if (typeid(*lex->currentToken()) == typeid(TokenSemicolon))
            {
                //Vamos a probar de nuevo. No fue buena idea de agregar in EPSILON, mejor dejamos vacio.
                prod->addSymbol(getSymbol(EPSILON));
            }
            else
            {
                
                symbol* symbolObj = getSymbol(lex->currentToken()->lexema());
                
                if (symbolObj == NULL)
                {
                   throw new compilerexception("Undefined symbol (" + lex->currentToken()->lexema() + ")!");  
                }
                
                lex->nextToken();
                
                if (typeid(*lex->currentToken()) == typeid(TokenJavaCode))
                {
                    code = lex->currentToken()->lexema();
                    lex->nextToken();
                    
                    prod->addCode(code);
                }
                else
                    prod->addCode(" ");
                
                
                prod->addSymbol(symbolObj);
                
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
