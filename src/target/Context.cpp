/// @author alexust27
/// @date 20.04.2020.

#include <cassert>
#include "Context.h"


namespace Target
{

Context::Context()
{
    curFunction = nullptr;
    freeFunctions.insert("free");
    freeFunctions.insert("close");
    freeFunctions.insert("fclose");
#ifdef _WIN32
    freeFunctions.insert("closesoket");
#endif
}

void Context::AddFunction( const std::string &name )
{
    curFunction = new Function(name);
    // FIXME: тут возникают коллизии если у функций одинаковые названия и они в разных файлах
    allFunctions[ name ] = curFunction;
}

void Context::AddToLast( Statement *s )
{
    if( isInIf )
    {
        assert(!ifStatementsStack.empty());
        if( onThen )
        {
            ifStatementsStack.back().second->thenSt->addState(s);
        }
        else
        {
            ifStatementsStack.back().second->elseSt->addState(s);
        }
        return;
    }
    assert(!compoundStatementsStack.empty());
    compoundStatementsStack.back()->addState(s);
}

CompoundStatement *Context::createCompoundStatement( bool addToStates )
{
    auto s = new CompoundStatement();
    if( compoundStatementsStack.empty())
    {
        // начало функции
        curFunction->MakeStart(s);
    }

    if( !compoundStatementsStack.empty() && addToStates )
    {
        AddToLast(s);
    }
    compoundStatementsStack.push_back(s);
    return s;
}


void Context::popCompound()
{
    //lastPoped = stackSt.back();
    compoundStatementsStack.pop_back();
}

Context &Context::Instance()
{
    static Context instance;
    return instance;
}

std::map<std::string, Target::Function *> *Context::getAllFunction()
{
    return &allFunctions;
}

void Context::AddFreeFunction( const std::string &function )
{
    freeFunctions.insert(function);
}

bool Context::IsFreeFunction( const std::string &function )
{
    return freeFunctions.count(function);
}

IfStatement *Context::CreateIfStatement( bool hasElse, const std::string &cond, const std::string &elseCond )
{
    auto thenSt = new CompoundStatement();
    auto elseSt = hasElse ? new CompoundStatement() : nullptr;
    auto ifSt = new IfStatement(thenSt, elseSt, cond, elseCond);
    AddToLast(ifSt);
    isInIf = true;
    onThen = true;
    ifStatementsStack.emplace_back(onThen, ifSt);
    return ifSt;
}

void Context::SwitchToElse()
{
    if( !onThen || ifStatementsStack.back().second->elseSt == nullptr )
    {
        throw std::logic_error("Else is empty, or you need go on then first");
    }
    onThen = false;
    ifStatementsStack.back().first = onThen;
}

void Context::PopIfStatement()
{
    assert(!ifStatementsStack.empty());
    ifStatementsStack.pop_back();
    if( ifStatementsStack.empty())
    {
        isInIf = false;
    }
    else
    {
        onThen = ifStatementsStack.back().first;
    }
}



//Function *Context::getLastFunction() const
//{
//    return curFunction;
//}

//Statement* getLastPoped() {
//    auto tmp = lastPoped;
//    lastPoped = nullptr;
//    return tmp;
//}

//    кладет с вершины стэка в общую схему
//    void addCompoundStatement()
//    {
//        //if (stackSt.size() > 1) {
//        //    stackSt[stackSt.size() - 2]->addState(stackSt.back());
//        //}
//        //AddToLast(stackSt.back());
//    }

//
//

}
