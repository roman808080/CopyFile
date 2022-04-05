#pragma once

#include <fstream>
#include <vector>

#include "Constants.h"

struct FileInfo;

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