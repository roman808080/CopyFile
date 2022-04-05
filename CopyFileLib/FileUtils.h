#pragma once
#include <string>

namespace FileUtils
{
	uintmax_t getFileSize(const std::string& filePath);
	uintmax_t getPossibleBlocksAmount(const std::string& filePath, const size_t blockSize);
}