#include <sstream>

#include "BinarySMT.h"

using namespace std;

string BinarySMT::FormatSMTLIB()
{
	string sop;
	switch (op)
	{
	case EqualSMT:
		sop = "= ";
		break;
	case LessSMT:
		sop = "< ";
		break;
	case GreaterSMT:
		sop = "> ";
		break;
	default:
		sop = "= ";
	}

	string negBeg = "";// = 
	string negEnd = "";// = ")";
	if (negation)
	{
		negBeg = "not (";
		negEnd = ")";
	}

	stringstream ss;
	ss << "(assert( " << negBeg << sop << lhs.VersionedName() << " " << rhs.VersionedName() << ")" << negEnd << ")\n";
	return ss.str();
}
