#ifndef EXEC_SOLVER
#define EXEC_SOLVER

#include <string>

//TODO: Write execution using http://stackoverflow.com/questions/478898/how-to-execute-a-command-and-get-output-of-command-within-c 

class ExecSolver
{
	public:
		static std::string Run(std::string file);
	private:
		static const std::string CommandLine;
};

#endif
