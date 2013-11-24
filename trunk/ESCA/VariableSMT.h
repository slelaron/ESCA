#ifndef VARIABLESMT_H
#define VARIABLESMT_H

#include "FormulaSMT.h"
#include "VersionedVariable.h"

class VariableSMT : public FormulaSMT
{
	public:
		virtual std::string FormatSMTLIB();
	private:
		VersionedVariable var;
};

#endif
