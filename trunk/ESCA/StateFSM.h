#ifndef STATEFSM_H
#define STATEFSM_H

#include <vector>
#include <deque>

#include "TypesFSM.h"
#include "VersionedVariable.h"
#include "FormulaSMT.h"


//class TransitionFSM;

class StateFSM
{
	public:
		~StateFSM();
	//private:
		FSMID id;
		//std::vector<TransitionFSM *> incoming;
		//std::vector<TransitionFSM *> outgoing;
		std::vector<FSMID> incoming;
		std::vector<FSMID> outgoing;

		std::vector<VersionedVariable> allocPointers;
		std::vector<VersionedVariable> allocArrays;
		std::vector<VersionedVariable> delPointers;
		std::vector<VersionedVariable> delArrays;
		std::deque<FormulaSMT *> formulae;
};

bool operator ==(const StateFSM &lhs, const StateFSM &rhs);
bool operator <(const StateFSM &lhs, const StateFSM &rhs);
//typedef shared_ptr<StateFSM> StateFSMPtr;

#endif
