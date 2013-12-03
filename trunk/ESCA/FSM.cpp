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

using namespace std;

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
				{
					//write formulae.
					int size = del.allocPointers.size();
					//unsat => delete is correct
					for (int i = 0; i < size; ++i)
					{
						shared_ptr<BinarySMT> form(new BinarySMT(v, del.allocPointers[i], EqualSMT, true));
						del.formulae.push_back(form);
					}

					stringstream ss;
					ss << "form" << iSat << ".sat";
					ofstream outf(ss.str());
					outf << del.PrintFormulae();
					outf.close();

					++iSat;
				}
				break;
			case VAR_ARRAY_POINTER:
				{
					//write formulae.
					int size = del.allocArrays.size();
					//unsat => delete is correct
					for (int i = 0; i < size; ++i)
					{
						shared_ptr<BinarySMT> form(new BinarySMT(v, del.allocArrays[i], EqualSMT, true));
						del.formulae.push_back(form);
					}

					stringstream ss;
					ss << "form" << iSat << ".sat";
					ofstream outf(ss.str());
					outf << del.PrintFormulaeSat();
					outf.close();

					auto solve = ExecSolver::Run(ss.str());

					if (solve.find("unsat") != -1) //unsat
					{
						llvm::errs() << "Correct delete\n";
					}


					++iSat;
				}
				break;
			default:
				break;
			}
		}
	}
}
