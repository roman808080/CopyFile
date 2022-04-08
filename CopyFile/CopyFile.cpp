// CopyFile.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <thread>

#include "OutputFile.h"
#include "InputFile.h"
#include "FileUtils.h"
#include "FileInfo.h"

#include "ThreadsafeQueue.h"

namespace
{
    void readFromFile(std::shared_ptr<InputFile> inputFile,
                    std::shared_ptr<ThreadsafeQueue<std::vector<char>>> queue)
    {
		while (!inputFile->isFinished())
		{
			auto block = std::move(inputFile->readBlock());
			queue->push(std::move(block));
		}

        queue->finalize();
    }

    void writeToFile(std::shared_ptr<OutputFile> outputFile,
        std::shared_ptr<ThreadsafeQueue<std::vector<char>>> queue)
    {
		while (!queue->isFinished())
		{
			auto block = std::move(queue->waitAndPop());
			outputFile->write(std::move(block));
		}
    }
}

int main()
{
    std::shared_ptr<OutputFile> outputFile(std::make_shared<OutputFile>("file.txt"));

    const std::string filePath("CopyFile.cpp"); // copying itself
    const size_t blockSize = Constants::Kilobyte;
    const auto numberOfBlocks = FileUtils::getPossibleBlocksAmount(filePath, blockSize);

    const size_t startBlock = 0;
    const uintmax_t endBlock = numberOfBlocks;

    auto fileInfo = std::make_unique<FileInfo>(filePath,blockSize, startBlock, endBlock);
    auto inputFile = std::make_shared<InputFile>(std::move(fileInfo));

    std::shared_ptr<ThreadsafeQueue<std::vector<char>>> queue(std::make_shared<ThreadsafeQueue<std::vector<char>>>());

    std::thread readThread(readFromFile, inputFile, queue);
    std::thread writeThread(writeToFile, outputFile, queue);

    readThread.join();
    writeThread.join();

    return 0;
}
