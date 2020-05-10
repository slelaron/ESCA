#include <iostream>
#include <utility>

#include "AST/ASTWalker.h"
#include "target/AnalyzeProcess.h"

const std::vector<std::string> INCLUDE_PATHS = {
        "/usr/include/",
        "/usr/include/c++/9/",
        "/usr/include/x86_64-linux-gnu/",
        "/usr/include/x86_64-linux-gnu/c++/9/",
        "/usr/lib/gcc/x86_64-linux-gnu/9/include/",
//        "/usr/lib/llvm-10/include/",
};


int main()
{
    const std::string RESOURCE_PATH = "/home/alex/CLionProjects/ESCA/tests/examples/";
    std::vector<std::string> files = {
            "test2.cpp",
    };
    ASTWalker walker;
    Options::Instance().setIncludeDirs(INCLUDE_PATHS);
    walker.SetIncludeDirectories(INCLUDE_PATHS);
    for( const auto &file: files )
    {
        if( !walker.WalkAST(RESOURCE_PATH + file))
        {
            std::cerr << "Failed to analyze for file: " << file << std::endl;
        }
        std::cout << "finish to analyze file: " << file << std::endl;
    }

    AnalyzeProcess a;
    a.StartAnalyze();
    return 0;
}