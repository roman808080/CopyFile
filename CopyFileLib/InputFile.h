#pragma once

#include <fstream>
#include <vector>
#include <memory>

#include "Constants.h"

struct FileInfo;

class InputFile
{
public:
	InputFile(std::unique_ptr<FileInfo> fileInfo);
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
	std::ifstream inputFile;
	std::unique_ptr<FileInfo> fileInfo;
};