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
	const std::string filePath;

	const uintmax_t startBlock = 0;
	uintmax_t endBlock = 0;

	const uintmax_t blockSize = Constants::Megabyte;
	const uintmax_t fileSize = getFileSize(filePath);

	const uintmax_t startPosition = startBlock * blockSize;
	uintmax_t endPosition = endBlock * blockSize;

	FileInfo(const std::string& filePath,
		const uintmax_t blockSize = Constants::Megabyte,
		const uintmax_t startBlock = 0,
		const uintmax_t endBlock = 0)
		: filePath(filePath),
		startBlock(startBlock),
		endBlock(endBlock),
		blockSize(blockSize),
		fileSize(getFileSize(filePath)),
		startPosition(startBlock * blockSize),
		endPosition(endBlock * blockSize)
	{
		if (this->endBlock == 0)
		{
			const auto numberOfBlocks = getPossibleBlocksAmount(filePath, blockSize);
			this->endBlock = numberOfBlocks;
			endPosition = this->endBlock * blockSize;
		}

		if (endPosition > fileSize)
		{
			endPosition = fileSize;
		}
	}
};


InputFile::InputFile(const std::string& fileName)
	: inputFile(fileName, std::ifstream::binary),
	  fileInfo(std::make_unique<FileInfo>(fileName))
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
