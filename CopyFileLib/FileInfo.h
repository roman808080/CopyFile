#pragma once

#include "FileUtils.h"

struct FileInfo
{
	const std::string filePath;

	const uintmax_t startBlock = 0;
	uintmax_t endBlock = 0;

	const uintmax_t blockSize = Constants::Megabyte;
	const uintmax_t fileSize = FileUtils::getFileSize(filePath);

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
		fileSize(FileUtils::getFileSize(filePath)),
		startPosition(startBlock * blockSize),
		endPosition(endBlock * blockSize)
	{
		if (this->endBlock == 0)
		{
			const auto numberOfBlocks = FileUtils::getPossibleBlocksAmount(filePath, blockSize);
			this->endBlock = numberOfBlocks;
			endPosition = this->endBlock * blockSize;
		}

		if (endPosition > fileSize)
		{
			endPosition = fileSize;
		}
	}
};