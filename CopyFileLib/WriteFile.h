#pragma once
#include <fstream>
#include <vector>

#include "Constants.h"

struct FileInfo;


class WriteFile
{
public:
	WriteFile(const std::string& fileName);
	~WriteFile();

	void write(const std::vector<char>& block);

private:
	std::ofstream outputFile;
};
