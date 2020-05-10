//
// Created by alex on 03.05.2020.
//

#include "common.h"


void Options::setIncludeDirs( const std::vector<std::string> &inclPaths )
{
    includeDirs = inclPaths;
}

bool Options::isInIncludeDirs( const std::string &file )
{
    for( const auto &path: includeDirs )
    {
        if( file.find(path) != std::string::npos )
        {
            return true;
        }
    }
    return false;
}
