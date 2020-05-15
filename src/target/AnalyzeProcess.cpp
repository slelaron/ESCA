/// @file AnalyzeProcess.cpp
///
/// @brief Класс для запуска анализатора на основе полученных состояний
///
/// @author alexust27
/// Contact: ustinov1998s@gmail.com
///

#include "AnalyzeProcess.h"

#include <VariableSMT.h>
#include <BinarySMT.h>
#include <iostream>

AnalyzeProcess::AnalyzeProcess()
{
    allFunctions = Context::Instance().getAllFunction();
    context = nullptr;
    allocatedFunctions.insert(std::string("malloc"));
    allocatedFunctions.insert(std::string("socket"));
    allocatedFunctions.insert(std::string("accept"));
    allocatedFunctions.insert(std::string("fopen"));
}

void AnalyzeProcess::StartAnalyze()
{
    std::cout << "Start analyze." << std::endl;
    for( const auto &f : *allFunctions )
    {
        processFunction(f.second);
    }
}

void AnalyzeProcess::processFunction( Target::Function *function )
{
    auto funName = function->getName();
    if( processedFunctions.find(funName) != processedFunctions.end())
    {
        // already in process
        return;
    }

    // process all callee
    for( const auto &c : function->callee )
    {
        auto calleeFun = allFunctions->find(c);
        if( calleeFun == allFunctions->end())
        {
            // unknown function
            continue;
        }

        // prevent recursion
        if( funName != calleeFun->first )
        {
            processFunction(calleeFun->second);
        }
    }

    // process
    context = std::make_unique<ProcessContext>(funName);

    processCompound(function->startState());

    if( !function->returnName.empty())
    {
        context->fsm->SetReturnVarName(function->returnName);
    }
    context->fsm->ProcessReturnNone();
    processedFunctions.insert(funName);
    context.reset();
}

void AnalyzeProcess::processStatement( Statement *stmt )
{
    switch( stmt->GetType())
    {
        case COMPOUND:
        {
            processCompound(dynamic_cast<CompoundStatement *>(stmt));
            break;
        }
        case VarAssigmentNew:
        {
            processVarAssigmentNew(dynamic_cast<VarAssigmentNewStatement *>(stmt));
            break;
        }
        case VarAssigmentFromFoo:
        {
            processVarAssigmentFromFoo(dynamic_cast<VarAssigmentFromFooStatement *>(stmt));
            break;
        }
        case VarAssigmentFromPointer:
        {
            processVarAssigmentFromPointer(dynamic_cast<VarAssigmentFromPointerStatement *>(stmt));
            break;
        }
        case DELETE:
        {
            processDelete(dynamic_cast<DeleteStatement *>(stmt));
            break;
        }
        case IF:
        {
            processIF(dynamic_cast<IfStatement *>(stmt));
            break;
        }
        case Return:
        {
            processReturn(dynamic_cast<ReturnStatement *>(stmt));
            break;
        }
        default:
        {
            std::cerr << "Unknown Statement for analyze" << std::endl;
            return;
        }
    }
}


void AnalyzeProcess::processCompound( Target::CompoundStatement *statement )
{
    for( auto st : statement->getStates())
    {
        processStatement(st);
    }
}

void AnalyzeProcess::processVarAssigmentNew( VarAssigmentNewStatement *statement )
{
    auto varName = statement->varName;
    PtrCounter &lhsCnt = context->variables[ varName ];
    ++(lhsCnt.count);

    StateFSM state;
    VersionedVariable vv(varName, statement->loc, VAR_POINTER, lhsCnt.count);
    if( statement->isArray )
    {
        vv.MetaType(VAR_ARRAY_POINTER);
        lhsCnt.meta = VAR_ARRAY_POINTER;
        state.allocArrays.push_back(vv);
    }
    else
    {
        lhsCnt.meta = VAR_POINTER;
        state.allocPointers.push_back(vv);
    }
    //Отметить new.
    context->allocated.push_back(vv);

    std::shared_ptr<VariableSMT> vvFormulae(new VariableSMT(vv));
    state.formulae.push_back(vvFormulae);

    context->fsm->AddStateToLeaves(state, context->fairPred);
}

