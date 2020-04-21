/// @author alexust27
/// @date 20.04.2020.

#include <cassert>
#include "Context.h"

/// @brief множество функции которые возвращают указатель на выделенную память
std::set<std::string> allocatedFunctions;


namespace Target
{

Context::Context()
{
    lastFoo = nullptr;
    allocatedFunctions.insert(std::string("malloc"));

    allocatedFunctions.insert(std::string("socket"));
    allocatedFunctions.insert(std::string("accept"));

    allocatedFunctions.insert(std::string("fopen"));
}

void Context::addFunction( const std::string &name )
{
    extFunctions.push_back(new Function(name));
    lastFoo = extFunctions.back();
}

Statement *Context::addDeleteStatement( const std::string &name, bool isArray )
{
    return addToLast(new DeleteStatement(name, isArray));
}

Statement *
Context::addIfStatement( Statement *thenSt, Statement *elseSt, const std::string &condStr,
                         const std::string &elseStr )
{
    return addToLast(new IfStatement(thenSt, elseSt, condStr, elseStr));
}

//    просто создаем набор стейтов и добавляем его на вершину стэка,
// не пуша никуда, для пуша в контекст есть отдельный метод
Statement *Context::createCompoundStatement( bool addToStates )
{
    //addToStates = true;
    auto s = new CompoundStatement();
    // начало функции
    if( stackSt.empty())
    {
        lastFoo->statement = s;
    }
    //lastPoped = nullptr;
    if( !stackSt.empty() && addToStates )
    {
        addToLast(s);
    }
    stackSt.push_back(s);
    return s;
}

Statement *Context::addVarDeclFromFoo( const std::string &varName, const std::string &fooName, const std::string &loc )
{
    return addToLast(new VarDeclFromFooStatement(varName, fooName, loc));
}

Statement *Context::addVarDeclNew( const std::string &varName, bool isArray, const std::string &loc )
{
    return addToLast(new VarDeclNewStatement(varName, isArray, loc));
}

Statement *
Context::addVarAssigmentFromFoo( const std::string &varName, const std::string &fooName, const std::string &loc )
{
    return addToLast(new VarAssigmentFromFooStatement(varName, fooName, loc));
}

Statement *
Context::addVarAssigmentFromPointer( const std::string &varName, const std::string &rhsName, const std::string &loc )
{
    return addToLast(new VarAssigmentFromPointerStatement(varName, rhsName, loc));
}

Statement *Context::addVarAssigmentNew( const std::string &varName, bool isArray, const std::string &loc )
{
    return addToLast(new VarAssigmentNewStatement(varName, isArray, loc));
}

Statement *Context::addReturn( const std::string &returnVarName )
{
    return addToLast(new ReturnStatement(returnVarName));
}

void Context::popCompound()
{
    //lastPoped = stackSt.back();
    stackSt.pop_back();
}

Statement *Context::addToLast( Statement *s )
{
    //if (isIf) {
    //    isIf = false;
    //    nextInIf = s;
    //}
    assert(!stackSt.empty());
    stackSt.back()->addState(s);
    return s;
}

}
