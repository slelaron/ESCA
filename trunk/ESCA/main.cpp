#include <vector>
#include <iostream>
#include <string>

#include "ASTWalker.h"

using namespace std;

int main(int argc, char **argv)
{
	ASTWalker walker;
	walker.WalkAST();
	return 0;
}