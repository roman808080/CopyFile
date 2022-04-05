#pragma once

#include <fstream>
#include <vector>

#include "Constants.h"

namespace FileUtils
{
	uintmax_t getFileSize(const std::string& filePath);
	uintmax_t getPossibleBlocksAmount(const std::string& filePath, const size_t blockSize);
}

struct FileInfo
{
	const std::string filePath;

	const uintmax_t startBlock = 0;
	const uintmax_t endBlock = 0;

	const uintmax_t blockSize = Constants::Megabyte;
	const uintmax_t fileSize = FileUtils::getFileSize(filePath);

	const uintmax_t startPosition = startBlock * blockSize;
	uintmax_t endPosition = endBlock * blockSize;

	FileInfo(const std::string& filePath,
		const uintmax_t startBlock,
		const uintmax_t endBlock,
		const uintmax_t blockSize = Constants::Megabyte)
		: filePath(filePath),
		startBlock(startBlock),
		endBlock(endBlock),
		blockSize(blockSize),
		fileSize(FileUtils::getFileSize(filePath)),
		startPosition(startBlock* blockSize),
		endPosition(endBlock* blockSize)
	{
		if (endPosition > fileSize)
		{
			endPosition = fileSize;
		}
	}
};

class InputFile
{
public:
	InputFile(std::unique_ptr<FileInfo> fileInfo);
	~InputFile();

	std::vector<char> readAll();
	std::vector<char> readBlock();

	bool isFinished();
	uintmax_t getCurrentPosition();

	uintmax_t size();

private:
	uintmax_t calculateBlockSize();

private:
	std::ifstream inputFile;
	std::unique_ptr<FileInfo> fileInfo;
};