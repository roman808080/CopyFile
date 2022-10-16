#include "pch.h"
#include "InputFile.h"

#include <filesystem>
#include <cmath>

#include "SharedMemoryBuffer.h"

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

InputFile::InputFile(const std::string& fileName,
					 const size_t blockSize)
	: inputFile(fileName, std::ifstream::binary)
	, blockSize(blockSize)
	, fileSize(getFileSize(fileName))
{
	if (!inputFile.is_open())
	{
		throw std::runtime_error("Failed to open the file.");
	}

	if (fileSize == 0)
	{
		throw std::runtime_error("End position is equel to zero.");
	}
}

InputFile::~InputFile()
{
	inputFile.close();
}

uintmax_t InputFile::calculateBlockSize()
{
	if ((getCurrentPosition() + blockSize) > fileSize)
	{
		return fileSize - getCurrentPosition();
	}

	return blockSize;
}

void InputFile::readBlock(Chunk& block)
{
	block.size = calculateBlockSize();
	if (block.size == 0)
	{
		throw std::runtime_error("Nothing to read.");
	}

	inputFile.read(block.startPosition, block.size);
}

void InputFile::readBlock(Block* block)
{
	block->size = calculateBlockSize();
	if (block->size == 0)
	{
		throw std::runtime_error("Nothing to read.");
	}

	inputFile.read(block->buffer, block->size);
}

bool InputFile::isFinished()
{
	if (inputFile.eof())
	{
		return true;
	}

	if (getCurrentPosition() > fileSize)
	{
		throw std::runtime_error("A wrong position in the input file.");
	}

	if (getCurrentPosition() == fileSize)
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
