#pragma once

#include <fstream>
#include <vector>
#include <memory>

#include "Constants.h"

struct FileInfo;

class InputFile
{
public:
	InputFile(const std::string& fileName, const size_t blockSize = Constants::Kilobyte);
	~InputFile();

	InputFile(const InputFile&) = delete;
    InputFile& operator=(const InputFile&) = delete;
    InputFile(InputFile&&) = delete;
    InputFile& operator=(InputFile&&) = delete;

	void readBlock(std::vector<char>* block);

	bool isFinished();
	uintmax_t getCurrentPosition();

	uintmax_t size();

private:
	uintmax_t calculateBlockSize();

private:
	std::unique_ptr<FileInfo> fileInfo;
	std::ifstream inputFile;
};