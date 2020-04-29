#include <iostream>

#include "AST/ASTWalker.h"

const std::vector<std::string> INCLUDE_PATHS = {
        "/usr/include/",
        "/usr/include/c++/9/",
        "/usr/include/x86_64-linux-gnu/",
        "/usr/include/x86_64-linux-gnu/c++/9/",
        "/usr/lib/gcc/x86_64-linux-gnu/9/include/",
        "/usr/lib/llvm-10/include/",
};

const std::string RESOURCE_PATH = "/home/alex/CLionProjects/ESCA/resource/";

int main()
{
    std::vector<std::string> files = {"test2.cpp"};
    ASTWalker walker;
    walker.SetIncludeDirectories(INCLUDE_PATHS);
    for( const auto& file: files )
    {
        walker.WalkAST(RESOURCE_PATH + file);
    }

    walker.RunAnalyzer();

    return 0;
}