#include "pch.h"
#include "InputFile.h"

#include <filesystem>
#include "Exceptions.h"

uintmax_t FileUtils::getFileSize(const std::string& filePath)
{
	try
	{
		return std::filesystem::file_size(filePath);
	}
	catch (std::filesystem::filesystem_error& error)
	{
		throw FileException(error.what());
	}
}

uintmax_t FileUtils::getPossibleBlocksAmount(const std::string& filePath, const size_t blockSize)
{
	return static_cast<uintmax_t>(std::ceil(FileUtils::getFileSize(filePath) * 1.0 / blockSize));
}



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

std::vector<char> InputFile::readAll()
{
	try
	{
		std::vector<char> fileContent;

		fileContent.resize(static_cast<size_t>(fileInfo->fileSize));
		fileContent.assign((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());

		return fileContent;
	}
	catch (std::exception& error)
	{
		throw FileException(error.what());
	}
}

std::vector<char> InputFile::readBlock()
{
	auto currentBlockSize = calculateBlockSize();
	if (currentBlockSize == 0)
	{
		throw EmptyBlock("Nothing to read.");
	}

	std::vector<char> fileContent;
	fileContent.resize(static_cast<size_t>(currentBlockSize));
	inputFile.read(&fileContent[0], currentBlockSize);

	return fileContent;
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
