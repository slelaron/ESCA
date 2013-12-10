#include "StateFSM.h"

StateFSM::~StateFSM()
{
}

std::string FormulaeToString(const std::deque<std::shared_ptr<FormulaSMT> > &formulae)
{
	auto size = formulae.size();
	std::string res("");
	for (int i = 0; i < size; ++i)
	{
		res += formulae[i]->FormatSMTLIB();
	}
	return res;
}
std::string FormulaeToStringSat(const std::deque<std::shared_ptr<FormulaSMT> > &formulae)
{
	return FormulaeToString(formulae) + "(check-sat)\n";
}


std::string StateFSM::PrintFormulae()
{
	return FormulaeToString(formulae);
}

std::string StateFSM::PrintFormulaeSat()
{
	return FormulaeToStringSat(formulae);
}


bool operator ==(const StateFSM &lhs, const StateFSM &rhs)
{
	return lhs.id == rhs.id;
}

bool operator <(const StateFSM &lhs, const StateFSM &rhs)
{
	return lhs.id < rhs.id;
}
