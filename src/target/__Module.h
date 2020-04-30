/// unused file
#include "Function.h"

namespace Target
{
class Module
{
public:

    Function *addFoo()
    {
        extFunctions.push_back(new Function());
        return extFunctions.back();
    }

private:
    std::vector<Function *> extFunctions;
    std::vector <Function> staticFunctions;
};
}