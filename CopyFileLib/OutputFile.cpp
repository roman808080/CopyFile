#include "pch.h"
#include "OutputFile.h"
#include "FileInfo.h"
#include "Exceptions.h"

OutputFile::OutputFile(const std::string& fileName)
	: outputFile(fileName, std::ofstream::binary)
{
	if (!outputFile.is_open())
	{
		throw FileException("An output file cannot be opened");
	}
}

OutputFile::~OutputFile()
{
	outputFile.close();
}

void OutputFile::write(std::vector<char>* block)
{
	outputFile.write(&(*block)[0], block->size());
	outputFile.flush();
}
