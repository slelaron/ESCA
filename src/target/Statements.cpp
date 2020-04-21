#include <StateFSM.h>
#include <VariableSMT.h>
#include <BinarySMT.h>
#include "Function.h"

extern std::set<std::string> allocatedFunctions;

namespace Target
{

void CompoundStatement::addState( Statement *st )
{
    statements.push_back(st);
}

void CompoundStatement::process( ProcessCtx &ctx )
{
    for( auto st : statements )
    {
        st->process(ctx);
    }
}

DeleteStatement::DeleteStatement( const std::string &name, bool isArray )
        : name(name), isArray(isArray)
{
}

void DeleteStatement::process( ProcessCtx &ctx )
{
    auto cntIter = ctx.variables.find(name);
    VersionedVariable vv(name, "unused", cntIter->second.meta, cntIter->second.count);

    ctx.fsm.AddDeleteState(vv, isArray);
}

IfStatement::IfStatement( Statement *thenSt, Statement *elseSt, const std::string &condStr,
                                  const std::string &elseStr )
        : thenSt(thenSt), elseSt(elseSt), condStr(condStr), elseStr(elseStr)
{
    static int a = 0;
    a++;
}

void IfStatement::process( ProcessCtx &ctx )
{
    // process then
    // скорей всего в then находятся какие то простые действия, которые нам не интересны
    // например присвоение констант или еще что то такое
    if( thenSt )
    {
//        ctx.fsm.PushCondition(condStr);
        StateFSM s;
        ctx.fsm.AddStateToLeaves(s, ctx.fairPred, condStr, false);
        thenSt->process(ctx);
//        ctx.fsm.PopCondition();
    }

    // process else
    // тоже самое, как для then, но есть еще случай, когда else вообще отсутствует
    if( elseSt )
    {
//        ctx.fsm.PushCondition(elseStr);
        StateFSM s;
        ctx.fsm.AddStateToLeaves(s, ctx.branchPred, elseStr, true);
        elseSt->process(ctx);
//        ctx.fsm.PopCondition();
    }
}

VarDeclFromFooStatement::VarDeclFromFooStatement( const std::string &varName, const std::string &fooName,
                                                          const std::string &loc )
        : varName(varName), fooName(fooName), loc(loc)
{
}

void VarDeclFromFooStatement::process( ProcessCtx &ctx )
{
    PtrCounter ptrCnt = {
            0,
            VAR_POINTER
    };
    //variables[name] = ptrCnt;
    auto cntIter = ctx.variables.insert({varName, ptrCnt});

    if( allocatedFunctions.find(fooName) != allocatedFunctions.end())
    {

        ++cntIter.first->second.count;

        // TODO: поддержать как царь разные варианты для массива и просто указателя
        StateFSM state;
        VersionedVariable vv(varName, loc, VAR_POINTER, 1);
        {
            state.allocPointers.push_back(vv);
        }
        //Отметить функцию как new.
        std::shared_ptr<VariableSMT> vvForm(new VariableSMT(vv));
        state.formulae.push_back(vvForm);

        ctx.fsm.AddStateToLeaves(state, ctx.fairPred);
    }
}

VarDeclNewStatement::VarDeclNewStatement( const std::string &varName, bool isArray, const std::string &loc )
        : varName(varName), isArray(isArray), loc(loc)
{
}

void VarDeclNewStatement::process( ProcessCtx &ctx )
{
    PtrCounter ptrCnt = {
            0,
            VAR_POINTER
    };
    //variables[name] = ptrCnt;
    auto cntIter = ctx.variables.insert({varName, ptrCnt});

    ++cntIter.first->second.count;

    StateFSM state;
    VersionedVariable vv(varName, loc, VAR_POINTER, 1);

    if( isArray ) //Declaration of array
    {
        vv.MetaType(VAR_ARRAY_POINTER);
        cntIter.first->second.meta = VAR_ARRAY_POINTER;
        state.allocArrays.push_back(vv);
    }
    else
    {
        state.allocPointers.push_back(vv);
    }
    ctx.allocated.push_back(vv);

    std::shared_ptr<VariableSMT> vvForm(new VariableSMT(vv));
    state.formulae.push_back(vvForm);

    ctx.fsm.AddStateToLeaves(state, ctx.fairPred);
}

VarAssigmentFromFooStatement::VarAssigmentFromFooStatement( const std::string &varName,
                                                                    const std::string &fooName, const std::string &loc )
        : varName(varName), fooName(fooName), loc(loc)
{
}

void VarAssigmentFromFooStatement::process( ProcessCtx &ctx )
{
    PtrCounter &lhsCnt = ctx.variables[ varName ];
    int lhsVer = ++(lhsCnt.count);

    if( allocatedFunctions.find(fooName) != allocatedFunctions.end())
    {
        StateFSM state;
        VersionedVariable vv(varName, loc, VAR_POINTER, lhsCnt.count);
        {
            state.allocPointers.push_back(vv);
            lhsCnt.meta = VAR_POINTER;
        }
        //Отметить new.
        ctx.allocated.push_back(vv);

        std::shared_ptr<VariableSMT> vvForm(new VariableSMT(vv));
        state.formulae.push_back(vvForm);

        ctx.fsm.AddStateToLeaves(state, ctx.fairPred);
    }
}

VarAssigmentFromPointerStatement::VarAssigmentFromPointerStatement( const std::string &varName,
                                                                            const std::string &rhsName,
                                                                            const std::string &loc )
        : varName(varName), rhsName(rhsName), loc(loc)
{
}

void VarAssigmentFromPointerStatement::process( ProcessCtx &ctx )
{
    PtrCounter &lhsCnt = ctx.variables[ varName ];
    int lhsVer = ++(lhsCnt.count);

    StateFSM state;
    VersionedVariable lhsVar(varName, loc, VAR_POINTER, lhsVer);
    //VariableSMT *lhsForm = new VariableSMT();
    std::shared_ptr<VariableSMT> lhsForm(new VariableSMT(lhsVar));
    state.formulae.push_back(lhsForm);

    PtrCounter &rhsCnt = ctx.variables[ rhsName ];
    int rhsVer = rhsCnt.count;
    VersionedVariable rhsVar(rhsName, loc, VAR_POINTER, rhsVer);
    //VariableSMT *rhsForm = new VariableSMT();
    std::shared_ptr<VariableSMT> rhsForm(new VariableSMT(rhsVar));
    state.formulae.push_back(rhsForm);
    lhsCnt.meta = rhsCnt.meta;

    //shared_ptr<VariableSMT> leftForm(new VariableSMT);
    //leftForm->Var(lhsVar);
    //state.formulae.push_back(leftForm);
    std::shared_ptr<FormulaSMT> bs(new BinarySMT(lhsVar, rhsVar, EqualSMT, false));
    state.formulae.push_back(bs);

    ctx.fsm.AddStateToLeaves(state, ctx.fairPred);
}

VarAssigmentNewStatement::VarAssigmentNewStatement( const std::string &varName, bool isArray,
                                                            const std::string &loc )
        : varName(varName), isArray(isArray), loc(loc)
{
}

void VarAssigmentNewStatement::process( ProcessCtx &ctx )
{
    PtrCounter &lhsCnt = ctx.variables[ varName ];
    int lhsVer = ++(lhsCnt.count);

    StateFSM state;
    VersionedVariable vv(varName, loc, VAR_POINTER, lhsCnt.count);
    if( isArray )
    {
        vv.MetaType(VAR_ARRAY_POINTER);
        state.allocArrays.push_back(vv);
        lhsCnt.meta = VAR_ARRAY_POINTER;
    }
    else
    {
        state.allocPointers.push_back(vv);
        lhsCnt.meta = VAR_POINTER;
    }
    //Отметить new.
    ctx.allocated.push_back(vv);

    std::shared_ptr<VariableSMT> vvForm(new VariableSMT(vv));
    state.formulae.push_back(vvForm);

    ctx.fsm.AddStateToLeaves(state, ctx.fairPred);
}

ReturnStatement::ReturnStatement( const std::string &returnVarName )
        : returnVarName(returnVarName)
{
    static int a = 123;
    a++;
}

void ReturnStatement::process( ProcessCtx &ctx )
{
    if( !returnVarName.empty())
    {
        std::set<std::string> tmp;
        tmp.insert(returnVarName);
        ctx.fsm.SetReturnVarName(tmp);
    }

    ctx.fsm.ProcessReturnNone();

    bool isAllocFoo = ctx.fsm.IsAllocReturns();
    if( isAllocFoo )
    {
        allocatedFunctions.insert(ctx.fsm.FunctionName());
    }

    ctx.fsm.ClearReturnVarName();
}
}