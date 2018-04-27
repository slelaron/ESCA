#ifndef EXEC_SOLVER
#define EXEC_SOLVER

#include <string>

class ExecSolver
{
	public:
		static std::string Run(std::string file);
	private:
		static const std::string CommandLine;
};

#endif
