#pragma once
#include <fstream>
#include <vector>

#include "Constants.h"

struct FileInfo;


class OutputFile
{
public:
	OutputFile(const std::string& fileName);
	~OutputFile();

	OutputFile(const OutputFile&) = delete;
    OutputFile& operator=(const OutputFile&) = delete;
    OutputFile(OutputFile&&) = delete;
    OutputFile& operator=(OutputFile&&) = delete;

	void write(std::unique_ptr<std::vector<char>> block);

private:
	std::ofstream outputFile;
};
