#include "ESCAASTConsumer.h"

using namespace llvm;
using namespace clang;

bool ESCAASTConsumer::HandleTopLevelDecl(DeclGroupRef DR)
{
    for (DeclGroupRef::iterator b = DR.begin(), e = DR.end();
                b != e; ++b)
        // Traverse the declaration using our AST visitor.
        visitor.TraverseDecl(*b);
    return true;
}