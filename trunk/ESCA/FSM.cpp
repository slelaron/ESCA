#include <algorithm>
#include <functional>
#include <stdexcept>
#include <sstream>
#include <memory>
#include <fstream>

#include <llvm/Support/raw_ostream.h>

#include "FSM.h"
#include "BinarySMT.h"
#include "ExecSolver.h"
#include "DefectStorage.h"

using namespace std;

std::string PrintSMT(int iSat, const FormulaStorage &f)
{
	stringstream ss;
	ss << "form" << iSat << ".sat";
	ofstream outf(ss.str());
	outf << FormulaeToStringSat(f);
	outf.close();
	return ss.str();
}

FSM::FSM() : iSat(0)
{
	StateFSM start;
	start.id = 0;
	states.push_back(start);
}

bool FSM::GetState(FSMID id, StateFSM &s) 
{
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

template<class T>
bool MoveVector(vector<T> &source, vector<T> &dst)
{
	vector<T> tmp;
	tmp.swap(source);
	tmp.insert(tmp.end(), dst.begin(), dst.end());
	dst.swap(tmp);
	return true;
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
	StateFSM &leaf = states[leafId];
	leaf.outgoing.push_back(t.id);
	//Move all the formulae from old leaf to new leaf.
	s.formulae.insert(s.formulae.begin(), leaf.formulae.begin(), leaf.formulae.end());
	leaf.formulae.clear();

	MoveVector(leaf.allocArrays, s.allocArrays);
	MoveVector(leaf.allocPointers, s.allocPointers);
	MoveVector(leaf.delArrays, s.delArrays);
	MoveVector(leaf.delPointers, s.delPointers);

	states.push_back(s);
	transitions.push_back(t);
	//states.insert(pair<FSMID, StateFSM>(s.id, s));
	//transitions.insert(pair<FSMID, TransitionFSM>(t.id, t));
	return s.id;
}

void FSM::AddStateToLeaves(const StateFSM &s)
{
	int size = states.size();
	for (int i = 0; i < size; ++i)
	{
		if (states[i].IsLeaf())
		{
			if (MatchEvents(i)) //This is an expensive check, so we want perform it when other conditions are checked.
			{
				StateToLeaf(i, s);
			}
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
void FSM::AddFormulaSMT(const std::shared_ptr<FormulaSMT> &f)
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

void FSM::AddAllocPointer(const VersionedVariable &ap)
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

void FSM::HandleDeletePtr(const VersionedVariable &v, std::vector<VersionedVariable> &alloc, 
			std::vector<VersionedVariable> &del, StateFSM &delState)
{
	//write formulae.
	int size = alloc.size();
	//unsat => delete is correct
	FormulaStorage f = delState.formulae;
	for (int i = 0; i < size; ++i)
	{
		shared_ptr<BinarySMT> form(new BinarySMT(v, alloc[i], EqualSMT, true));
		f.push_back(form);
	}

	auto fileName = PrintSMT(iSat, f);

	auto solve = ExecSolver::Run(fileName);

	if (solve.find("unsat") != -1) //unsat
	{
		llvm::errs() << "Correct delete\n";
		del.push_back(v);
	}
}

void FSM::AddDeleteState(const VersionedVariable &var, bool arrayForm)
{
	int size = states.size();
	for (int i = 0; i < size; ++i)
	{
		if (states[i].outgoing.empty())
		{
			StateFSM sdel;
			
			int sdelId = StateToLeaf(i, sdel);

			StateFSM &del = states[sdelId];
			VersionedVariable v(var);
			switch (var.MetaType())
			{
			case VAR_POINTER:
				HandleDeletePtr(v, del.allocPointers, del.delPointers, del);
				break;
			case VAR_ARRAY_POINTER:
				HandleDeletePtr(v, del.allocArrays, del.delArrays, del);
				break;
			default:
				break;
			}
			++iSat;
		}
	}
}

void FSM::ProcessReturnNone()
{
	//For each leaf
	auto size = states.size();
	for (int i = 0; i < size; ++i)
	{
		StateFSM &s = states[i];
		if (s.IsLeaf())
		{
			//Create a new end state
			CreateNewRetNoneState(s);
		}
	}
}

void FSM::SolveRet(bool isArray, const StateFSM &s) 
{
	VarStorage alloc, del;
	//FormulaStorage f = s.formulae; 
	if (isArray)
	{
		alloc = s.allocArrays;
		del = s.delArrays;
	}
	else
	{
		alloc = s.allocPointers;
		del = s.delPointers;
	}
	//for each allocated variable
	auto sAlloc = alloc.size();
	auto sDel = del.size();
	for (int i = 0; i < sAlloc; ++i)
	{
		//check whether this variable is del list.
		FormulaStorage f = s.formulae; //TODO: do this more effective.
		for (int j = 0; j < sDel; ++j)
		{
			//write formulae.
			shared_ptr<BinarySMT> bs(new BinarySMT(alloc[i], del[j], EqualSMT, true)); //unsat => in del list.
			f.push_back(bs);
		}
		auto fileName = PrintSMT(iSat++, f);
		//if this variable is not in del list then there is a memory leak.
		//auto fileName = PrintSMT(iSat, s.formulae);

		auto solve = ExecSolver::Run(fileName);

		if (solve.find("unsat") != -1) //unsat
		{
			//No leak
		}
		else if (solve.find("sat") != -1)
		{
			//Leak
			llvm::errs() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~LEAK~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
			llvm::errs() << "variable name: " << alloc[i].Name() << "\n";
			
			stringstream defect;
			defect << "Memory leak. Variable name: " << alloc[i].Name();
			DefectStorage::Instance().AddDefect(defect.str());
		}
	}

}

void FSM::CreateNewRetNoneState(StateFSM &leaf)
{
	StateFSM s;
	s.id = GetNewStateID();

	s.formulae.insert(s.formulae.begin(), leaf.formulae.begin(), leaf.formulae.end());
	leaf.formulae.clear();

	MoveVector(leaf.allocArrays, s.allocArrays);
	MoveVector(leaf.allocPointers, s.allocPointers);
	MoveVector(leaf.delArrays, s.delArrays);
	MoveVector(leaf.delPointers, s.delPointers);

	//Note:This may be parallelized
	SolveRet(true, s);
	SolveRet(false, s);
	
}

bool FSM::MatchEvents(FSMID stateID)
{
	//ConditionStorage events = states[stateID].events;
	int cur = stateID;
	while (!events.empty())
	{
		if (states[cur].incoming.empty()) //start state.
		{
			break;
		}
		auto trID = states[cur].incoming[0]; //in the current model there is one parent of each state.
		TransitionFSM &tr = transitions[trID];
		auto parent = tr.start;
		if (!events.empty())
		{
			if (tr.evt != TransitionFSM::EPSILON)
			{
				if (tr.evt != events.back())
				{
					return false;
				}
			}
		}
	}
	return true;
}
