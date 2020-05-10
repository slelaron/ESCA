#include "Function.h"

namespace Target
{

/// @brief Синглтон структура в которой сохраняется информация о всех состояниях анализируемой программы
class Context
{
public:
    Context( Context const & ) = delete;

    Context &operator=( Context const & ) = delete;

    /// @brief Возвращает единственный экземпляр контекста
    static Context &Instance();

    /// @brief Текущая анализируемая функция
    Function *curFunction;

    /// @brief Добавляем новую анализируемую функцию
    void AddFunction( const std::string &name );

    /// @brief Метод создает составное состояние и добавляет его на вершину стэка
    void createCompoundStatement( bool addToStates = true );

    /// @brief Метод удаляет последнее составное состояние со стека
    void popCompound();

    /// @brief Метод добавляет к последнему составному состоянию в стеке вложеное состояние
    void addToLast( Statement *s );

    std::map<std::string, Target::Function *> *getAllFunction();

private:

    Context();

    /// @brief Стек с составными состояниями
    std::vector<CompoundStatement *> compoundStatementsStack;

    /// @brief все фукнции которые будут проанализированы
    std::map<std::string, Target::Function *> allFunctions;

};
}
