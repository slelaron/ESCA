#include <iostream>
#include <string>
#include <fstream>
#include "AST/ASTWalker.h"


void usage()
{
    std::cout << "USAGE: ESCA [-f|--files] <source0> [... <sourceN>]" << std::endl
              << "\t[-p|--path] <path>" << std::endl
              << "\t[-c|--cmake] <cmake json file>" << std::endl;
}

void parseCmake( std::vector<std::string> &files, const std::string &json )
{
    std::ifstream js(json);
    if( !js.is_open())
    {
        std::cerr << "can't open file" << json << std::endl;
        exit(1);
    }
    std::string line;
    while( getline(js, line))
    {
        int s = line.find("\"file\"");
        if( s != std::string::npos )
        {
            line = line.substr(s + 9, line.length() - s - 10);
            files.push_back(line);
        }
    }
}

bool walkAlone = false;

void parseArgs( std::vector<std::string> &files, std::string &path, int argc, char **argv )
{
    if( argc <= 2 || std::string(argv[ 1 ]) == "-h" || std::string(argv[ 1 ]) == "--help" )
    {
        std::cout << "not enough arguments" << std::endl;
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
    else if( argv1 == "-p" || argv1 == "--path" )
    {
        path = argv[ 2 ];
        //Todo : walk dir and detect files
    }
    else if( argv1 == "-c" || argv1 == "--cmake" )
    {
        parseCmake(files, argv[ 2 ]);
    }
}


int main( int argc, char **argv )
{
    std::vector<std::string> files;
    std::string path;
    parseArgs(files, path, argc, argv);
    ASTWalker walker;
#ifdef __unix__
// TODO: autodetect it or let user do it
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
        std::cout << "Start analyze for " << file << std::endl;
        if( !walker.WalkAST(file))
        {
            std::cerr << "Failed to analyze for file: " << file << std::endl;
        }
    }

    std::cout << "---------------------------------------" << std::endl;

    for( auto p : allFunctions )
    {
        p.second->process();
    }

    return 0;
}