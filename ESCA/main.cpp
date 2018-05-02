#include <vector>
#include <iostream>
#include <string>
#include <fstream>

#include "ASTWalker.h"
#include "DefectStorage.h"
#include "Output.h"

#include "file.h"

#include <llvm/Support/raw_ostream.h>

using namespace std;

extern Target::Context ctx;
extern std::map<std::string, Target::Function*> allFunctions;
extern std::set<std::string> allocatedFunctions;

extern std::map<std::string, std::string> staticFuncMapping;

std::set<std::string> processedFunctions;

int main(int argc, char **argv)
{
	ASTWalker walker;
    int count = 0;

    std::string root = "C:\\Users\\Yuriy\\Desktop\\Diplom 2018\\tests\\test\\";
    std::string name;
    //name = "rtk";
    //name = "Its";
    //name = "gnss";
    //name = "quake";
    //name = "facebook";
    //name = "arduino";
    name = "stlink";

    //std::fstream fstream("C:\\Users\\Yuriy\\Desktop\\Diplom 2018\\tests\\test\\" + name + ".txt", std::ios_base::in);
    //std::string file;
    //while (std::getline(fstream, file)) {
    //    walker.WalkAST(root + file);
    //    llvm::errs() << "Parsed " << ++count << " files\n";
    //}

    walker.WalkAST("C:\\Users\\Yuriy\\Desktop\\Diplom 2018\\tests\\leak2\\leak2\\1.cpp");

    Cout << "---------------------------------------";

    allocatedFunctions.clear();
    allocatedFunctions.insert(std::string("malloc"));

    count = 0;
    for (auto p : allFunctions) {
        staticFuncMapping.clear();
        p.second->process();
    }

	DefectStorage::Instance().SaveDefects();

	return 0;
}