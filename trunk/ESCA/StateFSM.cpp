#include "StateFSM.h"

StateFSM::~StateFSM()
{
}

std::string StateFSM::PrintFormulae()
{
	auto size = formulae.size();
	std::string res("");
	for (int i = 0; i < size; ++i)
	{
		res += formulae[i]->FormatSMTLIB();
	}
	return res;
}

std::string StateFSM::PrintFormulaeSat()
{
	return PrintFormulae() + "(check-sat)\n";
}


bool operator ==(const StateFSM &lhs, const StateFSM &rhs)
{
	return lhs.id == rhs.id;
}

bool operator <(const StateFSM &lhs, const StateFSM &rhs)
{
	return lhs.id < rhs.id;
}
