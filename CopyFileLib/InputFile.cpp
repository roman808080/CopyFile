#include "pch.h"
#include "InputFile.h"

#include <filesystem>

#include "Exceptions.h"
#include "FileInfo.h"


InputFile::InputFile(std::unique_ptr<FileInfo> fileInfo)
	: inputFile(fileInfo->filePath, std::ifstream::binary),
	fileInfo(std::move(fileInfo))
{
	if (!inputFile.is_open())
	{
		throw FileException("Failed to open the file.");
	}

	inputFile.seekg(this->fileInfo->startPosition, inputFile.beg);
	if (this->fileInfo->endPosition == 0)
	{
		throw FileException("End position is equel to zero.");
	}
}

InputFile::~InputFile()
{
	inputFile.close();
}

uintmax_t InputFile::size()
{
	return fileInfo->fileSize;
}

uintmax_t InputFile::calculateBlockSize()
{
	if ((getCurrentPosition() + fileInfo->blockSize) > fileInfo->endPosition)
	{
		return fileInfo->endPosition - getCurrentPosition();
	}

	return fileInfo->blockSize;
}

std::unique_ptr<std::vector<char>> InputFile::readBlock()
{
	auto currentBlockSize = calculateBlockSize();
	if (currentBlockSize == 0)
	{
		throw EmptyBlock("Nothing to read.");
	}

	std::unique_ptr<std::vector<char>> fileContent(std::make_unique<std::vector<char>>());
	fileContent->resize(static_cast<size_t>(currentBlockSize));

	inputFile.read(&(*fileContent)[0], currentBlockSize);

	return std::move(fileContent);
}

bool InputFile::isFinished()
{
	if (inputFile.eof())
	{
		return true;
	}

	if (getCurrentPosition() > fileInfo->endPosition)
	{
		throw FileException("A wrong position in the input file.");
	}

	if (getCurrentPosition() == fileInfo->endPosition)
	{
		return true;
	}

	return false;
}

uintmax_t InputFile::getCurrentPosition()
{
	const std::streamoff currentPosition = inputFile.tellg();

	if (currentPosition == -1)
	{
		throw FileException("Failed to get the current position.");
	}

	return static_cast<const uintmax_t>(currentPosition);
}
