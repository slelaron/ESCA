
#include "Statements.h"

namespace Target
{

void CompoundStatement::addState( Statement *st )
{
    statements.push_back(st);
}

const std::vector<Statement *> &CompoundStatement::getStates() const
{
    return statements;
}


DeleteStatement::DeleteStatement( const std::string &name, bool isArray )
        : name(name), isArray(isArray)
{
}


IfStatement::IfStatement( Statement *thenSt, Statement *elseSt, const std::string &condStr,
                          const std::string &elseStr )
        : thenSt(thenSt), elseSt(elseSt), condStr(condStr), elseStr(elseStr)
{
    static int a = 0;
    a++;
}


VarDeclFromFooStatement::VarDeclFromFooStatement( const std::string &varName, const std::string &fooName,
                                                  const std::string &loc )
        : varName(varName), fooName(fooName), loc(loc)
{
}

VarDeclNewStatement::VarDeclNewStatement( const std::string &varName, bool isArray, const std::string &loc )
        : varName(varName), isArray(isArray), loc(loc)
{
}


VarAssigmentFromFooStatement::VarAssigmentFromFooStatement( const std::string &varName,
                                                            const std::string &fooName, const std::string &loc )
        : varName(varName), fooName(fooName), loc(loc)
{
}


VarAssigmentFromPointerStatement::VarAssigmentFromPointerStatement( const std::string &varName,
                                                                    const std::string &rhsName,
                                                                    const std::string &loc )
        : varName(varName), rhsName(rhsName), loc(loc)
{
}


VarAssigmentNewStatement::VarAssigmentNewStatement( const std::string &varName, bool isArray,
                                                    const std::string &loc )
        : varName(varName), isArray(isArray), loc(loc)
{
}

ReturnStatement::ReturnStatement( const std::string &returnVarName )
        : returnVarName(returnVarName)
{
    static int a = 123;
    a++;
}

}