void AnalyzeProcess::processVarAssigmentFromFoo( VarAssigmentFromFooStatement *statement )
{
    PtrCounter &lhsCnt = context->variables[ statement->varName ];

    if( allocatedFunctions.find(statement->fooName) !=
        allocatedFunctions.end())
    {
        ++(lhsCnt.count);
        StateFSM state;
        VersionedVariable vv(statement->varName, statement->loc, VAR_POINTER, lhsCnt.count);
        {
            // TODO: поддержать как царь разные варианты для массива и просто указателя
            state.allocPointers.push_back(vv);
            lhsCnt.meta = VAR_POINTER;
        }
//        if( statement->isDecl )
//        {
        //Отметить new.
        context->allocated.push_back(vv);
//        }

        std::shared_ptr<VariableSMT> vvForm(new VariableSMT(vv));
        state.formulae.push_back(vvForm);

        context->fsm->AddStateToLeaves(state, context->fairPred);
    }
}

void AnalyzeProcess::processVarAssigmentFromPointer( VarAssigmentFromPointerStatement *statement )
{
    auto varName = statement->varName;
    PtrCounter &lhsCnt = context->variables[ varName ];
    int lhsVer = ++(lhsCnt.count);

    StateFSM state;
    VersionedVariable lhsVar(varName, statement->loc, VAR_POINTER, lhsVer);
    //VariableSMT *lhsForm = new VariableSMT();
    std::shared_ptr<VariableSMT> lhsForm(new VariableSMT(lhsVar));
    state.formulae.push_back(lhsForm);

    PtrCounter &rhsCnt = context->variables[ statement->rhsName ];
    int rhsVer = rhsCnt.count;
    VersionedVariable rhsVar(statement->rhsName, statement->loc, VAR_POINTER, rhsVer);
    //VariableSMT *rhsForm = new VariableSMT();
    std::shared_ptr<VariableSMT> rhsForm(new VariableSMT(rhsVar));
    state.formulae.push_back(rhsForm);
    lhsCnt.meta = rhsCnt.meta;

    //shared_ptr<VariableSMT> leftForm(new VariableSMT);
    //leftForm->Var(lhsVar);
    //state.formulae.push_back(leftForm);
    std::shared_ptr<FormulaSMT> bs(new BinarySMT(lhsVar, rhsVar, EqualSMT, false));
    state.formulae.push_back(bs);

    context->fsm->AddStateToLeaves(state, context->fairPred);
}


void AnalyzeProcess::processDelete( DeleteStatement *statement )
{
    auto cntIter = context->variables.find(statement->name);
    VersionedVariable vv(statement->name, "unused", cntIter->second.meta, cntIter->second.count);

    context->fsm->AddDeleteState(vv, statement->isArray);
}

void AnalyzeProcess::processIF( IfStatement *statement )
{
    // process then
    // скорей всего в then находятся какие то простые действия, которые нам не интересны
    // например присвоение констант или еще что то такое
    if( statement->thenSt )
    {
//        ctx.fsm->PushCondition(condStr);
        StateFSM s;
        context->fsm->AddStateToLeaves(s, context->fairPred, statement->condStr, false);
        processStatement(statement->thenSt);
//        ctx.fsm->PopCondition();
    }

    // process else
    // тоже самое, как для then, но есть еще случай, когда else вообще отсутствует
    if( statement->elseSt )
    {
//        ctx.fsm->PushCondition(elseStr);
        StateFSM s;
        context->fsm->AddStateToLeaves(s, context->branchPred, statement->elseStr, true);
        processStatement(statement->elseSt);
//        ctx.fsm->PopCondition();
    }
}


void AnalyzeProcess::processReturn( ReturnStatement *statement )
{
    if( !statement->returnVarName.empty())
    {
        std::set<std::string> tmp;
        tmp.insert(statement->returnVarName);
        context->fsm->SetReturnVarName(tmp);
    }

    context->fsm->ProcessReturnNone();

    bool isAllocFoo = context->fsm->IsAllocReturns();
    if( isAllocFoo )
    {
        allocatedFunctions.insert(context->fsm->FunctionName());
    }

    context->fsm->ClearReturnVarName();
}



