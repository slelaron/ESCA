#include "Statements.h"

namespace Target
{
class Function
{
public:
    Function() = delete;

    explicit Function( const std::string &name ) : name(name), statement(nullptr)
    {
    }

    /// @brief Создает начальное состояние функции
    void makeStart( CompoundStatement *startState )
    {
        if( statement )
        {
            throw std::runtime_error("Function already has start statement");
        }
        statement = startState;
    }

    /// @brief Возвращает начальное состояние функции
    CompoundStatement *const startState() const
    {
        if( !statement )
        {
            throw std::runtime_error("Function hasn't had start statement yet");
        }
        return statement;
    }

    /// @brief Имена функций которые вызываются внутри функции
    std::vector<std::string> callee;

    std::set<std::string> returnName;

    /// @brief Возвращает имя функции
    std::string getName() const
    {
        return name;
    }

private:
    /// @brief Начальное состояние в функции
    CompoundStatement *statement;

    std::string name;

};

}
