#include <cstdio>
#include <cstdlib>

#include "ExecSolver.h"

using namespace std;

const std::string ExecSolver::CommandLine("z3 /smt2 ");

string ExecSolver::Run(std::string file)
{
	string cmd = CommandLine + file;
    FILE* pipe = _popen(cmd.c_str(), "r");
    if (!pipe) return "";
    char buffer[128];
    std::string result = "";
    while(!feof(pipe)) 
	{
    	if(fgets(buffer, 128, pipe) != NULL)
		{
    		result += buffer;
		}
    }
    _pclose(pipe);
	return result;
}
