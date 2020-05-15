/// @file AnalyzeProcess.h
///
/// @brief Класс для запуска анализатора на основе наших состояний
///
/// @author alexust27
/// Contact: ustinov1998s@gmail.com
///
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

    void processVarAssigmentFromFoo( VarAssigmentFromFooStatement *statement );

    void processVarAssigmentFromPointer( VarAssigmentFromPointerStatement *statement );

    void processVarAssigmentNew( VarAssigmentNewStatement *statement );

    void processDelete( DeleteStatement *statement );

    void processIF( IfStatement *statement );

    void processReturn( ReturnStatement *statement );


private:

    /// @brief Множество проанализированных функций
    std::set<std::string> processedFunctions;

    /// @brief Множество функции которые возвращают указатель на выделенную память
    std::set<std::string> allocatedFunctions;

    /// @brief Указатель на все функции
    std::map<std::string, Function *> *allFunctions;

    /// @brief Уникальный контекст для одной функции
    std::unique_ptr<ProcessContext> context;
};


#endif //ESCA_PROCESSCTX_H
