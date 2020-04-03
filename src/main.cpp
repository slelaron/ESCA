#include <vector>
#include <iostream>
#include <string>
#include <fstream>

#include "ASTWalker.h"
#include "DefectStorage.h"
#include "Output.h"

#include "file.h"

extern Target::Context ctx;
extern std::map<std::string, Target::Function *> allFunctions;
extern std::set<std::string> allocatedFunctions;

extern std::map<std::string, std::string> staticFuncMapping;

std::set<std::string> processedFunctions;

int main(int argc, char **argv) {
    int count = 0;

    std::string resource_path = "../resource/";
    //name = "rtk";
    //name = "Its";
    //name = "gnss";
    //name = "quake";
    //name = "facebook";
    //name = "arduino";
    //name = "stlink";

//    std::fstream fstream(root + name + ".txt", std::ios_base::in);
//    std::string file;
//    while (std::getline(fstream, file)) {
//        walker.WalkAST(root + file);
//        llvm::errs() << "Parsed " << ++count << " files\n";
//    }

    std::cout << "Start walk" <<std::endl;
    std::string test_file = "1.cpp";
    ASTWalker walker;
    walker.WalkAST(resource_path + test_file);

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