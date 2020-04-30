#include "../FSM/FSM.h"

namespace Target
{
struct ProcessCtx
{
    std::map<std::string, PtrCounter> variables;

    /// @brief вектор аллоцированных переменных
    std::vector<VersionedVariable> allocated;

    FSM fsm;

    FairLeafPredicate fairPred;
    BranchLeafPredicate branchPred;
};
}