#include "Statements.h"

namespace Target
{
class Function;
}

extern std::map<std::string, Target::Function *> allFunctions;

extern std::set<std::string> processedFunctions;

namespace Target
{
class Function
{
public:
    Function() = delete;

    explicit Function( const std::string &name ) : name(name)
    {
    }

    /// @brief состояние которое хранит все состояния функции
    CompoundStatement *statement = nullptr;

    /// @brief имена функций которые вызываются внутри функции
    std::vector<std::string> callee;

    std::set<std::string> returnName;

    /// @brief запуск анализа функции
    void process()
    {
        // already proccessed
        if( processedFunctions.find(name) != processedFunctions.end())
        {
            return;
        }

        // process all callee
        for( const auto &c : callee )
        {
            if( allFunctions.find(c) == allFunctions.end())
            {
                // нет такой функции среди известных нам
                continue;
            }

            // prevent recursion
            if( name != allFunctions[ c ]->name )
            {
                allFunctions[ c ]->process();
            }
        }

        // process
        ProcessCtx ctx;
        ctx.fsm.FunctionName(name);

//        static int x = 123;
//        if( name == "SCR_ScreenShot_f_1" )
//        {
//            ++x;
//        }

        statement->process(ctx);

        if( !returnName.empty())
        {
            ctx.fsm.SetReturnVarName(returnName);
        }
        ctx.fsm.ProcessReturnNone();

        processedFunctions.insert(name);
    }

private:
    std::string name;

};

}
