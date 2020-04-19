#include <iostream>
#include <string>

#include "AST/ASTWalker.h"
#include "target/Function.h"


void usage()
{
    std::cout << "USAGE: ESCA [-f|--files] <source0> [... <sourceN>]" << std::endl;
}

void parseArgs( std::vector<std::string> &files, int argc, char **argv )
{
    if( argc < 2 || std::string(argv[ 1 ]) == "-h" || std::string(argv[ 1 ]) == "--help" )
    {
        usage();
        exit(0);
    }
    auto argv1 = std::string(argv[ 1 ]);
    if( argv1 == "--files" || argv1 == "-f" )
    {
        for( int i = 2; i < argc; ++i )
        {
            files.emplace_back(argv[ i ]);
        }
    }
    else
    {
        files.push_back(argv1);
    }
}


int main( int argc, char **argv )
{
    std::vector<std::string> files;
    parseArgs(files, argc, argv);
    ASTWalker walker;
#ifdef __linux__
    std::vector<std::string> paths = {
            "/usr/include/",
            "/usr/include/c++/9/",
            "/usr/include/x86_64-linux-gnu/",
            "/usr/include/x86_64-linux-gnu/c++/9/",
            "/usr/lib/gcc/x86_64-linux-gnu/9/include/",
            "/usr/lib/llvm-10/include/",
    };
    walker.SetIncludeDirectories(paths);
#endif

    for( const auto &file : files )
    {
        std::cout << "Start walk for " << file << std::endl;
        if( !walker.WalkAST(file))
        {
            std::cerr << "Failed to walk for file: " << file << std::endl;
        }
    }

    std::cout << "---------------------------------------" << std::endl;

    //    allocatedFunctions.clear();
    //    allocatedFunctions.insert(std::string("new"));

    for( auto p : allFunctions )
    {
        p.second->process();
    }

    return 0;
}