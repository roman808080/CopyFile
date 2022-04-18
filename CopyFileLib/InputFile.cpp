#include "pch.h"
#include "InputFile.h"

#include <filesystem>

namespace
{
	uintmax_t getFileSize(const std::string& filePath)
	{
		return std::filesystem::file_size(filePath);
	}

	uintmax_t getPossibleBlocksAmount(const std::string& filePath, const size_t blockSize)
	{
		return static_cast<uintmax_t>(std::ceil(getFileSize(filePath) * 1.0 / blockSize));
	}
}

struct FileInfo
{
	const uintmax_t blockSize = Constants::Kilobyte;
	const uintmax_t fileSize = 0;

	FileInfo(const std::string& filePath,
			 const uintmax_t blockSize = Constants::Kilobyte)
		: blockSize(blockSize)
		, fileSize(getFileSize(filePath))
	{
	}
};


InputFile::InputFile(const std::string& fileName,
					 const size_t blockSize)
	: inputFile(fileName, std::ifstream::binary),
	  fileInfo(std::make_unique<FileInfo>(fileName, blockSize))
{
	if (!inputFile.is_open())
	{
		throw std::runtime_error("Failed to open the file.");
	}

	if (this->fileInfo->fileSize == 0)
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
	if ((getCurrentPosition() + fileInfo->blockSize) > fileInfo->fileSize)
	{
		return fileInfo->fileSize - getCurrentPosition();
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

	if (getCurrentPosition() > fileInfo->fileSize)
	{
		throw std::runtime_error("A wrong position in the input file.");
	}

	if (getCurrentPosition() == fileInfo->fileSize)
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
