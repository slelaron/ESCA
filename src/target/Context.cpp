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

void Context::addToLast( Statement *s )
{
    //if (isIf) {
    //    isIf = false;
    //    nextInIf = s;
    //}
    assert(!compoundStatementsStack.empty());
    compoundStatementsStack.back()->addState(s);
}

void Context::createCompoundStatement( bool addToStates )
{
    auto s = new CompoundStatement();
    if( compoundStatementsStack.empty())
    {
        // начало функции
        curFunction->makeStart(s);
    }

    if( !compoundStatementsStack.empty() && addToStates )
    {
        addToLast(s);
    }
    compoundStatementsStack.push_back(s);
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
//        //addToLast(stackSt.back());
//    }

//    Statement **nextInIf = nullptr;
//    bool isIf = false;
//
//    void setIf( Statement *s )
//    {
//
//        if( nextInIf )
//        {
//            *nextInIf = s;
//            nextInIf = nullptr;
//        }
//    }
//
//    void startIfSt( Statement **s )
//    {
//        nextInIf = s;
//        isIf = true;
//    }

//Statement* getAfterIfStatement() {
//    auto tmp = nextInIf;
//    nextInIf = nullptr;
//    isIf = false;
//    return tmp;
//}

}
