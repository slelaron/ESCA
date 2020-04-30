#include "Function.h"

namespace Target
{

/// @brief Структура в которой сохраняются информация о состояниях
class Context
{
public:
    Context();

    Function *lastFoo;

    void addFunction( const std::string &name );

    Statement *
    addDeleteStatement( const std::string &name, bool isArray );

    Statement *
    addIfStatement( Statement *thenSt, Statement *elseSt, const std::string &condStr, const std::string &elseStr );

    Statement *
    createCompoundStatement( bool addToStates = true );

    Statement *
    addVarDeclFromFoo( const std::string &varName, const std::string &fooName, const std::string &loc );

    Statement *
    addVarDeclNew( const std::string &varName, bool isArray, const std::string &loc );

    Statement *
    addVarAssigmentFromFoo( const std::string &varName, const std::string &fooName, const std::string &loc );

    Statement *
    addVarAssigmentFromPointer( const std::string &varName, const std::string &rhsName, const std::string &loc );

    Statement *
    addVarAssigmentNew( const std::string &varName, bool isArray, const std::string &loc );

    Statement *
    addReturn( const std::string &returnVarName );

    void popCompound();

    //кладет с вершины стэка в общую схему
    // FIXME: видимо больше не кладет
//    void addCompoundStatement()
//    {
//        //if (stackSt.size() > 1) {
//        //    stackSt[stackSt.size() - 2]->addState(stackSt.back());
//        //}
//        //addToLast(stackSt.back());
//    }

private:
    Statement *addToLast( Statement *s );

    std::vector<Function *> extFunctions;

    std::vector<CompoundStatement *> stackSt;


    //Statement* getLastPoped() {
    //    auto tmp = lastPoped;
    //    lastPoped = nullptr;
    //    return tmp;
    //}


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

};
}
