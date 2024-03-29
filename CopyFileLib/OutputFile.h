#pragma once
#include <fstream>
#include <vector>

#include "Constants.h"
#include "Router.h"

struct Block;

class OutputFile
{
public:
	OutputFile(const std::string& fileName);
	~OutputFile();

	OutputFile(const OutputFile&) = delete;
    OutputFile& operator=(const OutputFile&) = delete;
    OutputFile(OutputFile&&) = delete;
    OutputFile& operator=(OutputFile&&) = delete;

	void write(const Chunk block);
	void write(const Block* block);

private:
	std::ofstream outputFile;
};
