#ifndef FSM_H
#define FSM_H

#include <vector>
#include <deque>
#include <map>
#include <memory>
/*
#ifdef _MSC_VER
#if _MSC_VER == 1600
#elif _MSC_VER == 1800
#endif
#endif
*/


#include "StateFSM.h"
#include "TransitionFSM.h"
#include "VersionedVariable.h"
#include "FormulaSMT.h"

/*
typedef std::map<FSMID, StateFSM> StatesStorage;
typedef std::map<FSMID, TransitionFSM> TransitionsStorage;
typedef std::map<FSMID, StateFSM>::iterator StatesIter;
typedef std::map<FSMID, TransitionFSM>::iterator TransitionsIter;
*/
typedef std::vector<StateFSM> StatesStorage;
typedef std::vector<TransitionFSM> TransitionsStorage;
typedef std::vector<StateFSM>::iterator StatesIter;
typedef std::vector<TransitionFSM>::iterator TransitionsIter;
typedef std::deque<ConditionEvent> ConditionStorage;

std::string PrintSMT(int iSat, const FormulaStorage &f);

class FSM
{
	public:
		FSM();
		inline void AddState(const StateFSM &s) { states[s.id] = s; }
		inline void AddTransition(const TransitionFSM &trans) { transitions[trans.id] = trans; }
		bool GetState(FSMID id, StateFSM &s);
		bool GetTransition(FSMID id, TransitionFSM &t);

		inline FSMID GetNewStateID() const { return states.size(); }
		inline FSMID GetNewTransitionID() const { return transitions.size(); }

		inline void PushCondition(const std::string &cond) { events.push_back(cond); }
		inline void PopCondition() { if (!events.empty()) events.pop_back(); }

		int StateToLeaf(int leafId, const StateFSM &s);
		void AddStateToLeaves(const StateFSM &s);
		//void AddBranchToLeaves(StateFSM s);
		void AddFormulaSMT(const std::shared_ptr<FormulaSMT> &f); //Add SMT-formula to all leaves;
		void AddAllocPointer(const VersionedVariable &ap);
		//void AddTransition(shared_ptr<StateFSM> begin, shared_ptr<StateFSM> end, shared_ptr<Tra)
		void AddDeleteState(const VersionedVariable &var, bool arrayForm);
		void ProcessReturnNone();
	private://internal functions
		void HandleDeletePtr(const VersionedVariable &v, std::vector<VersionedVariable> &alloc, 
			std::vector<VersionedVariable> &del, StateFSM &delState);
		void CreateNewRetNoneState(StateFSM &leaf);
		void SolveRet(bool isArray, const StateFSM &s);
		bool MatchEvents(FSMID stateID);
	private: //member variables
		StatesStorage states;
		TransitionsStorage transitions;
		int iSat;		
		ConditionStorage events;
};

#endif
