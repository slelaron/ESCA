#ifndef STATEFSM_H
#define STATEFSM_H

#include <vector>
#include <deque>
#include <memory>
#include <string>

#include "TypesFSM.h"
#include "VersionedVariable.h"
#include "FormulaSMT.h"


//class TransitionFSM;

class StateFSM
{
	public:
		~StateFSM();
		FSMID id;
		std::vector<FSMID> incoming;
		std::vector<FSMID> outgoing;

		std::vector<VersionedVariable> allocPointers;
		std::vector<VersionedVariable> allocArrays;
		std::vector<VersionedVariable> delPointers;
		std::vector<VersionedVariable> delArrays;
		std::deque<std::shared_ptr<FormulaSMT> > formulae;

	public:
		std::string PrintFormulae();
		std::string PrintFormulaeSat();
};

bool operator ==(const StateFSM &lhs, const StateFSM &rhs);
bool operator <(const StateFSM &lhs, const StateFSM &rhs);
//typedef shared_ptr<StateFSM> StateFSMPtr;

#endif
