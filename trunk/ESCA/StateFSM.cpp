#include "StateFSM.h"

StateFSM::~StateFSM()
{
	int sf = formulae.size();
	for (int i = 0; i < sf; ++i)
	{
		delete formulae[i];
		formulae[i] = 0; //For safe deletion from other states;
	}
}

bool operator ==(const StateFSM &lhs, const StateFSM &rhs)
{
	return lhs.id == rhs.id;
}

bool operator <(const StateFSM &lhs, const StateFSM &rhs)
{
	return lhs.id < rhs.id;
}
