#ifndef VERSIONED_VARIABLE_H
#define VERSIONED_VARIABLE_H

#include <sstream>

#include "Variable.h"

class VersionedVariable : public Variable
{
public:
    VersionedVariable() = default; // TODO : make it delete

    VersionedVariable( const VersionedVariable &vv ) = default;

    VersionedVariable( const std::string &t, const std::string &n, const std::string &l, EMetaType mt, int ver )
            : Variable(t, n, l, mt), version(ver)
    {
    }

    inline int Version() const
    {
        return version;
    }

    inline std::string VersionedName() const
    {
        return name + "!" + std::to_string(version);
    }

private:
    int version;
};

bool operator==( const VersionedVariable &lhs, const VersionedVariable &rhs );

bool operator<( const VersionedVariable &lhs, const VersionedVariable &rhs );

#endif
