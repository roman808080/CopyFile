// CopyFile.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include "OutputFile.h"
#include "InputFile.h"
#include "FileUtils.h"
#include "FileInfo.h"

#include "ThreadsafeQueue.h"

int main()
{
    OutputFile outputFile("file.txt");

    const std::string filePath("CopyFile.cpp"); // copying itself
    const size_t blockSize = Constants::Kilobyte;
    const auto numberOfBlocks = FileUtils::getPossibleBlocksAmount(filePath, blockSize);

    const size_t startBlock = 0;
    const uintmax_t endBlock = numberOfBlocks;

    auto fileInfo = std::make_unique<FileInfo>(filePath, startBlock, endBlock, blockSize);
    auto file = std::make_unique<InputFile>(std::move(fileInfo));

    ThreadsafeQueue<std::vector<char> > queue;
    while (!file->isFinished())
    {
        std::vector<char> block = file->readBlock();
        queue.push(block);
    }

    while (!queue.empty())
    {
        std::unique_ptr<std::vector<char>> block = std::move(queue.tryPop());

        outputFile.write(*block);
    }

    return 0;
}
