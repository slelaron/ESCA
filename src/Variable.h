#ifndef VARIABLE_H
#define VARIABLE_H

#include <string>

enum EMetaType
{
    VAR_COMMON,
    VAR_POINTER,
    VAR_ARRAY_POINTER,
    VAR_UNKNOWN,
};

struct PtrCounter
{
    int count;
    EMetaType meta;
};

class Variable
{
public:
    Variable() = default; // TODO: make it delete

    Variable( const Variable &v ) = default;

    Variable( const std::string &type, const std::string &name, const std::string &loc, EMetaType mt )
            : name(name), type(type), loc(loc), metaType(mt)
    {
    }

    inline std::string Name() const
    {
        return name;
    }

    inline std::string Type() const
    {
        return type;
    }

    inline EMetaType MetaType() const
    {
        return metaType;
    }

    inline void MetaType( EMetaType mt )
    {
        metaType = mt;
    }

    std::string getLocation() const
    {
        return loc;
    }

    void setLocation( const std::string &l )
    {
        loc = l;
    }

    std::string TypeSMT() const;

protected:
    std::string name;
    std::string type;
    std::string loc;
    EMetaType metaType;
};

#endif
