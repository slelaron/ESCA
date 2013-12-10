#include <fstream>

#include "DefectStorage.h"

using namespace std;

DefectStorage &DefectStorage::Instance()
{
	static DefectStorage instance;
	return instance;
}

void DefectStorage::SaveDefects()
{
	if (outputPath == "")
	{
		return;
	}

	ofstream outf(outputPath);
	if (!outf.good())
	{
		return;
	}

	auto size = defects.size();
	for (int i = 0; i < size; ++i)
	{
		outf << defects[i] << endl;
	}

	outf.close();
}
