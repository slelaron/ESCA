#ifndef EQUALSMT_H
#define EQUALSMT_H

#include "FormulaSMT.h"
#include "VersionedVariable.h"

enum OperatorSMT
{
	EqualSMT,
	LessSMT,
	GreaterSMT
};

class BinarySMT : FormulaSMT
{
	public:
		virtual std::string FormatSMTLIB();
	private:
		VersionedVariable lhs;
		VersionedVariable rhs;
		OperatorSMT op;
		bool negation;
};

#endif
