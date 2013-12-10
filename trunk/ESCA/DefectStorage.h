#ifndef DEFECT_STORAGE_H
#define DEFECT_STORAGE_H

#include <vector>
#include <string>

//Singleton
class DefectStorage
{
	private:
		DefectStorage() {}
		DefectStorage(const DefectStorage &inst);
		DefectStorage &operator =(const DefectStorage &rhs);
	public:
		static DefectStorage &Instance();

		std::vector<std::string> Defects() const { return defects; }
		void Defects(const std::vector<std::string> &val) { defects = val; }
		void AddDefect(const std::string &d) { defects.push_back(d); }

		std::string OutputPath() const { return outputPath; }
		void OutputPath(const std::string &val) { outputPath = val; }
	private: 
		std::vector<std::string> defects;
		std::string outputPath;
	public:
		void SaveDefects();
};
#endif
