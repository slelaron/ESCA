#include <map>
#include <set>
#include <vector>


enum STATEMENTS
{
    COMPOUND,
    DELETE,
    IF,
    VarDeclFromFoo,
    VarDeclNew,
    VarAssigmentFromFoo,
    VarAssigmentFromPointer,
    VarAssigmentNew,
    Return,
    UNKNOWN
};

namespace Target
{
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

//    void process( ProcessCtx &ctx ) override;

private:
    std::vector<Statement *> statements;
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


class VarDeclFromFooStatement : public Statement
{
public:
    VarDeclFromFooStatement( const std::string &varName, const std::string &fooName,
                             const std::string &loc );

    STATEMENTS GetType() override
    {
        return STATEMENTS::VarDeclFromFoo;
    }

    std::string varName;
    std::string fooName;
    std::string loc;
};


class VarDeclNewStatement : public Statement
{
public:
    VarDeclNewStatement( const std::string &varName, bool isArray, const std::string &loc );;

    STATEMENTS GetType() override
    {
        return STATEMENTS::VarDeclNew;
    }


    std::string varName;
    bool isArray;
    std::string loc;
};


class VarAssigmentFromFooStatement : public Statement
{
public:
    VarAssigmentFromFooStatement( const std::string &varName, const std::string &fooName, const std::string &loc );

    STATEMENTS GetType() override
    {
        return STATEMENTS::VarAssigmentFromFoo;
    }

    std::string varName;
    std::string fooName;
    std::string loc;
};


class VarAssigmentFromPointerStatement : public Statement
{
public:
    VarAssigmentFromPointerStatement( const std::string &varName, const std::string &rhsName,
                                      const std::string &loc );

    STATEMENTS GetType() override
    {
        return STATEMENTS::VarAssigmentFromPointer;
    }

//    void process( ProcessCtx &ctx ) override;

    std::string varName;
    std::string loc;
    std::string rhsName;
};


class VarAssigmentNewStatement : public Statement
{
public:
    VarAssigmentNewStatement( const std::string &varName, bool isArray, const std::string &loc );

    STATEMENTS GetType() override
    {
        return STATEMENTS::VarAssigmentNew;
    }

    std::string varName;
    bool isArray;
    std::string loc;
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
