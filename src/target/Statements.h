/// @file Statements.h
///
/// @brief Класс всех состояний анализируемого кода
///
/// @author alexust27
/// Contact: ustinov1998s@gmail.com
///

#include <map>
#include <set>
#include <vector>


enum STATEMENTS
{
    COMPOUND,
    VarAssigmentNew,
    VarAssigmentFromFoo,
    VarAssigmentFromPointer,
    DELETE,
    IF,
    Return,
    UNKNOWN
};

namespace Target
{
/// @brief Общий класс для всех состояний анализируемого кода
class Statement
{
public:

    virtual STATEMENTS GetType()
    {
        return STATEMENTS::UNKNOWN;
    }

    virtual ~Statement() = default;
};

/// @brief Состовное состояние, содержит несколько состояний
class CompoundStatement : public Statement
{
public:
    CompoundStatement() = default;

    void addState( Statement *st );

    const std::vector<Statement *> &getStates() const;

    STATEMENTS GetType() override
    {
        return STATEMENTS::COMPOUND;
    }

private:
    std::vector<Statement *> statements;
};


class VarAssigmentFromFooStatement : public Statement
{
public:
    VarAssigmentFromFooStatement( const std::string &varName, const std::string &fooName, const std::string &loc,
                                  bool isDecl );

    STATEMENTS GetType() override
    {
        return STATEMENTS::VarAssigmentFromFoo;
    }

    bool isDecl;
    std::string varName;
    std::string fooName;
    std::string loc;
};


class VarAssigmentFromPointerStatement : public Statement
{
public:
    VarAssigmentFromPointerStatement( const std::string &varName, const std::string &rhsName,
                                      const std::string &loc, bool isDecl );

    STATEMENTS GetType() override
    {
        return STATEMENTS::VarAssigmentFromPointer;
    }

    std::string varName;
    std::string loc;
    std::string rhsName;
    bool isDecl;
};

/// auto x = new X;
class VarAssigmentNewStatement : public Statement
{
public:
    VarAssigmentNewStatement( const std::string &varName, bool isArray, const std::string &loc, bool isDecl );

    STATEMENTS GetType() override
    {
        return STATEMENTS::VarAssigmentNew;
    }

    std::string varName;
    std::string loc;
    bool isArray;
    bool isDecl;
};


class DeleteStatement : public Statement
{
public:
    DeleteStatement( const std::string &name, bool isArray );

    STATEMENTS GetType() override
    {
        return STATEMENTS::DELETE;
    }

    std::string name;
    bool isArray;
};


class IfStatement : public Statement
{
public:
    IfStatement( Target::Statement *thenSt, Target::Statement *elseSt, const std::string &condStr,
                 const std::string &elseStr );

    STATEMENTS GetType() override
    {
        return STATEMENTS::IF;
    }

    Statement *thenSt = nullptr;
    Statement *elseSt = nullptr;
    std::string condStr;
    std::string elseStr;
};


class ReturnStatement : public Statement
{
public:
    explicit ReturnStatement( const std::string &returnVarName );

    STATEMENTS GetType() override
    {
        return STATEMENTS::Return;
    }

    std::string returnVarName;
};

} // target
