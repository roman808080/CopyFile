#pragma once

#include <fstream>
#include <vector>
#include <memory>

#include "Constants.h"
#include "Router.h"

struct Block;

class InputFile
{
public:
	InputFile(const std::string& fileName, const size_t blockSize = Constants::Kilobyte);
	~InputFile();

	InputFile(const InputFile&) = delete;
    InputFile& operator=(const InputFile&) = delete;
    InputFile(InputFile&&) = delete;
    InputFile& operator=(InputFile&&) = delete;

	void readBlock(Chunk& block);
	void readBlock(Block* block);

	bool isFinished();

private:
	uintmax_t getCurrentPosition();
	uintmax_t calculateBlockSize();

private:
	std::ifstream inputFile;

	const uintmax_t blockSize;
	const uintmax_t fileSize;
};