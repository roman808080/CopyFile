// CopyFile.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <thread>

#include "OutputFile.h"
#include "InputFile.h"
#include "FileUtils.h"
#include "FileInfo.h"

#include "ThreadsafeQueue.h"
#include "Reader.h"
#include "Writer.h"


int main()
{
    std::shared_ptr<OutputFile> outputFile(std::make_shared<OutputFile>("file.txt"));

    const std::string filePath("CopyFile.cpp"); // copying itself
    const size_t blockSize = Constants::Kilobyte;
 
    auto fileInfo = std::make_unique<FileInfo>(filePath, blockSize);
    auto inputFile = std::make_shared<InputFile>(std::move(fileInfo));

    std::shared_ptr<ThreadsafeQueue<std::vector<char>>> queue(std::make_shared<ThreadsafeQueue<std::vector<char>>>());

    std::shared_ptr<Reader> reader(std::make_shared<Reader>(inputFile, queue));
    std::jthread readThread([reader]() {
			reader->read();
        });

    std::shared_ptr<Writer> writer(std::make_shared<Writer>(outputFile, queue));
    std::jthread writeThread([writer]() {
            writer->write();
        });

    return 0;
}
