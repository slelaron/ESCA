//
// Created by alex on 10.05.2020.
//

#ifndef ESCA_PROCESSCTX_H
#define ESCA_PROCESSCTX_H

#include "Context.h"
#include "ProcessContext.h"

using namespace Target;

class AnalyzeProcess
{

public:
    AnalyzeProcess();

    void StartAnalyze();

private:

    void processFunction( Target::Function *function );

    void processStatement( Statement *stmt );

    void processCompound( Target::CompoundStatement *statement );

    void processDelete( DeleteStatement *statement );

    void processIF( IfStatement *statement );

    void processVarAssigmentFromFoo( VarAssigmentFromFooStatement *statement );

    void processVarAssigmentFromPointer( VarAssigmentFromPointerStatement *statement );

    void processVarAssigmentNew( VarAssigmentNewStatement *statement );

    void processVarDeclNew( VarDeclNewStatement *statement );

    void processReturn( ReturnStatement *statement );

    void processVarDeclFromFoo( VarDeclFromFooStatement *statement );


private:

    /// @brief Множество проанализированных функций
    std::set<std::string> processedFunctions;

    /// @brief множество функции которые возвращают указатель на выделенную память
    std::set<std::string> allocatedFunctions;

    /// @brief Указатель на все функции
    std::map<std::string, Function *> *allFunctions;

    std::unique_ptr<ProcessContext> context;
};


#endif //ESCA_PROCESSCTX_H
