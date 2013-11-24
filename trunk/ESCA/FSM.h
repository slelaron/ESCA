#ifndef FSM_H
#define FSM_H

#include <vector>
#include <map>
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

class FSM
{
	public:
		FSM();
		inline void AddState(StateFSM s) { states[s.id] = s; }
		inline void AddTransition(TransitionFSM trans) { transitions[trans.id] = trans; }
		bool GetState(FSMID id, StateFSM &s);
		bool GetTransition(FSMID id, TransitionFSM &t);

		inline FSMID GetNewStateID() const { return states.size(); }
		inline FSMID GetNewTransitionID() const { return transitions.size(); }

		int StateToLeaf(int leafId, const StateFSM &s);
		void AddStateToLeaves(StateFSM s);
		//void AddBranchToLeaves(StateFSM s);
		void AddFormulaSMT(FormulaSMT *f); //Add SMT-formula to all leaves;
		void AddAllocPointer(VersionedVariable ap);
		//void AddTransition(shared_ptr<StateFSM> begin, shared_ptr<StateFSM> end, shared_ptr<Tra)
	private:
		StatesStorage states;
		TransitionsStorage transitions;
};

#endif
