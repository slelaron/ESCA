#include <iostream>
#include <string>
#include <fstream>
#include <ctime>

#include "AST/ASTWalker.h"


void usage()
{
    std::cout << "USAGE: ESCA [Options] <source> " << std::endl
              << "\tOptions:\n\t[-f|--files] <text file with exec files>" << std::endl
              << "\t[-c|--cmake] <cmake json file>" << std::endl;
}

void parseFile( std::vector<std::string> &files, const std::string &file, bool isCmake )
{
    std::ifstream fs(file);
    if( !fs.is_open())
    {
        std::cerr << "can't open file: " << file << std::endl;
        exit(1);
    }
    std::string line;
    while( getline(fs, line))
    {
        if( isCmake )
        {
            int s = line.find("\"file\"");
            if( s != std::string::npos )
            {
                line = line.substr(s + 9, line.length() - s - 10);
                files.push_back(line);
            }
        }
        else
        {
            files.push_back(line);
        }
    }
}

void parseArgs( std::vector<std::string> &files, std::string &path, int argc, char **argv )
{
    if( argc == 2 && argv[ 1 ][ 0 ] != '-' )
    {
        files.emplace_back(argv[ 1 ]);
        return;
    }
    if( argc <= 2 || std::string(argv[ 1 ]) == "-h" || std::string(argv[ 1 ]) == "--help" )
    {
        std::cout << "not enough arguments" << std::endl;
        usage();
        exit(0);
    }
    auto argv1 = std::string(argv[ 1 ]);
    if( argv1 == "--files" || argv1 == "-f" )
    {
        parseFile(files, argv[ 2 ], false);
    }
    else if( argv1 == "-c" || argv1 == "--cmake" )
    {
        parseFile(files, argv[ 2 ], true);
    }
    else
    {
        std::cerr << "undefined args" << std::endl;
        exit(1);
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

    walker.RunAnalyzer();

//    unsigned int end_time = clock();
//    unsigned int search_time = end_time - start_time;
    std::cout << "Working time:" << clock() / CLOCKS_PER_SEC << "sec" << std::endl;

    return 0;
}