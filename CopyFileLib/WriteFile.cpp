#include "pch.h"
#include "WriteFile.h"
#include "FileInfo.h"
#include "Exceptions.h"

WriteFile::WriteFile(const std::string& fileName)
	: outputFile(fileName, std::ofstream::binary)
{
	if (!outputFile.is_open())
	{
		throw FileException("An output file cannot be opened");
	}
}

WriteFile::~WriteFile()
{
	outputFile.close();
}

void WriteFile::write(const std::vector<char>& block)
{
	outputFile << &block[0];
	outputFile.flush();
}
