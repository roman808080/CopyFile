#include "pch.h"
#include "FileUtils.h"

#include <string>
#include <filesystem>

uintmax_t FileUtils::getFileSize(const std::string& filePath)
{
	return std::filesystem::file_size(filePath);
}

uintmax_t FileUtils::getPossibleBlocksAmount(const std::string& filePath, const size_t blockSize)
{
	return static_cast<uintmax_t>(std::ceil(FileUtils::getFileSize(filePath) * 1.0 / blockSize));
}
