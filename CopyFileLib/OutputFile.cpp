#include "pch.h"
#include "OutputFile.h"

OutputFile::OutputFile(const std::string& fileName)
	: outputFile(fileName, std::ofstream::binary)
{
	if (!outputFile.is_open())
	{
		throw std::runtime_error("An output file cannot be opened");
	}
}

OutputFile::~OutputFile()
{
	outputFile.close();
}

void OutputFile::write(const Chunk block)
{
	outputFile.write(block.startPosition, block.size);
	outputFile.flush();
}
