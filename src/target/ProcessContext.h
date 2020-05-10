#include "../FSM/FSM.h"

namespace Target
{
class ProcessContext
{
public:
    explicit ProcessContext( const std::string &funName )
            : fsm(std::make_unique<FSM>(funName))
    {
    }

    std::map<std::string, PtrCounter> variables;

    /// @brief вектор аллоцированных переменных
    std::vector<VersionedVariable> allocated;

    std::unique_ptr<FSM> fsm;

    FairLeafPredicate fairPred;
    BranchLeafPredicate branchPred;
};
}