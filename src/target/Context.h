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
    CompoundStatement *createCompoundStatement( bool addToStates = true );

    /// @brief Метод удаляет последнее составное состояние со стека
    void popCompound();

    /// @brief Метод добавляет к последнему составному состоянию в стеке вложеное состояние
    void AddToLast( Statement *s );

    /// @brief Метод добавляет к контексту имя функции которая может освобождать ресурсы
    void AddFreeFunction( const std::string &function );

    /// @brief Метод проверяет может ли функция освобождать ресурсы
    bool IsFreeFunction( const std::string &function );

    /// @brief Метод возвращает указатель на мап со всеми функциями
    std::map<std::string, Target::Function *> *getAllFunction();

    IfStatement *CreateIfStatement( bool hasElse, const std::string &cond, const std::string &elseCond );

    void PopIfStatement();

    void SwitchToElse();

private:
    Context();

    bool onThen = true;
    bool isInIf = false;

    /// @brief Стек с условными состояниями (onThen, ifStatement)
    std::vector<std::pair<bool, IfStatement *>> ifStatementsStack;

    /// @brief Стек с составными состояниями
    std::vector<CompoundStatement *> compoundStatementsStack;

    /// @brief Все фукнции, сохраненные для анализа
    std::map<std::string, Target::Function *> allFunctions;

    /// @brief Фукнции, которые могут освобождать ресурсы
    std::set<std::string> freeFunctions;

};
}
