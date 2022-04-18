#include "pch.h"
#include "InputFile.h"

#include <filesystem>

#include "FileInfo.h"


InputFile::InputFile(std::unique_ptr<FileInfo> fileInfo)
	: inputFile(fileInfo->filePath, std::ifstream::binary),
	fileInfo(std::move(fileInfo))
{
	if (!inputFile.is_open())
	{
		throw std::runtime_error("Failed to open the file.");
	}

	inputFile.seekg(this->fileInfo->startPosition, inputFile.beg);
	if (this->fileInfo->endPosition == 0)
	{
		throw std::runtime_error("End position is equel to zero.");
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

void InputFile::readBlock(std::vector<char>* block)
{
	auto currentBlockSize = calculateBlockSize();
	if (currentBlockSize == 0)
	{
		throw std::runtime_error("Nothing to read.");
	}

	block->resize(static_cast<size_t>(currentBlockSize));
	inputFile.read(&(*block)[0], currentBlockSize);
}

bool InputFile::isFinished()
{
	if (inputFile.eof())
	{
		return true;
	}

	if (getCurrentPosition() > fileInfo->endPosition)
	{
		throw std::runtime_error("A wrong position in the input file.");
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
		throw std::runtime_error("Failed to get the current position.");
	}

	return static_cast<const uintmax_t>(currentPosition);
}
