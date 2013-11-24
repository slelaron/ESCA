#include <algorithm>
#include <functional>
#include <stdexcept>
#include <sstream>

#include "FSM.h"

using namespace std;

FSM::FSM()
{
	StateFSM start;
	start.id = 0;
	states.push_back(start);
}

bool FSM::GetState(FSMID id, StateFSM &s) 
{
	/*
	StatesIter iter = states.find(id);
	if (iter != states.end())
	{
		s = iter->second;
		return true;
	}
	*/
	int ss = states.size();
	if (id < ss && id >= 0)
	{
		if (states[id].id == id)
		{
			s = states[id];
			return true;
		}
		else
		{
			stringstream sstr;
			sstr << "id == " << id << " and index in vector states must be the same";
			throw logic_error(sstr.str());
		}
	}
	return false;
}

//TODO: rewrite these functions properly.

int FSM::StateToLeaf(int leafId, const StateFSM &newState)
{
			StateFSM s = newState;
			TransitionFSM t;
			t.start = leafId;
			s.id = GetNewStateID();
			t.end = s.id;
			t.id = GetNewTransitionID();
			t.evt = TransitionFSM::EPSILON;
			states[leafId].outgoing.push_back(t.id);
			states.push_back(s);
			transitions.push_back(t);
			//states.insert(pair<FSMID, StateFSM>(s.id, s));
			//transitions.insert(pair<FSMID, TransitionFSM>(t.id, t));
			return 0;
}

void FSM::AddStateToLeaves(StateFSM s)
{
	int size = states.size();
	for (int i = 0; i < size; ++i)
	{
		if (states[i].outgoing.empty())
		{
			StateToLeaf(i, s);
		}
	}
	/*
	for (StatesIter iter = states.begin(); iter != states.end(); ++iter)
	{
		if (iter->outgoing.empty()) //leaf
		{
			TransitionFSM t;
			t.start = iter->id;
			s.id = GetNewStateID();
			t.end = s.id;
			t.id = GetNewTransitionID();
			t.evt = TransitionFSM::EPSILON;
			states.insert(pair<FSMID, StateFSM>(s.id, s));
			transitions.insert(pair<FSMID, TransitionFSM>(t.id, t));
		}
	}
	*/
}

/*
void FSM::AddBranchToLeaves(StateFSM s)
{
	for (StatesIter iter = states.begin(); iter != states.end(); ++iter)
	{
		if (iter->second.outgoing.empty()) //leaf
		{
			TransitionFSM t;
			t.start = iter->second.id;
			s.id = GetNewStateID();
			t.end = s.id;
			t.id = GetNewTransitionID();
			t.evt = TransitionFSM::EPSILON;
			states.insert(pair<FSMID, StateFSM>(s.id, s));
			transitions.insert(pair<FSMID, TransitionFSM>(t.id, t));
		}
	}
}
*/
void FSM::AddFormulaSMT(FormulaSMT *f)
{
	int size = states.size();
	for (int i = 0; i < size; ++i)
	{
		if (states[i].outgoing.empty())
		{
			states[i].formulae.push_back(f);
		}
	}
}

void FSM::AddAllocPointer(VersionedVariable ap)
{
	int size = states.size();
	for (int i = 0; i < size; ++i)
	{
		if (states[i].outgoing.empty())
		{
			states[i].allocPointers.push_back(ap);
		}
	}
}
