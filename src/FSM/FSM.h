#ifndef FSM_H
#define FSM_H

#include <set>

#include "TransitionFSM.h"
#include "LeafPredicate.h"

typedef std::vector<StateFSM> StatesStorage;
typedef std::vector<TransitionFSM> TransitionsStorage;
typedef std::vector<StateFSM>::iterator StatesIter;
typedef std::vector<TransitionFSM>::iterator TransitionsIter;
typedef std::deque<ConditionEvent> ConditionStorage;

//std::string PrintSMT( int iSat, const FormulaStorage &f );

class FSM
{
public:
    explicit FSM( const std::string &functionName );

    /// @brief Сброс состояний автомата до одного стартового
    ~FSM();

    std::string FunctionName() const
    {
        return functionName;
    }

    inline void AddState( const StateFSM &s )
    {
        states[ s.id ] = s;
    }

    inline void AddTransition( const TransitionFSM &trans )
    {
        transitions[ trans.id ] = trans;
    }

    bool GetState( FSMID id, StateFSM &s );

//    bool GetTransition( FSMID id, TransitionFSM &t );

    inline FSMID GetNewStateID() const
    {
        return states.size();
    }

    inline FSMID GetNewTransitionID() const
    {
        return transitions.size();
    }

    inline void PushCondition( const std::string &cond )
    {
        events.push_back(cond);
    }

    inline void PopCondition()
    {
        if( !events.empty())
            events.pop_back();
    }

    int StateToLeaf( int leafId, const StateFSM &newState );

    int StateToLeaf( int leafId, const StateFSM &newState, const std::string &pred );

    void AddStateToLeaves( const StateFSM &s, LeafPredicate &pred );

    void AddStateToLeaves( const StateFSM &s, LeafPredicate &pred, const std::string &cond, bool finCond );

    //void AddBranchToLeaves(StateFSM s);
    void AddFormulaSMT( const std::shared_ptr<FormulaSMT> &f ); //Add SMT-formula to all leaves;
    void AddAllocPointer( const VersionedVariable &ap );

    //void AddTransition(shared_ptr<StateFSM> begin, shared_ptr<StateFSM> end, shared_ptr<Tra)
    void AddDeleteState( const VersionedVariable &var, bool arrayForm );

    void ProcessReturnNone();

    void SetReturnVarName( const std::set<std::string> &varName )
    {
        returnVarName = varName;
        isAllocReturns = false;
    }

    void ClearReturnVarName()
    {
        returnVarName.clear();
        isAllocReturns = false;
    }

    /// @brief Возвращает ли функция указатель на выделенную память
    /// @return true - возвращает, иначе - false
    bool IsAllocReturns()
    {
        return isAllocReturns;
    }

#ifdef SAVE_XML

    void SaveToXML();

    void SaveToXML( const std::string &path );

#endif

private:
    std::set<std::string> returnVarName;

    /// @brief флаг того что функция возвращает указатель на выделенную память
    bool isAllocReturns = false;

//internal functions
    void CreateStart();

    void HandleDeletePtr( const VersionedVariable &v, std::vector<VersionedVariable> &alloc,
                          std::vector<VersionedVariable> &del, StateFSM &delState ) const;

    void CreateNewRetNoneState( StateFSM &leaf );

    void SolveRet( bool isArray, const StateFSM &s );

    bool MatchEvents( FSMID stateID );

//member variables
    std::string functionName;

    /// @brief Все состояния автомата
    StatesStorage states;

    TransitionsStorage transitions;
    int iSat;

    ConditionStorage events;
};

#endif
