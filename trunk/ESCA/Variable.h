#ifndef VARIABLE_H
#define VARIABLE_H

#include <string>

enum EMetaType
{
	VAR_COMMON,
	VAR_POINTER,
	VAR_ARRAY_POINTER
};

class Variable
{
	public:
		Variable() {}
		Variable(const Variable &v) { name = v.name; type = v.type; metaType = v.metaType; }
		Variable (std::string t, std::string n, EMetaType mt) : name(n), type(t) { metaType = mt;}

		inline std::string Name() const { return name; }
		inline std::string Type() const { return type; }
		inline EMetaType MetaType() const { return metaType; }
	protected:
		std::string name;
		std::string type;
		EMetaType metaType;
};

#endif
