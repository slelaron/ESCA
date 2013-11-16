#include <vector>
#include <iostream>
#include <string>

#include "ASTWalker.h"

using namespace std;

int main(int argc, char **argv)
{
	ASTWalker walker;
	if (argc > 1)
	{
		walker.WalkAST(argv[1]);
	}
	else
	{
		walker.WalkAST();
	}
	return 0;
}