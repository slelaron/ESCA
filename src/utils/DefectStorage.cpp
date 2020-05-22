#include <fstream>
#include <llvm/Support/raw_ostream.h>

#include "DefectStorage.h"

using namespace std;

DefectStorage &DefectStorage::Instance()
{
    static DefectStorage instance;
    return instance;
}

void DefectStorage::AddDefect( const string &var, const string &location )
{
    if( !defectsLocations.count(location))
    {
        defectsLocations.insert(location);
        defects.push_back({var, location});
    }
}

void DefectStorage::PrintDefects( const std::string &outputFile )
{
    if( outputFile.empty())
    {
        for( const auto &defect : defects )
        {
            llvm::errs() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~LEAK~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
            llvm::errs() << "Location: " << defect.location << "\n";
            llvm::errs() << "variable name: " << defect.varName << "\n";
        }
        llvm::errs() << "Found " << defects.size() << " leaks\n";
        return;
    }

    ofstream outf(outputFile);
    if( !outf.good())
    {
        llvm::errs() << "Wrong output path: " << outputFile;
        return;
    }

    for( const auto &defect : defects )
    {
        outf << "Resource leak. Variable name: " << defect.varName << " Location: " << defect.location << "\n";
    }
    outf << "Found " << defects.size() << " leaks\n";
    outf.close();
}
