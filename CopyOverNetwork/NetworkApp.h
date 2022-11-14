#pragma once

#include <string>

#include "Constants.h"

class NetworkApp
{
public:
    NetworkApp(const size_t blockSize = Constants::Kilobyte);

    void run();

    void setInputFile(const std::string &inputFileName);
    void setOutputFile(const std::string &outputFileName);

    void setIsServer(bool isServer);

private:

private:
    std::string inputFileName;
    std::string outputFileName;

    const size_t blockSize;
    bool isServer;
};
