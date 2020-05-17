#include "Statements.h"

namespace Target
{
class Function
{
public:
    Function() = delete;

    explicit Function( const std::string &name ) : name(name), statement(nullptr), isThrowable(false)
    {
    }

    /// @brief Создает начальное состояние функции
    void MakeStart( CompoundStatement *startState )
    {
        if( statement )
        {
            throw std::logic_error("Function already has start statement");
        }
        statement = startState;
    }

    /// @brief Возвращает начальное состояние функции
    CompoundStatement *const StartState() const
    {
        if( !statement )
        {
            throw std::logic_error("Function hasn't had start statement yet");
        }
        return statement;
    }

    bool isThrowable;

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
